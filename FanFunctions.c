
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

static int maxRpm = 2400;

//----------------------------------------------------- Fan Functions -----------------------------------------------------//

//TODO improve reliability under fast turning. Probably to do with sampling rate, not sequence of encoder bits.
//Define function that takes void input and returns user input, change in speed
//demand, based on current and previous encoder readings
void RotaryEncoder(Speed * speedPtr)
{
	static int increment = 5;
	static int sum, prevSum;

	//Declare and initialise variable n used to ensure only one demand signal is sent per click of encoder.
	//Otherwise two changes detected per click due to complete 2-bit Grey code sequence per click
	static int n = 0;

	//Reset current demand to zero at start of each cycle
	speedPtr -> demand = 0;

	//Sum rotary encoder pins A and B from shifted and masked input from GPIO
	sum = ( (*GPIOA >> 17 & 0x1) << 1) | (*GPIOA >> 19 & 0x1);

	//Compare sum of bits to previous sum to derive direction of rotation.
	switch (sum)
	{
		case(0):
			if (prevSum == 2)
			{
				n++;
			}
			else if (prevSum == 1)
			{
				n--;
			}
			break;

		case(1):
			if (prevSum == 0)
			{
				n++;
			}
			else if (prevSum == 3)
			{
				n--;
			}
			break;

		case(2):
			if (prevSum == 3)
			{
				n++;
			}
			else if (prevSum == 0)
			{
				n--;
			}
			break;

		case(3):
			if (prevSum == 1)
			{
				n++;
			}
			else if (prevSum == 2)
			{
				n--;
			}
			break;

		default:
			break;
	}

	if (n > 3)
	{
		speedPtr -> demand = increment;
		n = 0;
	}
	else if (n < -3)
	{
		speedPtr -> demand = -increment;
		n = 0;
	}
	else
	{
		speedPtr -> demand = 0;
	}

	//Assign current value to previous and return demand
	prevSum = sum;
}

//Define function that takes void input and returns measurement of current fan speed
void SpeedMeasure(Speed * speedPtr)
{
	static int prevSpeed;
	static Time tTacho;

	//Declare and initialise number of edges counted in while loop
	static int edgeCount = 0;

	//Declare and define number of edges to be counter before exiting while loop
	static int noEdges = 30;

	//Define relationship between fan speed and number of edge counts limit in loop
	noEdges = (prevSpeed/ 90) + 3;

	//Get start time-stamp
	tTacho.t1 = * Counter;
	printf("t1: %d", tTacho.t1);

	//Detect X edges, where X = noEdges
	while (edgeCount < noEdges)
	{
		//Declare two variables to store previous tachometer readings in. Static will ensure this are not
		//overwritten between loops
		static int prev, pPrev;

		//Shift and mask input from GPIO to get to tachometer input
		int tacho = *GPIOA >> 1 & 0x1;

		//Compare current and previous fan tachometer readings to detect rising edge.
		if (tacho > pPrev && tacho == prev)
		{
			edgeCount++;
		}

		//Leave loop if fan stationary
		/*c = *Counter;
		if (c > t1 + 0xFFFFFF)
		{
			noEdges = 0;
			break;
		}*/

		//Assign previous values
		pPrev = prev;
		prev = tacho;
	};

	//Get end time-stamp
	tTacho.t2 = * Counter;
	printf(", t2: %d", tTacho.t2);

	//Check if fan stationary
	if (noEdges == 0)
	{
		speedPtr -> measured = 0;
	}
	else //If not, calculate speed
	{
		//Get time based on timer struct specific to timing tacho signal edges. Time returned in seconds
		GetTime(&tTacho, 0);

		//Calculate time for one fan revolution
		float tRev = 2*(tTacho.time)/ noEdges;

		printf(", tRev: %f,", tRev);

		//Calculate RPM
		speedPtr -> measured = 60/ tRev;
	}

	//Assign previous speed value and return measured speed
	prevSpeed = speedPtr -> measured;
}

//Define function that returns user input, target fan speed, based  on input of previous demand speed and change in speed demanded as a percentage of max RPM.
void SpeedControl(Speed * speedPtr)
{
	static int prev;

	//Calculate new target speed
	speedPtr -> target = prev + (speedPtr -> demand * maxRpm)/ 100;

	//Validate target speed is within range of fan
	speedPtr -> target = SpeedValidate(speedPtr -> target);

	prev = speedPtr -> target;
}

//Define function that takes in the desired speed and returns a speed value capped between zero and max fan rpm.
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

/*
//Define function that returns speed value based on: user input, target speed, and measured current fan speed
int PID(int targetSpeed, int measuredSpeed)
{
	static int diff, prop, deriv, integ;

	//Set coefficients
	prop = 0.5;
	deriv = 4;
	integ = 2;

	//Calculate difference in target speed and measured speed
	diff = targetSpeed - measuredSpeed;

	//Calculate resulting output speed based on PID control
	pidSpeed = prop + deriv + integ;
	//Validate target speed is within range of fan
	pidSpeed = SpeedValidate(pidSpeed);

	return pidSpeed;
}

//Define function that returns speed value based on: temperature sensor
int Temperature()
{
	//Shift and mask input from GPIO to get to reading from thermistor board
	int thermistor = *GPIOB >> 1 & 0X1;

	if(thermistor > maxUncooled)
	{
		if (thermistor > maxSafe)
		{
			printf("Do something here that protects against the thermistor board overheating.");
		}
		else
		{
			//Set speed based on temperature response profile
			tempSpeed = (3 * thermistor) - 200;
			//Validate target speed is within range of fan
			tempSpeed = SpeedValidate(tempSpeed);
		}
	}
	else
	{
		tempSpeed = 0;
	}

	return tempSpeed;
}

int PWM(int switch_0)
{
//	return pwmSignal;
}
*/
