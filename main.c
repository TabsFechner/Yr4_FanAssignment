/*
 *
 *      Fan Control:	Integrated Engineering, EE30186
 *       Start Date:	25.11.2019
 *  Submission Date:	12.12.2019
 *
 *	Program used to control a single 3-pin fan, taking user input through various periphery devices
 *  on either the FPGA or the extension board. Two modes of operation implement either open-loop or
 *  closed-loop PID control of the fan speed. HEX displays used to display information about the fan
 *  and system.
 *
 */

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

	//Declare struct of custom type speed to store fan speed data and
	//initialise fan speed target as zero
	static Speed speed = { .target = 0, .measured = 0 };

	//Declare and initialise stuct of custom type Mode
	static Mode mode = { .isOn = 0, .changed = 1 };

	//------------------------ Initialise Data Direction Register ------------------------

	//Initialise data direction register for GPIOA and set pin 3 of GPIO
	//to be output
	volatile int * GPIOA_Ddr = GPIOA + 1;
	*GPIOA_Ddr = 0x8;

	//------------------------------- Enter while loop ----------------------------------

	while (1)
	{
		//Check system on-off
		if (mode.isOn)
		{
			//Check for current mode
			CheckMode(&mode, &tDisplay);

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
			DisplayManage(&tDisplay, &speed, &mode);

			CheckOn(&mode);
		}
		else
		{
			//Check for current mode
			CheckMode(&mode, &tDisplay);

			//Set GPIO register to all zeros
			*GPIOA = 0x0;

			//Check current display status and update display accordingly
			DisplayManage(&tDisplay, &speed, &mode);

			CheckOn(&mode);
		}
	}

	//Clean and close the FPGA configuration
    EE30186_End();

    return 0;
}
