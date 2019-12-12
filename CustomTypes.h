
#ifndef CUSTOMTYPES_H
#define CUSTOMTYPES_H

/*
  Custom struct type Mode used to store system mode information
*/
typedef struct Mode_t{
	int isOn;
	int mode;
	int changed;
	char description[20];
}Mode;

/*
  Custom struct type Speed used to store fan speed information
*/
typedef struct Speed_t{
	int pid;
	int temp;
	int demand;
	int measured;
	int target;
}Speed;

/*
  Custom struct type Time used to create multiple timers of coherent format,
  later used for a variety of purposes.
*/
typedef struct Time_t{
	int t1;
	int t2;
	float time;
}Time;

/*
  Custom struct type Display used to store data about the display and
  display information.
*/
typedef struct Display_t{
	int iDisp;
	int scrl;
	int scrlMode;
	int nTime;
	char infoStr[100];
}Display;

#endif
