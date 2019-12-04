
#include "EE30186.h"
#include "system.h"
#include "socal/socal.h"
#include "DisplayFunctions.h"
#include "MiscFunctions.h"

#include <inttypes.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

extern float time;
static int nTime = 0;

//----------------------------------------------------- Display Functions -----------------------------------------------------//

//Define function that updates diplay based on current status of display and recent system changes
void UpdateDisplay(volatile int* Counter)
{
	static int t2, iDisp;

	//Check for time interval since last HEX display
	t2 = *Counter;
	//Returns time in seconds
	time = GetTime(t2, 0);
	//Check for recent mode changed

	mode_t *modePtr, mode;
	modePtr = &mode;

	CheckMode(*modePtr);

	//Check for two conditions that initiate scrolling text
	//Condition 1: Mode has been changed by user
	if (modeChanged)
	{
		iDisp = ScrollSetup(mode, 0);
		scrl = 1;
	}
	//Condition 2: No user input for over 1 minute
	else if (time > 20)
	{
		//Increment 20s count
		nTime++;
		//Restart timer
		t1 = *Counter;

		//If 120s reached
		if (nTime > 2)
		{
			iDisp = ScrollSetup(mode, 1);
			scrl = 1;
			nTime = 0;
		}
	}

	//Check for scrolling status and update display accordingly
	if (scrl)
	{
		ScrollRun(iDisp);
	}
	else
	{
		//Set HexA to result from multi-digit decoder
	}
}

//Define function that takes multi digit value and encodes into active segments for hex display HexA
int MultiDigitEncoder (int value)
{
	//Define empty display value to return
	int returnValue = 0xffffffff;

	// We need to be able to keep track of which digit in the number we are dealing
	// with
	int CurrentDigit = 0;

	// As we extract the digits we need a temporary variable to put the values into
	int SingleDigitDisplay;

	//Loop up through the digits in the number
	do
	{
		// Extract the bottom digit
		SingleDigitDisplay = (value % 10);

		// adjust the input value to reflect the extraction of the bottom digit
		value /= 10;

		// Clear the space that we are going to put the decoder result into
		returnValue = returnValue & ~(0xFF << (CurrentDigit * 8));

		// Shift the single decoded digit to the right place in the int and insert
		// it
		returnValue = returnValue |  (SingleDigitDisplay << (CurrentDigit * 8));

		// Update the digit postion so that if the value is non-zero we put the
		// next digit 8 bits further to the left.
		CurrentDigit++;
	}while (value > 0);

	// Pass back the multi-digit decoded result.
	return returnValue;
}

//Define function that takes char from info string and encodes into active segments for hex display
int CharEncoder(char ch)
{
	int seg;
	// Choose a value for the seg based on the value of the input. The Hex
	// display is inverted so a bit value of 1 turns off the digit.
	switch (ch)
	{
		case '0':
			seg = 0x40;
			break;
		case '1':
			seg = 0xF9;
			break;
		case '2':
			seg = 0x24;
			break;
		case '3':
			seg = 0x30;
			break;
		case '4':
			seg = 0x19;
			break;
		case '5':
			seg = 0x12;
			break;
		case '6':
			seg = 0x02;
			break;
		case '7':
			seg = 0xF8;
			break;
		case '8':
			seg = 0x00;
			break;
		case '9':
			seg = 0x10;
			break;
		case 'A':
			seg = 0xA0;
			break;
		case 'B':
			seg = 0x83;
			break;
		case 'C':
			seg = 0xA7;
			break;
		case 'D':
			seg = 0xA1;
			break;
		case 'E':
			seg = 0x86;
			break;
		case 'F':
			seg = 0x8E;
			break;
		case 'G':
			seg = 0xC2;
			break;
		case 'H':
			seg = 0x8B;
			break;
		case 'I':
			seg = 0xEE;
			break;
		case 'J':
			seg = 0xF2;
			break;
		case 'K':
			seg = 0x8A;
			break;
		case 'L':
			seg = 0xC7;
			break;
		case 'M':
			seg = 0xAA;
			break;
		case 'N':
			seg = 0xAB;
			break;
		case 'O':
			seg = 0xA3;
			break;
		case 'P':
			seg = 0x8C;
			break;
		case 'Q':
			seg = 0x98;
			break;
		case 'R':
			seg = 0xAF;
			break;
		case 'S':
			seg = 0xD2;
			break;
		case 'T':
			seg = 0x87;
			break;
		case 'U':
			seg = 0xE3;
			break;
		case 'V':
			seg = 0xD5;
			break;
		case 'W':
			seg = 0x95;
			break;
		case 'X':
			seg = 0xEB;
			break;
		case 'Y':
			seg = 0x91;
			break;
		case 'Z':
			seg = 0xE4;
			break;

		default:
			seg = 0xFF;
			break;
	}

	// Pass back the value needed to set the seg correctly
	return seg;
}

