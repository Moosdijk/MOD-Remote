/* 
MOD Remote. 
Thomas van de Moosdijk

This project was done for MOD Devices, as a final master project for Industrial Design at the Technical University in Eindhoven

The project relies heavily on the MPU9250 library by Brian R taylor,
https://github.com/bolderflight/MPU9250
brian.taylor@bolderflight.com
and the fork by Pat Deegan of psychogenic technologies INC
https://github.com/psychogenic/MPU9250/tree/MadgwickAHRS
Many thanks go out to them for doing the heavy lifting by creating the library and further improving on it.
 */

#include <Arduino.h>
#include <SPI.h>
#include <MPU9250.h>
#include <WiFi.h>
#include <WiFiUdp.h>

/* ***** Configuration ******
 *
 * Short version: get as many samples as reasonable but don't lie to your
 * Madgwick filter...
 *
 * Figure out your effective MFILTER_SAMPLE_FREQ_HZ and set
 * IMU_LOWPASSFILTER_BANDWIDTH accordingly.
 *
 * For example, on an 80MHz ESP32, setting:
 *
 *   IMU_POLL_DELAY_MS              2
 *   MFILTER_SAMPLE_FREQ_HZ         480
 *   IMU_LOWPASSFILTER_BANDWIDTH    MPU9250::DLPF_BANDWIDTH_182HZ
 *
 * worked smashingly, whereas on a 3v3 8MHz Pro Mini:
 *   IMU_POLL_DELAY_MS              5
 *   MFILTER_SAMPLE_FREQ_HZ         100
 *   IMU_LOWPASSFILTER_BANDWIDTH    MPU9250::DLPF_BANDWIDTH_41HZ
 *
 * does a good job, once its settled.
 *
 */

// the SPI CS pin you're using
#define CHIP_SELECT_PIN 15

#define SERIAL_BAUD_RATE 115200

// IMU_POLL_DELAY_MS -- delay between loops/readSensor, in ms.
// this is:
//  - the (approx) inverse of your max sampling frequency...
//    so, e.g. for 2ms: freq = 1000/2 = 500Hz
//              for 5ms: freq = 1000/5 = 200Hz
//  - affects your setting for MFILTER_SAMPLE_FREQ_HZ, below
//  - affect your setting for the DLPF, you should poll/readSensor
//    at least twice as often as whatever is set in there
#define IMU_POLL_DELAY_MS 2

// MFILTER_SAMPLE_FREQ_HZ -- tells the Madgwick filter "how
// long each update applies"...
// this should be set according to whatever is delaying your
// reads and updates to the filter.  In this case, it's mostly the
// IMU_POLL_DELAY_MS above, plus a little fudge factor as we're
// spending time doing the reads and the crunching and the reporting...

// this parameter may require some tweaking... start by taking your nominal
// max frequency based on IMU_POLL_DELAY_MS or your specific circumstances
// and
#define MFILTER_SAMPLE_FREQ_HZ 350

#define IMU_LOWPASSFILTER_BANDWIDTH MPU9250::DLPF_BANDWIDTH_184HZ

// CALIB_DISABLE -- when defined, calibration values are ignored,
// even if *_CALIB_DONE is defined (below).
// #define CALIB_DISABLE

// ACC_CALIB_DONE -- undefine this to force a manual calibration
// of the accelerometer on startup.  You'll have to hold the IMU
// steady in all 6 directions, and when done, take note of the results
// to stick them into the setup()
#define ACC_CALIB_DONE

// MAG_CALIB_DONE -- undefine this to force manual calib for the
// mag sensor on startup.  It's pretty annoying.  You're to gently move
// the IMU around until you've hit max & min on every axis, meaning the
// thing has to eventually point at every direction in the 3-sphere around
// you.
// make note of the output values when it's done, and stick those in the setup()
#define MAG_CALIB_DONE

