
#ifndef FANFUNCTIONS_H
#define FANFUNCTIONS_H

#include "CustomTypes.h"

//----------------------------------------------------- Fan Functions -----------------------------------------------------//

//Declare function that takes void input and returns user input, change in speed demand, based on current and previous encoder readings
void RotaryEncoder(Speed *);

//Declare function that takes void input and returns measurement of current fan speed
void SpeedMeasure(Time *, Speed *);


//Declare function that returns user input, target fan speed, based  on input of previous demand speed and change in speed demanded as a percentage of max RPM.
void SetTarget(Speed *);

//Declare function that takes in the desired speed and returns a speed value capped between zero and max fan rpm.
int SpeedValidate(int);

void SetPWM(Time *, Speed *);

/*

//Declare function that returns speed value based on: user input, target speed, and measured current fan speed
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

//Declare function that returns speed value based on: temperature sensor
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

#endif
