
#ifndef CUSTOMTYPES_H
#define CUSTOMTYPES_H

typedef struct Mode_t{
	int mode;
	int changed;
}Mode;

typedef struct Speed_t{
	int pid;
	int temp;
	int demand;
	int measured;
}Speed;

typedef struct Time_t{
	int t1;
	int t2;
	float time;
}Time;

#endif
