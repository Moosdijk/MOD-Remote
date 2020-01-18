/* Bunction.h - library for button functionality for the MOD Remote
This library also makes relative control for the MOD Remote possible
Many thanks to Elco Jacobs for the median filter implementation.
Thomas van de Moosdijk, 
*/

#ifndef Bunction_h
#define Bunction_h

#include "Arduino.h"
#include <array>

class Bunction
{
public:
    Bunction(char dimension, int sensitivity);
    float processValues(float pitch, float yaw, float roll, int mapTo);
    void updateValues(float pitch, float yaw, float roll);
    int filter(int input);  //pitch, yaw, and roll filters
    int getOutput();        //if you just want to view the value, instead of updating it
    char getDimension();    //if you want to get the dimension of the bunction that you're interacting with
    void changeDimension(); //for changing the dimension of a bunction
private:
    //values that hold pitch yaw roll orientation for relative control over midi values
    float _lastp; //the variable that holds the last known value
    float _sendp; //the unmapped variable before it is sent
    float _sendP; //the value that will be sent over midi/control chain/serial.

    float _lastr; //etc
    float _sendr;
    float _sendR;

    float _lasty;
    float _sendy;
    float _sendY;

    int _sensitivity; //the sensitivity for each of the axes.
    char _dimension;  //pitch yaw or roll.

    std::array<int32_t, 9> _history;
    int _index;
    int _output;
};

#endif