
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

extern volatile int * Counter;
extern volatile int * Hexa;
extern volatile int * Hexb;

//----------------------------------------------------- Display Functions -----------------------------------------------------//

/*
  Function prepares and calls the two major functions used to update the HEX display. 
  Input: Pointer to display timer struct of custom type Time
  Input: Pointer to speed struct of custom type Speed
  Input: Pointer to mode struct of custom type Mode
  Output: void 
*/
void DisplayManage(Time *  tDisplayPtr, Speed * speedPtr, Mode * modePtr)
{
	static Display display = { .nTime = 0, .scrl = 0, .iDisp = 0 };

	//Check for time interval since last HEX display
	tDisplayPtr -> t2 = * Counter;

	//Calculate time between to display timer readings
	GetTime(tDisplayPtr, 0);

	UpdateInfo(&display, tDisplayPtr, modePtr, speedPtr);

	UpdateDisplay(&display, tDisplayPtr, modePtr, speedPtr);
}

/*
  Function updates information to be displayed on HEX displays, either if the system mode has 
  recently changed or if there has been no user input for over 2 minutes, by initiating the 
  scrolling text setup.
  Input: Pointer to display information struct of custom type Display
  Input: Pointer to display timer struct of custom type Time
  Input: Pointer to mode struct of custom type Mode
  Input: Pointer to speed struct of custom type Speed
  Output: void 
*/
void UpdateInfo(Display * displayPtr, Time * tDisplayPtr, Mode * modePtr, Speed * speedPtr)
{
	//Check for two conditions that trigger scrolling text to be updated

	//Condition 1: Mode has been changed by user
	if (modePtr -> changed)
	{
		//Pass infoStr char array as compiler automatically converts into pointer to first element
		ScrollSetup(displayPtr, modePtr, speedPtr);

		//Set current character index to zero
		displayPtr -> iDisp = 0;

		//Set scrolling status to true
		displayPtr -> scrl = 1;
	}

	//Condition 2: No user input for over 20 s
	else if (tDisplayPtr -> time > 20)
	{
		//Increment 20s count
		displayPtr -> nTime++;

		//Restart timer
		tDisplayPtr -> t1 = * Counter;

		//If 120s reached
		if (displayPtr -> nTime > 5)
		{
			//Set up scrolling display
			ScrollSetup(displayPtr, modePtr, speedPtr);

			//Set current character index to zero
			displayPtr -> iDisp = 0;

			//Set scrolling status to true
			displayPtr -> scrl = 1;

			//Reset 20s counter to zero since no input for 120s condition has been met
			displayPtr -> nTime = 0;
		}
	}
}

/*
  Function updates the display once the necessary display information changes have been made. If
  no scrolling text is to be displayed, the display output will be set to show live information 
  about the system mode and fan speed.
  Input: Pointer to display information struct of custom type Display
  Input: Pointer to display timer struct of custom type Time
  Input: Pointer to mode struct of custom type Mode
  Input: Pointer to speed struct of custom type Speed
  Output: void 
*/
void UpdateDisplay(Display * displayPtr, Time * tDisplayPtr, Mode * modePtr, Speed * speedPtr)
{
	int displayValue, d1, d2;

	//Check for scrolling status and update display accordingly
	if (displayPtr -> scrl)
	{
		//Check for current character index being greater than length of info string
		if (displayPtr -> iDisp > strlen(displayPtr -> infoStr))
		{
			if (modePtr -> isOn)
			{
				ClearDisplay();

				//Set scrolling to false
				displayPtr -> scrl = 0;

				//Restart timer
				tDisplayPtr -> t1 = * Counter;
			}
			else
			{
				//Set current character index to zero
				displayPtr -> iDisp = 0;
			}
		}
		else
		{
			//Update scrolling display
			ScrollRun(displayPtr, tDisplayPtr, Counter);
		}
	}
	else
	{
		switch(modePtr -> mode)
		{
			case 0:
				d1 = CharEncoder('O');
				d2 = CharEncoder('L');
				break;

			case 1:
				d1 = CharEncoder('C');
				d2 = CharEncoder('L');
				break;

			case 2:
				//Implement temp
				break;
		}

		*Hexb = d2 | (d1 << 8);

		displayValue = speedPtr -> measured;

		*Hexa = MultiDigitEncoder(displayValue);
	}
}

/*
  Function takes mult digit value and encodes into active segments for HEX display HexA.
  Input: Integer value of multiple digit fan speed
  Output: Binary value used to write to display output
*/
int MultiDigitEncoder (int value)
{
	//Define empty display value to return
	int returnValue = 0xffffffff;

	// We need to be able to keep track of which digit in the number we are dealing
	// with
	int currentDigit = 0;

	// As we extract the digits we need a temporary variable to put the values into
	int seg;

	//Loop up through the digits in the number
	do
	{
		//Define empty char array to store integers values as characters. Array defined
		//as single digit enforces array boundaries and removes risk of using sprintf.
		char x [1] = {};

		//Append result from modulus calculation into array this ensures it is in
		//the correct format for the char encoder.
		sprintf(x, "%d", value % 10);

		//Encode current digit to HEX value
		seg = CharEncoder(x[0]);

		// adjust the input value to reflect the extraction of the bottom digit
		value /= 10;

		// Clear the space that we are going to put the decoder result into
		returnValue = returnValue & ~(0xFF << (currentDigit * 8));

		// Shift the single decoded digit to the right place in the int and insert
		// it
		returnValue = returnValue |  (seg << (currentDigit * 8));

		// Update the digit postion so that if the value is non-zero we put the
		// next digit 8 bits further to the left.
		currentDigit++;
	}while (value > 0);

	// Pass back the multi-digit decoded result.
	return returnValue;
}

