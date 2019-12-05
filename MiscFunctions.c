#include "MiscFunctions.h"
#include "CustomTypes.h"


extern volatile int * Switches;
extern volatile int * Keys;

//Function takes two input timestamps and returns calculated time value in desired output units
//Units: 1 = min, 0 = s, -3 = ms, -6 = us
void GetTime(Time * timerPtr, int units)
{
	//Define counter limit of 32-bits and counter frequency of 50M Hz
	static int counterLim = 0x7FFFFFFF;
	static int counterFreq = 50000000;

	static int noCounts, countsA, countsB;

	//Calculate number of counts between the two input timestamps
	//Check for counter sign change
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

//Function inverts value of isOn if Key_0 has just been pressed.
int CheckOn()
{
	static int prev, isOn;

	//Mask input from keys input to select only Key_0
	 int key_0 = *Keys & 0x1;

	//Check if value of Key_0 has recently switched to 1
	if (key_0 == 1 && key_0 != prev)
	{
		//Invert global variable isOn
		isOn = ~isOn;
	}

	//Store current value of key_0 in prev
	prev = key_0;
	return isOn;
}

//Function sets mode value based on input value from switch 1.
//Mode 0: PID Control
//Mode 1: Temperature Control
void CheckMode(Mode * modePtr)
{
	static int prev, pPrev;

	//Mask input from switches input to select only switch_0
	int switch_0 = * Switches & 0x1;

	//Check if mode has just changed
	if (switch_0 == 1 && prev == 1 && switch_0 != pPrev)
	{
		modePtr -> changed = 1;
		modePtr -> mode = 1;
	}
	else if (switch_0 == 0 && prev == 0 && switch_0 != pPrev)
	{
		modePtr -> changed = 1;
		modePtr -> mode = 0;
	}
	else
	{
		modePtr -> changed = 0;
	}

	//Store previous values
	pPrev = prev;
	prev = switch_0;
}
