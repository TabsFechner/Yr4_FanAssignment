/*
 *
 *      Fan Control:	Integrated Engineering, EE30186
 *       Start Date:	25.11.2019
 *  Submission Date:	12.12.2019
 *
 */

//TODO PID control
//TODO Use third mode to allow for setting to open loop control also
//TODO Insert some error catches
//TODO Improve speed measured stability when writing to HEX and system cycle period increases
//TODO (Temp)
//TODO Readme
//Use of structs and pointers to structs
//Emphasis on minimal appearance of main function while loop
//Custom types: can have multiple counters - no need for delay, only to set counters in specified struct
//TODO Check commenting

//---------------------------------------------------------- Setup --------------------------------------------------------//

//Include given assignment and system header files
#include "EE30186.h"
#include "system.h"
#include "socal/socal.h"

//Include custom header files
#include "CustomTypes.h"
#include "FanFunctions.h"
#include "DisplayFunctions.h"
#include "MiscFunctions.h"

//Include libraries
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

//----------------------------------------------------- Main Function -----------------------------------------------------//

int main(int argc, char** argv)
{
	//Initialise FPGA configuration
	EE30186_Start();

	//------------------------------- Initialise timers ---------------------------------

	//Set start time variable t1 with first counter value
	//Display timer
	static Time tDisplay;
	tDisplay.t1 = * Counter;

	//PWM timer
	static Time tPWM;
	tPWM.t1 = * Counter;

	//Tacho timer
	static Time tTacho;
	tTacho.t1 = * Counter;

	//------------------------- Initialise other custom structs -------------------------

	//Define struct of custom type speed to store fan speed data and
	//initialise fan speed target as zero
	Speed speed;
	speed.target = 0;
	speed.measured = 0;

	//Define stuct of custom type Mode to store current mode and mode change flag
	Mode mode;
	mode.isOn = 0;
	mode.changed = 1;

	//Initialise data direction register for GPIOA and set pin 3 of GPIO
	//to be output
	volatile int * GPIOA_Ddr = GPIOA + 1;
	*GPIOA_Ddr = 0x8;

	//------------------------------- Enter while loop ----------------------------------

	while (1)
	{
		//Check on-off
		if (mode.isOn)
		{
			//Read user input as demand for change in speed
			RotaryEncoder(&speed, &tDisplay);

			//Set target speed
			SetTarget(&speed);

			//Measure current fan speed
			SpeedMeasure(&tTacho, &speed);

			if (mode.mode == 1)
			{
				//Calculate fan speed for mode: PID control
				PID(&speed);
			}
			else if (mode.mode >= 2)
			{
				//Implement temperature mode
			}

			//Generate PWM signal to drive fan output
			SetPWM(&tPWM, &speed, &mode);

			//Check current display status and update display accordingly
			UpdateDisplay(&tDisplay, &speed, &mode);

			CheckOn(&mode);
		}
		else
		{
			//Set GPIO register to all zeros
			*GPIOA = 0x0;

			//Check current display status and update display accordingly
			UpdateDisplay(&tDisplay, &speed, &mode);

			CheckOn(&mode);
		}
	}

	//Clean and close the FPGA configuration
    EE30186_End();

    return 0;
}

/*
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
*/