//Define function that simply clears the hex display
void ClearDisplay()
{
	//Clear Hex display
	*Hexa = 0xffffffff;
	*Hexb = 0xffffffff;
}

//-------------------- Display Functions: Scrolling Display

//Define function that sets up scrolling of new string
int ScrollSetup(int mode, int scrollOp)
{
	ClearDisplay();

	//Set current character index to zero
	int iDisp = 0;

	if (scrollOp == 0)
	{
		//Generate information string based on fan speed and mode
		GetInfoString(pidSpeed, tempSpeed, mode);
	}
	if (scrollOp == 1)
	{
		//Generate string based on random extract from Romeo and Juliet
		GetJuliet();
	}

	return iDisp;
}

//Define function that generates information string depending on current mode set.
void GetInfoString(int pid, int temp, int mode)
{
	//Declare array
	static char array[32];

	//Store concatenated string in array depending on current mode
	switch (mode)
	{
		case 0:
			sprintf(array, "%d      ", temp);
			strcpy(infoStr, "MODE PID SPEED ");
			strcat(infoStr, array);
			break;
		case 1:
			sprintf(array, "%d      ", pid);
			strcpy(infoStr, "MODE TEMP SPEED ");
			strcat(infoStr, array);
			break;
	}
}

//Define function that randomly selects Romeo and Juliet extract and stores in infoStr array
void GetJuliet()
{
	int x = rand();

	switch (x % 9)
	{
		case 0:
			strcpy(infoStr, "THESE VIOLENT DELIGHTS HAVE VIOLENT ENDS AND IN THEIR TRIUMP DIE LIKE FIRE AND POWDER      ");
			break;
		case 1:
			strcpy(infoStr, "MY BOUNTY IS AS BOUNDLESS AS THE SEA MY LOVE AS DEEP      ");
			break;
		case 2:
			strcpy(infoStr, "THUS WITH A KISS I DIE      ");
			break;
		case 3:
			strcpy(infoStr, "PARTING IS SUCH SWEET SORROW THAT I SHALL SAY GOOD NIGHT TILL IT BE MORROW      ");
			break;
		case 4:
			strcpy(infoStr, "DO NOT SWEAR BY THE MOON FOR SHE CHANGES CONSTANTLY THEN YOUR LOVE WOULD ALSO CHANGE      ");
			break;
		case 5:
			strcpy(infoStr, "DO YOU BITE YOUR THUMB AT US SIR      ");
			break;
		case 6:
			strcpy(infoStr, "O SERPENT HEART HID WITH A FLOWERING FACE      ");
			break;
		case 7:
			strcpy(infoStr, "LOVE IS A SMOKE MADE WITH THE FUME OF SIGHS      ");
			break;
	}
}

//Define function that writes scrolling text to hex displays
void ScrollRun(int iDisp)
{
	static int seg;

	//If character display value is greater than length of string
	if (iDisp > strlen(infoStr))
	{
		ClearDisplay();

		//Set scrolling to false
		scrl = 0;
		//Restart timer
		t1 = *Counter;
	}
	else if (time > 0.1)
	{
		//Encode character to 7 segment signal
		seg = CharEncoder(infoStr[iDisp]);
		//Shift character into scrolling display
		ScrollOut(seg);
		//Increment info string character index
		iDisp++;
		//Restart timer
		t1 = *Counter;
	}
}

//Define function that writes encoded display message to HEX displays
void ScrollOut(int hexValue)
{
	//Define empty display value to return
	static int outA = 0xffffffff;
	static int outB = 0xffff;
	static int curA;
	curA = *Hexa;
	static int curB;
	curB = *Hexb;

	//Select final digit to pass into HexB
	int transfer = 0xFF & (curA >> 24);
	//Shift current hexB register by 8 bits and transfer last digit from hexA by doing bitwise OR
	outB = transfer | (curB << 8);
	//Shift current hexA register by 8 bits and insert new input hex value
	outA = hexValue | (curA << 8);

	//Write new display values
	*Hexa = outA;
	*Hexb = outB;
}
