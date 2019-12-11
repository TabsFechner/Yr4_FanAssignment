
#include "EE30186.h"
#include "system.h"
#include "socal/socal.h"
#include "FanFunctions.h"
#include "MiscFunctions.h"

#include <inttypes.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

//Declare necessary pointers to interface ports. Defined elsewhere.
extern volatile int * GPIOA;
extern volatile int * Counter;

int maxRpm = 2500;

//----------------------------------------------------- Fan Functions -----------------------------------------------------//

/*Function reads input from the 2 pins of the rotary encoder and derives the direction of rotation
  based on previous pin values. Standard increment of +- 5 used to change target fan speed.
  Input: Pointer to speed struct of custom type Speed
  Input: Pointer to display timer struct of custom type Time
  Output: Void */
void RotaryEncoder(Speed * speedPtr, Time * tDisplayPtr)
{
	static int increment = 5;
	static int prevSum;

	//Reset current demand to zero at start of each cycle
	speedPtr -> demand = 0;

	//Sum rotary encoder pins A and B from shifted and masked input from GPIO
	int sum = ((*GPIOA >> 17 & 0x1) << 1) | (*GPIOA >> 19 & 0x1);

	//Compare sum of bits to previous sum to derive direction of rotation.
	switch (sum)
	{
		//If acceptable rotary encoder input is receieved, set the demand and reset the
		//user input timer
		case(0):
			if (prevSum == 2)
			{
				speedPtr -> demand = -increment;
				tDisplayPtr -> t1 = * Counter;
			}
			else if (prevSum == 1)
			{
				speedPtr -> demand = increment;
				tDisplayPtr -> t1 = * Counter;
			}
			break;

		case(1):
			if (prevSum == 0)
			{
				speedPtr -> demand = -increment;
				tDisplayPtr -> t1 = * Counter;
			}
			else if (prevSum == 3)
			{
				speedPtr -> demand = increment;
				tDisplayPtr -> t1 = * Counter;
			}
			break;

		case(2):
			if (prevSum == 3)
			{
				speedPtr -> demand = -increment;
				tDisplayPtr -> t1 = * Counter;
			}
			else if (prevSum == 0)
			{
				speedPtr -> demand = increment;
				tDisplayPtr -> t1 = * Counter;
			}
			break;

		case(3):
			if (prevSum == 1)
			{
				speedPtr -> demand = -increment;
				tDisplayPtr -> t1 = * Counter;
			}
			else if (prevSum == 2)
			{
				speedPtr -> demand = increment;
				tDisplayPtr -> t1 = * Counter;
			}
			break;

		default:
			speedPtr -> demand = 0;
			break;
	}

	//Assign current value to previous and return demand
	prevSum = sum;
}

/*Function uses signal from fan tachometer sensor to calculate the speed of rotation in
  revolutions per minute. Number of rising edges counted in given time window used to 
  calculate exponential moving average of measured fan speed.
  Input: Pointer to tachometer timer struct of customer type Time
  Input: Pointer to speed struct of custom type Speed
  Output: Void */
void SpeedMeasure(Time * tTachoPtr, Speed * speedPtr)
{
	//Define timer limit used to count number of edges and calculate fan speed
	int tLimit = 500;

	//Declare and initialise number of rising edges detected in fan tachometer signal
	static int edgeCount = 0;

	//Declare the variable to store previous tachometer readings in and previous calculated
	//speed, used in moving average calculation to smooth measured speed reading.
	static int pTach, pRpm;

	//Shift and mask input from GPIO to get to tachometer input
	int tacho = *GPIOA >> 1 & 0x1;

	//Compare current and previous fan tachometer readings to detect rising edge.
	if (tacho == 1 && pTach == 0)
	{
		edgeCount++;
	}

	//Assign previous values
	pTach = tacho;

	//Get end time-stamp
	tTachoPtr -> t2 = * Counter;

	//Get time based on timer struct specific to timing tacho signal edges.
	//Time returned in milliseconds
	GetTime(tTachoPtr, -3);

	if (tTachoPtr -> time > tLimit)
	{
		//Check if fan is stationary
		if (edgeCount == 0)
		{
			speedPtr -> measured = 0;

			//pRpm = 0;
		}
		else
		{
			//Calculate time for one fan revolution. NOTE: one tachometer cycle equates
			//to half a fan revolution.
			float tRev = (2 * tLimit) / (float)edgeCount;

			//Calculate current rpm
			int cRpm = 60000/ tRev;

			//Calculate and validate exponential moving average of measured fan speed using previous rpm
			speedPtr -> measured = SpeedValidate(0.5 * cRpm + (1-0.5) * pRpm);
			//speedPtr -> measured = 60000/ tRev;
			//Assign previous rpm value
			pRpm = cRpm;
		}

		edgeCount = 0;

		//Restart tacho timer
		tTachoPtr -> t1 = * Counter;
	}
}
/*Function uses user input of change in speed from rotary encoder to change target fan speed.
  Target speed validated to be within fan operation range.
  Input: Pointer to speed struct of custom type Speed
  Output: Void */
