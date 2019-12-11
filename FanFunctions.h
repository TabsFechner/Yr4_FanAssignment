
#ifndef FANFUNCTIONS_H
#define FANFUNCTIONS_H

#include "CustomTypes.h"

//----------------------------------------------------- Fan Functions -----------------------------------------------------//

//Declare function that takes void input and returns user input, change in speed demand, based on current and previous encoder readings
void RotaryEncoder(Speed *, Time *);

//Declare function that takes void input and returns measurement of current fan speed
void SpeedMeasure(Time *, Speed *);

//Declare function that returns user input, target fan speed, based  on input of previous demand speed and change in speed demanded as a percentage of max RPM.
void SetTarget(Speed *);

//Declare function that takes in the desired speed and returns a speed value capped between zero and max fan rpm.
int SpeedValidate(int);

void SetPWM(Time *, Speed *, Mode *);

//Declare function that returns speed value based on: user input, target speed, and measured current fan speed
void PID(Speed *);

#endif