//CALIBRATE_ENABLE will enable the MODremote to listen for a calibration command from the
//receiver, to calibrate the MODremote while in use.
#define CALIBRATE_ENABLE

//WIFI_ON -- undefine to turn wifi on. For normal operation, WIFI must be on.
#define WIFI_ON

// an MPU9250 object with the MPU-9250 sensor on the hSPI bus and chip select pin 15
SPIClass hSPI(HSPI);
MPU9250 IMU(hSPI, CHIP_SELECT_PIN);

// the Madgwick AHRS filter object
Madgwick AHRSFilter;

//Wifi variables, enter your credentials here
const char *networkName = "RECEIVER SSID";
const char *networkPswd = "PASSWORD";

const char *udpAddress = "192.168.4.1";
const int udpPort = 3333;

char incomingData[255]; //the character array to hold incoming data

//Are we currently connected?
boolean connected = false;

//The udp library class
WiFiUDP udp;

int status; //IMU Status
bool newData = false;

//which pins on the ESP32 are the buttons connected to?
int button1 = 21;
int button2 = 19;
int button3 = 18;

int button1Reading;
int button2Reading;
int button3Reading;

int tStart;                //the time that the connection was made
int sendStartupSignal = 0; //for notifying the base station (and the user) that a connection has been made

// a little container for the AHRS bytes
typedef union {
  struct
  {
    float roll;
    float pitch;
    float yaw;
  };
  uint8_t valArray[sizeof(float) * 3];
} NotifBytesBunch;

NotifBytesBunch AHRSValues;
void filterReady();
void sendValues(int button1, int button2, int button3);
void connectToWiFi(const char *ssid, const char *pwd);
void WiFiEvent(WiFiEvent_t event);
//******************************************************************************************************************************************************************************************************************

void IRAM_ATTR filterReady() //the interrupt handler. It has to be put in RAM on the ESP32 as well as
                             //called before setup(), therefore it's here and preceded by IRAM_ATTR.
{
  newData = true;
}

//******************************************************************************************************************************************************************************************************************
//******************************************************************************************************************************************************************************************************************

