

#ifndef MISCFUNCTIONS_H
#define MISCFUNCTIONS_H

#include "CustomTypes.h"

//Function inverts system ON status if Key_0 has just been pressed.
//output units. Function used by several timers specific to that purpose.
void CheckOn(Mode *);

//Function takes two input timestamps and returns calculated time value in desired 
void GetTime(Time *, int);

//Function sets system mode based on current position of switches 0 and 1. Mode 
//also used to indicate system ON status.
void CheckMode(Mode *, Time *);

#endif
