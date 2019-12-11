
#ifndef DISPLAYFUNCTIONS_H
#define DISPLAYFUNCTIONS_H

#include "CustomTypes.h"

//----------------------------------------------------- Display Functions -----------------------------------------------------//

//Declare function that updates diplay based on current status of display and recent system changes
void DisplayManage(Time *, Speed *, Mode *);

void UpdateInfo(Display *, Time *, Mode *, Speed *);

void UpdateDisplay(Display *, Time *, Mode *, Speed *);

//Declare function that takes multi digit value and encodes into active segments for hex display HexA
int MultiDigitEncoder (int);

//Declare function that takes char from info string and encodes into active segments for hex display
int CharEncoder(char);

//Declare function that simply clears the hex display
void ClearDisplay(void);

//----------------------------------------------------- Scrolling Functions -----------------------------------------------------//

//Declare function that sets up scrolling of new string
void ScrollSetup(Display *, Mode *, Speed *);

//Declare function that generates information string depending on current mode set.
void GetInfoString(Display *, Mode *, Speed *);

//Declare function that randomly selects Romeo and Juliet extract and stores in infoStr array
void GetJuliet(Display *);

//Declare function that writes scrolling text to hex displays
void ScrollRun(Display *, Time *, volatile int *);

//Declare function that writes encoded display message to HEX displays
void ScrollOut(int);

#endif