void SetTarget(Speed * speedPtr)
{
	static int prev;

	//Calculate and validate new target speed
	speedPtr -> target = SpeedValidate(prev + (speedPtr -> demand * maxRpm)/100);

	prev = speedPtr -> target;
}

/*Function takes in arbitrary integer value as speed and limits to within minimum and maximum 
  fan speed range of operation.
  Input: Integer value for speed
  Output: Integer value for limited speed */
int SpeedValidate(int spd)
{
	//Validate target range
	if (spd > maxRpm)
	{
		spd = maxRpm;
	}
	else if (spd < 0)
	{
		spd = 0;
	}

	return spd;
}

/*Function calculates duty cycle based on target speed and writes output to fan for given time period
  to generate a pulse-width-modulated square wave that controls the power supplied to the fan and thus
  the fan output speed.
  Input: Pointer to PWM timer struct of customer type Time
  Input: Pointer to speed struct of custom type Speed
  Input: Pointer to mode struct of custom type Mode
  Output: Void */
void SetPWM(Time * tPWMPtr, Speed * speedPtr, Mode * modePtr)
{
	//Define signal period as 50ms
	static int T = 50;

	//Declare duty-cycle variable
	static float D;

	//Set duty cycle according to current mode
	switch (modePtr -> mode)
	{
		case 0:
			//Control fan output according to user input target speed (open-loop control)
			D = (float)speedPtr -> target / maxRpm;
			break;

		case 1:
			//Control fan output according to pid control
			D = (float)speedPtr -> pid / maxRpm;
			break;

		case 2:
			//Implement temp control mode
			D = 0;
			break;

		default:
			//Default to open-loop control
			D = (float)speedPtr -> target / maxRpm;
			break;
	}

	//Set second timer reading
	tPWMPtr -> t2 = * Counter;

	//Time returned in milliseconds
	GetTime(tPWMPtr, -3);

	//Set fan to ON if current time is less than time on period, where time on period
	//is duty cycle times signal time period
	if (tPWMPtr -> time <= D * T)
	{
		//Set 3rd pin (4th bit), fan output, of GPIO register to 1
		*GPIOA = 0x8;
	}
	else if (tPWMPtr -> time >= T)
	{
		//Restart timer if signal period reached
		tPWMPtr -> t1 = * Counter;
	}
	else
	{
		//Set GPIO register to all zeros
		*GPIOA = 0x0;
	}
}

/*Function adjusts fan target speed based on error between 'measured' fan speed and 'target'
  fan speed and stores corrected target as new speed 'pid' in speed struct. Function uses
  predefined coefficients tuned to control this system specifically.
  Input: Pointer to speed struct of custom type Speed
  Output: Void */
void PID(Speed * speedPtr)
{
	//Declare variable to store previous error calculated
	static float pErr = 0;
	static float integ = 0;

	//Set control coefficients
	float Kp = 0.585; //0.5
	float Kd = 0.45; //0.35
	float Ki = 0.00005;

	//Calculate difference in target speed and measured speed
	int err = speedPtr -> target - speedPtr -> measured;

	//Calculate derivative term using current and previous error
	float deriv = err - pErr;

	//Calculate integral term as accumulation of error
	integ += err;

	//Calculate total PID control component
	int pidCtrl = (Kp * err) + (Ki * integ) + (Kd * deriv);

	//Calculate new speed using PID control term
	int pidSpeed = speedPtr -> target + pidCtrl;

	//Validate target speed is within range of fan
	pidCtrl = SpeedValidate(pidSpeed);

	//Set pid speed
	speedPtr -> pid = pidSpeed;

	//Assign previous error values
	pErr = err;
}