/*
  Function takes character from display information string and encodes into active segments
  for HEX display.
  Input: Character value of single letter or number
  Output: Binary value used to write to display output
*/
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

//Define function that simply clears the entire HEX display
void ClearDisplay()
{
	//Clear Hex display
	*Hexa = 0xffffffff;
	*Hexb = 0xffffffff;
}

//----------------------------------------------- Display Functions: Scrolling Display --------------------------------------------------//

/*
  Function gets display information string based on scrolling purpose.
  Input: Pointer to display information struct of custom type Display
  Input: Pointer to mode struct of custom type Mode
  Input: Pointer to speed struct of custom type Speed
  Output: void 
*/
//TODO: move this to be within update info Function
void ScrollSetup(Display * displayPtr, Mode * modePtr, Speed * speedPtr)
{
	ClearDisplay();

	if (displayPtr -> scrlMode == 0)
	{
		//Generate information string based on fan speed and mode
		GetInfoString(displayPtr, modePtr, speedPtr);
	}
	if (displayPtr -> scrlMode == 1)
	{
		//Generate string based on random extract from Romeo and Juliet
		GetJuliet(displayPtr);
	}
}

/*
  Function generates information string based on current system mode description and 
  fan speed.
  Input: Pointer to display information struct of custom type Display
  Input: Pointer to mode struct of custom type Mode
  Input: Pointer to speed struct of custom type Speed
  Output: void 
*/
void GetInfoString(Display * displayPtr, Mode * modePtr, Speed * speedPtr)
{
	//Declare array
	char array[32];

	//Store concatenated string in array depending on current mode
	switch (modePtr -> mode)
	{
		case 9:
			strcpy(displayPtr -> infoStr, modePtr -> description);
			break;

		case 0:
			sprintf(array, "%d      ", speedPtr -> target);
			strcpy(displayPtr -> infoStr, modePtr -> description);
			strcat(displayPtr -> infoStr, array);
			break;
		case 1:
			sprintf(array, "%d      ", speedPtr -> pid);
			strcpy(displayPtr -> infoStr, modePtr -> description);
			strcat(displayPtr -> infoStr, array);
			break;
		case 2:
			sprintf(array, "%d      ", speedPtr -> temp);
			strcpy(displayPtr -> infoStr, modePtr -> description);
			strcat(displayPtr -> infoStr, array);
			break;
		case 3:
			sprintf(array, "%d      ", speedPtr -> temp);
			strcpy(displayPtr -> infoStr, modePtr -> description);
			strcat(displayPtr -> infoStr, array);
			break;
	}
}

/*
  Function randomly selects 1 of 8 Romeo and Juliet extract to be used as display 
  information string.
  Input: Pointer to display information struct of custom type Display
  Output: void 
*/
void GetJuliet(Display * displayPtr)
{
	int x = rand();

	switch (x % 9)
	{
		case 0:
			strcpy(displayPtr -> infoStr, "THESE VIOLENT DELIGHTS HAVE VIOLENT ENDS AND IN THEIR TRIUMP DIE LIKE FIRE AND POWDER      ");
			break;
		case 1:
			strcpy(displayPtr -> infoStr, "MY BOUNTY IS AS BOUNDLESS AS THE SEA MY LOVE AS DEEP      ");
			break;
		case 2:
			strcpy(displayPtr -> infoStr, "THUS WITH A KISS I DIE      ");
			break;
		case 3:
			strcpy(displayPtr -> infoStr, "PARTING IS SUCH SWEET SORROW THAT I SHALL SAY GOOD NIGHT TILL IT BE MORROW      ");
			break;
		case 4:
			strcpy(displayPtr -> infoStr, "DO NOT SWEAR BY THE MOON FOR SHE CHANGES CONSTANTLY THEN YOUR LOVE WOULD ALSO CHANGE      ");
			break;
		case 5:
			strcpy(displayPtr -> infoStr, "DO YOU BITE YOUR THUMB AT US SIR      ");
			break;
		case 6:
			strcpy(displayPtr -> infoStr, "O SERPENT HEART HID WITH A FLOWERING FACE      ");
			break;
		case 7:
			strcpy(displayPtr -> infoStr, "LOVE IS A SMOKE MADE WITH THE FUME OF SIGHS      ");
			break;
	}
}

/*
  Function, after a given time period, encodes current character from display information 
  string and decodes into output for HEX display. After calling a function to scroll 
  display output, the current character index is incremented.
  Input: Pointer to display information struct of custom type Display
  Input: Pointer to display timer struct of custom type Time
  Input: Pointer to system counter, used to restart display timer
  Output: void
*/
void ScrollRun(Display * displayPtr, Time * tDisplayPtr, volatile int * Counter)
{
	static int seg;

	if (tDisplayPtr -> time > 0.2)
	{
		//Encode current character to 7 segment signal
		seg = CharEncoder(*(displayPtr -> infoStr + displayPtr -> iDisp));

		//Shift character into scrolling display
		ScrollOut(seg);

		//Increment info string character index
		(displayPtr -> iDisp)++;

		//Restart timer
		tDisplayPtr -> t1 = * Counter;
	}
}

/*
  Function shifts display along and writes new character into display.
  Input: Decoded binary value for HEX display output
  Output: void
*/
void ScrollOut(int hexValue)
{
	//Define empty display value to return
	static int outA = 0xffffffff;
	static int outB = 0xffff;

	//Declare variables to store current values from HEX display registers
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
