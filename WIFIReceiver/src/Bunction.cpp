/* Bunction.cpp - library for button functionality for the MODremote
Thomas van de Moosdijk, 
06-11-2019 - added median filter 
29-10-2019 - main functionality

This is a simple way of creating button functions for the peripheral for MOD devices.
When created, its functions are called whenever a given button is pressed. If so, it 
updates the values to be sent based on the valArray that it gets from the remote.
Afterwards, it translates these to midi values and sends these over to the instrument/daw.
Many thanks to Elco Jacobs for the median filter implementation.
*/

#include "Arduino.h"
#include "Bunction.h"
#include <algorithm>
#include <array>
#include <stdint.h>
#include <vector>

Bunction::Bunction(char dimension, int sensitivity)
{
    _dimension = dimension;
    _sensitivity = 1270 / sensitivity;
    _index = 0;
    std::array<int32_t, 9> _history = {0};
}

float Bunction::processValues(float pitch, float yaw, float roll, int mapTo)
{
    switch (_dimension) //which dimension (pitch, yaw, roll) is assigned to the bunction?
    {
    case 'p': //if it's pitch:
    {
        float tempP = pitch - _lastp; //1, 2
        if (tempP <= -30 || tempP >= 30)
        {
            tempP = 0;
        }
        _sendP += tempP;                                      //3, 1
        _sendP = constrain(_sendP, -5, _sensitivity / 2 + 5); //3, 3
        _lastp = pitch;                                       //2
        _sendp = map(_sendP, 0, _sensitivity / 2, mapTo, 0);  //4,3
        _sendp = constrain(_sendp, 0, mapTo);                 //4
        _sendp = filter(_sendp);                              //4
        return _sendp;
    }
    break;
    case 'y': //if it's yaw:
    {
        float tempY = yaw - _lasty; //1, 2
        if (tempY <= -100 || tempY >= 100)
        {
            tempY = 0;
        }
        _sendY += tempY;                                  //3, 1
        _sendY = constrain(_sendY, -5, _sensitivity + 5); //3, 3
        _lasty = yaw;                                     //2
        _sendy = map(_sendY, 0, _sensitivity, 0, mapTo);  //4,3
        _sendy = constrain(_sendy, 0, mapTo);             //4
        _sendy = filter(_sendy);                          //4
        return _sendy;
    }
    break;
    case 'r': //if it's roll:
    {
        float tempR = roll - _lastr; //1, 2
        if (tempR <= -100 || tempR >= 100)
        {
            tempR = 0;
        }
        _sendR += tempR;                                  //3, 1
        _sendR = constrain(_sendR, -5, _sensitivity + 5); //3, 3
        _lastr = roll;                                    //2
        _sendr = map(_sendR, 0, _sensitivity, 0, mapTo);  //4,3
        _sendr = constrain(_sendr, 0, mapTo);             //4
        _sendr = filter(_sendr);                          //4
        return _sendr;
    }
    break;
    }
}

void Bunction::updateValues(float pitch, float yaw, float roll)
{
    _lastp = pitch;
    _lasty = yaw;
    _lastr = roll;
}

int Bunction::filter(int input) //a simple median filter. It works the following way:
                                //it populates the filter history with read values, then it
                                //sorts them and takes out the middle value (the median).
                                //This means that high/low values are filtered out at the expense of
                                //some responsiveness.
{
    _history[_index] = input;                                     //add value to the history
    _index = (_index + 1) % 9;                                    //go on to the next place (and roll over)
    std::array<int32_t, 9> copy = _history;                       //copy the array over
    std::nth_element(copy.begin(), copy.begin() + 4, copy.end()); //sort the array
    _output = copy[4];                                            //copy the middle value
    return _output;
}

int Bunction::getOutput()
{
    return _output;
}

char Bunction::getDimension()
{
    return _dimension;
}

void Bunction::changeDimension()
{
    if (_dimension == 'p')
    {
        _dimension = 'y';
        return;
    }
    if (_dimension == 'y')
    {
        _dimension = 'r';
        return;
    }
    if (_dimension == 'r')
    {
        _dimension = 'p';
        return;
    }
}