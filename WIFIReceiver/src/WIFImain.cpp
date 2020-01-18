/* Thomas van de Moosdijk
WIFI Receiver code
WIFImain.ccp
*/

//Uncomment the following options to enable the corresponing functionality. For debugging or using midi over processing, SERIAL_ON should be used.
//to use the MIDI port, uncomment MIDI_ON. WIFI should almost always be on, since part of the main functionality of the device is to listen to
//incoming wireless connections.

#define MIDI_ON
// #define SERIAL_ON
#define WIFI_ON
int MIDIchannel = 1;        //which channel do you want to send midi data to?
#define amount_of_buttons 3 //how many buttons does the remote have connected?
#define BAUD_RATE 115200    //115200 for processing, 31250 for MIDI
int sensitivity = 10;       //on a scale from 0 (not sensitive) to 10 (very sensitive), how sensitive do you want the sensors to be?

#include <WiFi.h>     //used to enable WIFI connectivity
#include <MIDI.h>     //used for MIDI connectivity
#include <FastLED.h>  //used for the RGB led functionality‰to digitalwrite()
#include <Bunction.h> //used for recognizing if the buttons on the MODremote are pressed and processing values.
#include <algorithm>
#include "vector"
#include "stdint.h"

#define LED_BUILTIN 22 // the onboard LED of the ESP32
#define DATA_PIN 14    // the pin that the RGB led is connected to.
#define button1 34
#define button2 35
#define button3 32
int ledState = LOW;                   //for blinking the onboard led.
char assignments[] = {'r', 'p', 'y'}; //for every button, which dimension do you want to use? (add functions )
                                      //('p'=pitch, 'y'=yaw, 'r'=roll)

int button1State = LOW;
int button2State = LOW;
int button3State = LOW;
int button1HolddownTimer = 0;
int ControllerByteValues[] = {16, 17, 18, 19, 80, 81, 82, 83};
bool checkViaLight = false;

// Set these to your desired credentials.
const char *ssid = "RECEIVER SSID";
const char *password = "PASSWORD";

unsigned int localPort = 3333;         //the UDP port
char incomingData[255];                //the character array to hold incoming data
float valArray[amount_of_buttons + 3]; //array for the orientation and buttonstates.

WiFiUDP udp;
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI); //the MIDI instance is created regardless of the MIDI_ON flag.
CRGBArray<1> leds;

Bunction *bunction[amount_of_buttons]; //Creating an array of button and their functions (bunctions) for easy reference later

void processValues(float p, float r, float y, float b1, float b2, float b3);
void runCalibration();
void flashPixels(char color, int onTime, int offTime, int flashes);
void checkButtons();

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(BAUD_RATE);
  WiFi.softAP(ssid, password, 1, 1, 1); //1, 1, 1 for channel 1, hidden and 1 max connection
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, 1); // initialize the RGB led object
  udp.begin(localPort);                             //open ESP32 for udp connections
  MIDI.begin(1);                                    // Launch MIDI on channel 1
  pinMode(22, OUTPUT);                              //set builtin LED as output
  pinMode(button1, INPUT_PULLDOWN);                 //Using the builtin pulldown resistors
  pinMode(button2, INPUT_PULLDOWN);
  pinMode(button3, INPUT_PULLDOWN);
  for (int i = 0; i < amount_of_buttons; i++)
  { //create new bunction objects, to be able to process data when a button on the MODremote is pressed.
    bunction[i] = new Bunction(assignments[i], sensitivity);
  }
  flashPixels('W', 200, 100, 2); //at startup, flash the RGB Led for 2 times, 200ms on time, 100ms interval.
  Serial.println("Server started");
}

