
#include "MiscFunctions.h"
#include "CustomTypes.h"

#include <string.h>
#include <stdio.h>

//Declare necessary pointers to interface ports. Defined in main.c
extern volatile int * Switches;
extern volatile int * Counter;
extern volatile int * Keys;

/*
  Function takes two input timestamps and returns calculated time value in desired 
  output units. Function used by several timers specific to that purpose.
  Units: 1 = min, 0 = s, -3 = ms, -6 = us
  Input: Pointer to a specific timer struct of custom type time
  Input: Integer value used to indicate desired ouput units
  Output: Void
*/
void GetTime(Time * timerPtr, int units)
{
	//Define counter limit and counter frequency of 50M Hz
	static int counterLim = 0x7FFFFFFF;
	static int counterFreq = 50000000;

	static int noCounts, countsA, countsB;

	//Calculate number of counts between the two input timestamps, handling all 
	//combinations of timestamps across counter boundaries
	if (timerPtr -> t1 > timerPtr -> t2)
	{
		countsA = counterLim - timerPtr -> t1;

		//Assume time greater than counter limit will not be measured
		countsB = counterLim + timerPtr -> t2;

		noCounts = countsA + countsB;
	}
	else
	{
		noCounts = timerPtr -> t2 - timerPtr -> t1;
	}

	//Generate time value in desired output units
	switch (units)
	{
	case 1:
		timerPtr -> time = (float)noCounts/ ((float)counterFreq * 60);
		break;
	case 0:
		timerPtr -> time = (float)noCounts/ (float)counterFreq;
		break;
	case -3:
		timerPtr -> time = ((float)noCounts/ (float)counterFreq) * 1000;
		break;
	case -6:
		timerPtr -> time = ((float)noCounts/ (float)counterFreq) * 1000000;
		break;
	}
}

/*
  Function inverts system ON status if Key_0 has just been pressed.
  Input: Pointer to mode struct of custom type Mode
  Output: Void
*/
void CheckOn(Mode * modePtr)
{
	static int prev;

	//Mask input from keys input to select only Key_0
	int key_0 = ~*Keys & 0x1;

	//Check if Key_0 has recently been pressed
	if (key_0 == 1 && key_0 != prev)
	{
		//Invert system ON status
		modePtr -> isOn = ~(modePtr -> isOn) & 0x1;
	}

	//Store current value of key_0 in prev
	prev = key_0;
}

/*
  Function sets system mode based on current position of switches 0 and 1. Mode 
  also used to indicate system ON status.
  Input: Pointer to mode struct of custom type Mode
  Input: Pointer to display timer struct of custom type Time
  Output: Void
*/
void CheckMode(Mode * modePtr, Time * tDisplayPtr)
{
	//Initialise previous sum as value that will ensure correct system startup 
	//procedure
	static int pSum = 100;
	static int pOn = 1;

	//Check if system is on
	if (modePtr -> isOn)
	{
		//Mask input from switches input to select only switch 0 and 1 and sum
		int sum =  *Switches & 0x3;

		//Check if mode or ON status has recently changed
		if (sum != pSum || pOn != modePtr -> isOn)
		{
			modePtr -> changed = 1;

			//Restart user input timer
			tDisplayPtr -> t1 = * Counter;
		}
		else
		{
			modePtr -> changed = 0;
		}

		//Change mode and set mode description based on switch positions
		switch(sum)
		{
			//Open loop control mode
			case 0:
				modePtr -> mode = 0;
				strcpy(modePtr -> description, "MODE OL TARGET ");
				break;

			//Closed loop PID control mode
			case 1:
				modePtr -> mode = 1;
				strcpy(modePtr -> description, "MODE CL TARGET ");
				break;

			//Temperature mode
			case 2:
				modePtr -> mode = 2;
				strcpy(modePtr -> description, "MODE TEMP TARGET ");
				break;

			//Temperature mode
			case 3:
				modePtr -> mode = 2;
				strcpy(modePtr -> description, "MODE TEMP TARGET ");
				break;

			default:
				modePtr -> mode = 0;
				strcpy(modePtr -> description, "MODE OL TARGET ");
				break;
		}

		//Store previous values
		pSum = sum;
	}
	else
	{
		//Check if system ON status has just changed
		if (pOn == 1)
		{
			modePtr -> changed = 1;
		}
		else if (pOn == modePtr -> isOn)
		{
			modePtr -> changed = 0;
		}

		//Set mode to indicate system is off
		modePtr -> mode = 9;

		//Define info string to be displayed when system is off
		strcpy(modePtr -> description, "FAN OFF      ");
	}

	pOn = modePtr -> isOn;
}
