

#ifndef MISCFUNCTIONS_H
#define MISCFUNCTIONS_H

#include "CustomTypes.h"

//Declare function that inverts value of isOn if Key_0 has just been pressed.
int CheckOn(void);

//Declare function that takes two input timestamps and returns calculated time value in desired output units
//Units: 1 = min, 0 = s, -3 = ms, -6 = us
float GetTime(int, int, int);

//Function sets mode value based on input value from switch 1.
//Mode 0: PID Control
//Mode 1: Temperature Control
void CheckMode(Mode *);

#endif
