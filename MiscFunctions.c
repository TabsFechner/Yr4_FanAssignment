#include "MiscFunctions.h"
#include "CustomTypes.h"

//Function takes two input timestamps and returns calculated time value in desired output units
//Units: 1 = min, 0 = s, -3 = ms, -6 = us
float GetTime(int t2, int units)
{
	//Define counter limit of 32-bits and counter frequency of 50M Hz
	static int counterLim = 0x7FFFFFFF;
	static int counterFreq = 50000000;

	static int noCounts, countsA, countsB;
	static float time;

	//Calculate number of counts between the two input timestamps
	//Check for counter sign change
	if (t1 > t2)
	{
		countsA = counterLim - t1;
		//Assume time greater than counter limit will not be measured
		countsB = counterLim + t2;
		noCounts = countsA + countsB;
	}
	else
	{
		noCounts = t2 - t1;
	}

	//Generate time value in desired output units
	switch (units)
	{
	case 1:
		time = (float)noCounts/ ((float)counterFreq * 60);
		break;
	case 0:
		time = (float)noCounts/ (float)counterFreq;
		break;
	case -3:
		time = ((float)noCounts/ (float)counterFreq) * 1000;
		break;
	case -6:
		time = ((float)noCounts/ (float)counterFreq) * 1000000;
		break;
	}

	return time;
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
void CheckMode(mode_t * modePTR)
{
	static int prev, pPrev;

	//Mask input from switches input to select only switch_0
	int switch_0 = *Switches & 0x1;

	//Check if mode has just changed
	if (switch_0 != prev && switch_0 != pPrev)
	{
		modePTR->changed = 1;
	}
	else
	{
		modePTR->changed = 0;
	}

	//Check value of switch and set mode value accordingly
	if (switch_0 == 1 && prev == 1)
	{
		modePTR->mode = 1;
	}
	else
	{
		modePTR->mode = 0;
	}

	//Store previous values
	pPrev = prev;
	prev = switch_0;
}
