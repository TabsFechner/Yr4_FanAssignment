/*
 *
 * Fan Control:		Integrated Engineering, EE30186
 * Date:			25.11.2019
 *
 */

//TODO check use of global variables and static variables is correct. Does it need to be global variable for
//passing back from variable? If variable if used just in function should it always be static?

//Standard includes
#include "EE30186.h"
#include "system.h"
#include "socal/socal.h"

//Custom header files
#include "FanFunctions.h"
#include "DisplayFunctions.h"
#include "MiscFunctions.h"
#include "GlobalVariables.h"

#include <inttypes.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

//Assign pointers to address of peripheries
volatile int * LEDs = (volatile int *)ALT_LWFPGA_LED_BASE;
volatile int * Switches = (volatile int *)ALT_LWFPGA_SWITCH_BASE;
volatile int * Keys = (volatile int *)(ALT_LWFPGA_KEY_BASE);
volatile int * Counter = (volatile int *)(ALT_LWFPGA_COUNTER_BASE);
volatile int * Hexa = (volatile int *)(ALT_LWFPGA_HEXA_BASE);
volatile int * Hexb = (volatile int *)(ALT_LWFPGA_HEXB_BASE);
volatile int * GPIOA = (volatile int *)(ALT_LWFPGA_GPIO_0A_BASE);
volatile int * GPIOB = (volatile int *)(ALT_LWFPGA_GPIO_0B_BASE);

#define key0 0xE
#define key1 0xD
#define key2 0xB
#define key3 0x7

//----------------------------------------------------- Variable Declaration -----------------------------------------------------//

//Declare global struct 'speed' to store fan speed information
struct speed
{
	int demand, target, measured, temp, pid;
};

//--------------------------- Declare time variables
extern int t1;

//----------------------------------------------------- Main Function -----------------------------------------------------//

int main(int argc, char** argv)
{
	//Initialise FPGA configuration
	EE30186_Start();

	#define maxRPM 2500;
	static int isOn = 0;

	//Initialise data direction register for GPIOA and set pin 3 of GPIO to be output
	volatile int * GPIOA_Ddr = GPIOA + 1;
	*GPIOA_Ddr = 0x8;

	//Initialise counter start time variable t1 with first counter value
	t1 = *Counter;

	while (1)
	{
		*LEDs = *Switches;

		//Check on-off
		if (isOn)
		{
			//---struct speed spd;

			//Read user input, change in speed demand
			//---spd.demand = RotaryEncoder(GPIOA);

			//Measure current fan speed
			//---spd.measured = SpeedMeasure(Counter);
			//---printf(", measuredSpeed: %d\n", spd.measured);

			//Check current display status and display corresponding information
			UpdateDisplay(Counter);

			/*
			//Derive target speed
			spd.target = SpeedControl(prevDemand, spd.demand, maxRPM);
			//Calculate fan speed for mode: PID control
			spd.pid = PID(spd.target, spd.measured);
			//Calculate fan speed for mode: temperature
			spd.temp = temperature();
			prevDemand = spd.demand;
			*/

			//Set 3rd pin (4th bit) of GPIO register to 1
			*GPIOA = 0x8;

			isOn = CheckOn();
		}
		else
		{
			//Set GPIO register to all zeros
			*GPIOA = 0x0;
			isOn = CheckOn();
		}
	}

	//Clean and close the FPGA configuration
    EE30186_End();

    return 0;
}

/*
//Function returns user input, target fan speed, based  on input of previous demand speed and change in speed demanded as a percentage of max RPM.
int SpeedControl(int prevDemand, int demand, int maxRPM)
{
	//Calculate new target speed
	targetSpeed = prevDemand + (demand * maxRPM);

	//Validate target speed is within range of fan
	targetSpeed = SpeedValidate(targetSpeed);

	return targetSpeed;
}

//Function takes in the desired speed and returns a speed value capped between zero and max fan rpm.
int SpeedValidate(int desired)
{
	//Validate target range
	if (desired > maxRPM)
	{
		desired = maxRPM;
	}
	else if (desired < 0)
	{
		desired = 0;
	}

	return desired;
}

//Function returns speed value based on: user input, target speed, and measured current fan speed
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

//Function returns speed value based on: temperature sensor
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

int SpeedSet(int prevDemand, int direction) //Not needed in closed-loop control
{
	//Adjust duty based on direction
	if (direction == 0) //Clockwise
	{
		demand = prevDemand + (maxRPM*0.05);
	}
	else //Anti-clockwise
	{
		duty -= 5;
	}

	//Validate duty range
	if (duty > 100)
	{
		duty = 100;
	}
	else if (duty < 0)
	{
		duty = 0;
	}

	return duty;
}

*/