void setup()
{
  // begin serial connection
  Serial.begin(SERIAL_BAUD_RATE);

  // begin communication with IMU
  status = IMU.begin();
  if (status < 0)
  {
    Serial.println("IMU initialization unsuccessful");
    Serial.println("Check IMU wiring or try cycling power");
    Serial.print("Status: ");
    Serial.println(status);
    while (1)
    {
    }
  }
  if (status > -1)
  {
    Serial.println("GOT POwER");
  }
  IMU.enableDataReadyInterrupt();                    //The interrupt pin on the MPU9250 is used, so data is only processed when it is available.
  IMU.setAccelRange(MPU9250::ACCEL_RANGE_16G);       //Experimentally, these have been found to be the best values.
  IMU.setGyroRange(MPU9250::GYRO_RANGE_2000DPS);     //Basically, it's full range for both accellerometer and gyroscope.
  IMU.setDlpfBandwidth(IMU_LOWPASSFILTER_BANDWIDTH); //Set the lowpass filter bandwith to the frequency set before.
  IMU.setSrd(1);                                     //Setting SRD to 1 means that the data output rate is 500Hz.

  //declaring button functions
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  pinMode(27, INPUT);                       //pin 27 is connected to the INT pin on the MPU9250
  pinMode(22, OUTPUT);                      //builtin LED
  attachInterrupt(27, filterReady, RISING); //Of course, also the interrupt pin (with which the MPU9250 will let the ESP32 know that data is ready)

#ifdef ACC_CALIB_DONE //if the calibration is done:

#ifndef CALIB_DISABLE
  // params are the bias/scaleFactor reported by calibration step
  IMU.setAccelCalX(0.152247, 0.998807); //sideways, proto2
  IMU.setAccelCalY(0.152247, 0.998807);
  IMU.setAccelCalZ(0, 1);
#endif

//If wifi is on (normal operation):
#ifdef WIFI_ON
  connectToWiFi(networkName, networkPswd);
#endif

#else //if the sensor needs calibrating:
  Serial.println(F("********** ACC calib **************"));

  char *dirs[6] = {"X+", "X-", "Y+", "Y-", "Z+", "Z-"};

  for (uint8_t i = 0; i < 6; i++)
  {
    Serial.print(F("Enter when ready for dir "));
    Serial.print((int)(i + 1));
    Serial.print(' ');
    Serial.print(dirs[i]);
    while (!Serial.available())
    {
      delay(10);
    }

    while (Serial.available())
    {
      Serial.read();
      delay(5);
      Serial.print('.');
    }
    Serial.println();
    IMU.calibrateAccel();
  }

  Serial.println(F("Acc calib done"));

  Serial.println(F("Copy and paste these values below the line: #ifndef CALIB_DISABLE and delete the old vlues"));

  Serial.print(F("X: "));
  Serial.print(IMU.getAccelBiasX_mss(), 6);
  Serial.print('/');
  Serial.println(IMU.getAccelScaleFactorX(), 6);

  Serial.print(F("Y: "));
  Serial.print(IMU.getAccelBiasY_mss(), 6);
  Serial.print('/');
  Serial.println(IMU.getAccelScaleFactorY(), 6);

  Serial.print(F("Z: "));
  Serial.print(IMU.getAccelBiasZ_mss(), 6);
  Serial.print('/');
  Serial.println(IMU.getAccelScaleFactorZ(), 6);

#endif

#ifdef MAG_CALIB_DONE
#ifndef CALIB_DISABLE
//These are the values that I calibrated when creating the original MOD Remote.
  IMU.setMagCalX(5.876961, 0.994304);
  IMU.setMagCalY(33.504135, 1.056313);
  IMU.setMagCalZ(-41.672421, 0.954579);

#endif

#else
  Serial.print(F("CALIB MAG -- move in figure 8s until I say stop!!!"));
  delay(500);
  IMU.calibrateMag();
  Serial.println(F(" done!"));

  Serial.print(F("X: "));
  Serial.print(IMU.getMagBiasX_uT(), 6);
  Serial.print('/');
  Serial.println(IMU.getMagScaleFactorX(), 6);

  Serial.print(F("Y: "));
  Serial.print(IMU.getMagBiasY_uT(), 6);
  Serial.print('/');
  Serial.println(IMU.getMagScaleFactorY(), 6);

  Serial.print(F("Z: "));
  Serial.print(IMU.getMagBiasZ_uT(), 6);
  Serial.print('/');
  Serial.println(IMU.getMagScaleFactorZ(), 6);
#endif
  //start the filter
  AHRSFilter.begin(MFILTER_SAMPLE_FREQ_HZ); //begin the filter
  digitalWrite(5, LOW);                     //flash LED to tell user that setup is done
  delay(500);
  digitalWrite(5, HIGH);
}

//******************************************************************************************************************************************************************************************************************
//******************************************************************************************************************************************************************************************************************

