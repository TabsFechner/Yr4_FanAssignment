
#ifndef DISPLAYFUNCTIONS_H
#define DISPLAYFUNCTIONS_H

#include "CustomTypes.h"

//----------------------------------------------------- Display Functions -----------------------------------------------------//

//Function prepares and calls the two major functions used to update the HEX display. 
void DisplayManage(Time *, Speed *, Mode *);

//Function updates information to be displayed on HEX displays, either if the system mode has 
//recently changed or if there has been no user input for over 2 minutes, by initiating the 
//scrolling text setup.
void UpdateInfo(Display *, Time *, Mode *, Speed *);

//Function updates the display once the necessary display information changes have been made. If
//no scrolling text is to be displayed, the display output will be set to show live information 
//about the system mode and fan speed.
void UpdateDisplay(Display *, Time *, Mode *, Speed *);

//Function takes multi digit value and encodes into active segments for HEX display HexA.
int MultiDigitEncoder (int);

//Function takes character from display information string and encodes into active segments
//for HEX display.
int CharEncoder(char);

//Define function that simply clears the entire HEX display
void ClearDisplay(void);

//----------------------------------------------------- Scrolling Functions -----------------------------------------------------//

//Function gets display information string based on scrolling purpose.
void ScrollSetup(Display *, Mode *, Speed *);

//Function generates information string based on current system mode description and 
//fan speed.
void GetInfoString(Display *, Mode *, Speed *);

//Function randomly selects 1 of 8 Romeo and Juliet extract to be used as display 
//information string.
void GetJuliet(Display *);

//Function, after a given time period, encodes current character from display information 
//string and decodes into output for HEX display. After calling a function to scroll 
//display output, the current character index is incremented.
void ScrollRun(Display *, Time *, volatile int *);

//Function shifts display along and writes new character into display.
void ScrollOut(int);

#endif
