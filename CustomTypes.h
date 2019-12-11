
#ifndef CUSTOMTYPES_H
#define CUSTOMTYPES_H

typedef struct Mode_t{
	int isOn;
	int mode;
	int changed;
	char description[20];
}Mode;

typedef struct Speed_t{
	int pid;
	int temp;
	int demand;
	int measured;
	int target;
}Speed;

typedef struct Time_t{
	int t1;
	int t2;
	float time;
}Time;

typedef struct Display_t{
	int iDisp;
	int scrl;
	int scrlMode;
	int nTime;
	char infoStr[100];
}Display;

#endif