void loop()
{
  checkButtons();
#ifdef WIFI_ON

  if (udp.parsePacket()) //if there's a UDP packet available
  {
    int len = udp.read(incomingData, 255); //the length of the packet
    if (len > 0)
    {
      incomingData[len] = 0;
      if (strcmp(incomingData, "connected") == 0) //If the incoming message is "connected"
      {
        flashPixels('W', 100, 50, 5); //flash the RGB led white 5 times, 100ms on time, 50ms interval
      }
    }

    //take the incoming data, split it at the delimiters ("," in our case).
    //Then store these values in the valArray array to be used later:
    char *ptr = strtok(incomingData, ","); // splits the incoming data string into strings, at the ",".
    int i = 0;
    while (ptr != NULL)
    {
      valArray[i] = atof(ptr); //convert the split values to floats to be sent over MIDI or serial.
      ptr = strtok(NULL, ","); //move on to the next value in the string, starting after ",".
      i++;
    }
    for (int j = 3; j < amount_of_buttons + 3; j++) //for every button:
    {
      if (valArray[j] == 1) //if the button is pressed (1 is pressed)
      {
        if (button3State == HIGH) //If the third button is pressed, this means that the button pressed on the MOD Remote should change function, therefore, if button3State is high:
        {
          if (valArray[3] == 1)
          {
            bunction[0]->changeDimension(); //button 1 on the MOD Remote
            button3State = LOW;
          }
          if (valArray[4] == 1)
          {
            bunction[1]->changeDimension(); //button 2 on the MOD Remote
            button3State = LOW;
          }
          if (valArray[5] == 1)
          {
            bunction[2]->changeDimension(); //button 3 on the MOD Remote
            button3State = LOW;
          }
        }
        if (checkViaLight) //if the middle button on the remote has been pressed, the RGB light will display brightness corresponding to the MIDI vanlue of the corresponding button on the MOD Remote.
        {
          switch (bunction[j - 3]->getDimension()) //Using the getDimension function, a p, y, or r is returned for the function.
          {
          case 'p': //if p (pitch) is returned, make the light red
          {
            Serial.println("p");
            leds[0] = CHSV(0, 255, 35 + bunction[j - 3]->getOutput());
            FastLED.show();
          }
          break;
          case 'y': //if y (yaw) is returned, make the light green
          {
            Serial.println("y");
            leds[0] = CHSV(96, 255, 35 + bunction[j - 3]->getOutput());
            FastLED.show();
          }
          break;
          case 'r': //if r (roll) is returned, make the light blue
          {
            Serial.println("r");
            leds[0] = CHSV(160, 255, 35 + bunction[j - 3]->getOutput());
            FastLED.show();
          }
          break;
          }
        }
#ifdef MIDI_ON
        {
          MIDI.sendControlChange(ControllerByteValues[j - 3], bunction[j - 3]->processValues(valArray[0], valArray[1], valArray[2], 127), 1); //process the values and send them over midi (j-2 for OP1)
          //PUTTING SERIAL OVER HERE FOR A SECOND
        }
#endif
#ifdef SERIAL_ON
        {
          bunction[j - 3]->processValues(valArray[0], valArray[1], valArray[2], 127);
          String sendLine = "<,";
          sendLine += bunction[0]->getOutput(); //bunction 1
          sendLine += ",";
          sendLine += bunction[1]->getOutput(); //bunction 1
          sendLine += ",";
          sendLine += bunction[2]->getOutput(); //bunction 1
          sendLine += ",";
          sendLine += (int)valArray[3]; //button1 on/off
          sendLine += ",";
          sendLine += (int)valArray[4]; //button2 on/off
          sendLine += ",";
          sendLine += (int)valArray[5]; //button3 on/off
                                        // sendLine += valArray[6]; //button4 on/off (not sent with 3-button peripherals)
          Serial.println(sendLine);
        }
#endif
      }
      else //if the button is not pressed (0 means not pressed)
      {
        bunction[j - 3]->updateValues(valArray[0], valArray[1], valArray[2]); //just update the values within each bunction to be able to get up to date measurements.
      }
    }

    ledState = !ledState;
  }
#endif
  /////////////////////////
  digitalWrite(LED_BUILTIN, ledState); //The buitin LED flickers quickly to note that the receiver is working
  leds[0] = CRGB::Black;
  FastLED.show();
}

void runCalibration()
{ //this function sends a calibration command to the MODremote, then waits while there's no new data received.
  long previousMillis = millis();
  bool up = true;    //fade up function for RGB LED
  bool down = false; //fade down function for RGB LED
  int i = 0;
  long tStart = millis();
  while (millis() - tStart < 65000)
  {
    if (millis() - previousMillis > 25 && up == true)
    {
      i += 2;
      previousMillis = millis();
      leds[0] = CHSV(0, 0, i);
      FastLED.show();
    }
    if (millis() - previousMillis > 25 && down == true)
    {
      i -= 2;
      previousMillis = millis();
      leds[0] = CHSV(0, 0, i);
      FastLED.show();
    }
    if (i >= 90)
    {
      up = false;
      down = true;
    }
    if (i <= 0)
    {
      up = true;
      down = false;
    }
  }
}

void flashPixels(char color, int timeOn, int timeOff, int flashes)
{
  for (int i = 0; i < flashes; i++)
  {
    if (color == 'W')
    {
      leds[0] = CHSV(0, 0, 100);
    }
    if (color == 'R' || color == 'p')
    {
      leds[0] = CHSV(0, 255, 100);
    }
    if (color == 'G' || color == 'y')
    {
      leds[0] = CHSV(96, 255, 100);
    }
    if (color == 'B' || color == 'r')
    {
      leds[0] = CHSV(160, 255, 100);
    }
  }
  FastLED.show();
  delay(timeOn);
  leds[0] = CHSV(0, 0, 0);
  FastLED.show();
  delay(timeOff);
}

void checkButtons()
{
  if (digitalRead(button1) == HIGH && button1State == LOW)
  {
    button1HolddownTimer++;
    leds[0] = CRGB::Red;
    FastLED.show();
    if (button1HolddownTimer >= 1000)
    {
      button1State = HIGH;
    }
  }
  if (digitalRead(button2) == HIGH)
  {
    leds[0] = CRGB::Green;
    FastLED.show();
    button2State = HIGH;
  }
  if (digitalRead(button3) == HIGH)
  {
    leds[0] = CRGB::Blue;
    FastLED.show();
    button3State = HIGH;
  }

  if (button1State == HIGH && digitalRead(button1) == LOW)
  {
    leds[0] = CRGB::Red;
    FastLED.show();
    udp.beginPacket(udp.remoteIP(), udp.remotePort());
    udp.print("calibrate");
    udp.endPacket();
    button1State = LOW;
    runCalibration();
  }
  if (button2State == HIGH && digitalRead(button2) == LOW)
  {
    leds[0] = CRGB::Green;
    FastLED.show();
    checkViaLight = !checkViaLight;
    button2State = LOW;
  }
}