void loop()
{
#ifdef MAG_CALIB_DONE //only when the magnitometer is calibrated, the main loop will run, because otherwise, the serial monitor would get flooded.
#ifdef WIFI_ON
  //First, the loop waits a second to settle.
  if (millis() - tStart >= 1000 && millis() - tStart <= 1020)
  {
    Serial.println(sendStartupSignal);
    sendStartupSignal++;
  }
  if (sendStartupSignal > 30 && sendStartupSignal < 32)
  {
    Serial.println("connected");
    udp.beginPacket(udpAddress, udpPort);
    udp.print("connected");
    udp.endPacket();
  }
  if (connected == true && millis() > 2000 && millis() < 2200)
  {
    udp.beginPacket(udpAddress, udpPort);
    udp.println("connected"); //if connected, send a message saying so, so the receiver can notify the user
    udp.endPacket();
  }
#endif
  if (newData) //If the interrupt pin has fired, new data is available
  {
    newData = false;
    // read the sensor, passing in the filter so it will be updated
    IMU.readSensor(AHRSFilter);
    float roll_ = AHRSFilter.getRoll();
    if (roll_ < 0)
    {
      // keep roll positive
      roll_ = 360.0 + roll_;
    }
    AHRSValues.roll = roll_;
    AHRSValues.pitch = AHRSFilter.getPitch();
    AHRSValues.yaw = AHRSFilter.getYaw();
    //To send 1=pressed, 0=not pressed, the readings have to be flipped.
    button2Reading = !digitalRead(button2);
    button1Reading = !digitalRead(button1);
    button3Reading = !digitalRead(button3);
    sendValues(button1Reading, button2Reading, button3Reading);

#ifdef MAG_CALIB_DONE

#else //for easy copying:
    Serial.print("IMU.setMagCalX(");
    Serial.print(IMU.getMagBiasX_uT(), 6);
    Serial.print(", ");
    Serial.print(IMU.getMagScaleFactorX(), 6);
    Serial.println(");");

    Serial.print("IMU.setMagCalY(");
    Serial.print(IMU.getMagBiasY_uT(), 6);
    Serial.print(", ");
    Serial.print(IMU.getMagScaleFactorY(), 6);
    Serial.println(");");

    Serial.print("IMU.setMagCalZ(");
    Serial.print(IMU.getMagBiasZ_uT(), 6);
    Serial.print(", ");
    Serial.print(IMU.getMagScaleFactorZ(), 6);
    Serial.println(");");
    Serial.println(" ");

#endif
  }
#ifdef WIFI_ON
  if (udp.parsePacket()) //if there's a UDP packet available
  {
    int len = udp.read(incomingData, 255); //the length of the packet
    if (len > 0)
    {
      incomingData[len] = 0;
      if (strcmp(incomingData, "calibrate") == 0) //If the "calibrate" message is received
      {
        Serial.println("calibrating magnetometer");
        IMU.calibrateMag(); //caibrate magnetometer. 
      }
    }
  }
#endif
#endif
}
void sendValues(int button1, int button2, int button3, int button4)
{
  String send = ""; //create a string to hold the pitch, yaw, roll and button values
  send += AHRSValues.pitch;
  send += ",";
  send += AHRSValues.yaw;
  send += ",";
  send += AHRSValues.roll;
  send += ",";
  send += button1;
  send += ",";
  send += button2;
  send += ",";
  send += button3;
  send += ",";
  send += button4;
#ifdef WIFI_ON
  if (connected) //If there's a connection
  {
    //Send a packet
    udp.beginPacket(udpAddress, udpPort);
    udp.println(send); //the string from above
    udp.endPacket();
  }
#else //if not using wifi, a cabled connection is made, so serial communication is used.
  String sender = "V,";
  sender += send;
  Serial.println(sender);
#endif
}

void connectToWiFi(const char *ssid, const char *pwd)
{
  Serial.println("Connecting to WiFi network: " + String(ssid));
  WiFi.disconnect(true);   // delete settings from previous connections
  WiFi.onEvent(WiFiEvent); //register event handler that runs when an event is called.
  WiFi.begin(ssid, pwd);   //open connection
  Serial.println("Waiting for WIFI connection...");
}

//wifi event handler
void WiFiEvent(WiFiEvent_t event)
{
  switch (event) //depending on the event
  {
  case SYSTEM_EVENT_STA_GOT_IP: //if the MODremote has an IP:
    Serial.print("WiFi connected! IP address: ");
    Serial.println(WiFi.localIP());
    udp.begin(WiFi.localIP(), udpPort); //initializes the UDP state, this initializes the transfer buffer
    connected = true;
    tStart = millis();
    break;
  case SYSTEM_EVENT_STA_DISCONNECTED: //if the MODremote has been disconnected
    Serial.println("WiFi lost connection");
    connected = false;
    break;
  default:
    break;
  }
}
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
