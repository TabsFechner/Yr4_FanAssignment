
#ifndef DISPLAYFUNCTIONS_H
#define DISPLAYFUNCTIONS_H

#include "CustomTypes.h"

//----------------------------------------------------- Display Functions -----------------------------------------------------//

//Declare function that updates diplay based on current status of display and recent system changes
void UpdateDisplay(Time *, Speed *, Mode *);

//Declare function that takes multi digit value and encodes into active segments for hex display HexA
int MultiDigitEncoder (int);

//Declare function that takes char from info string and encodes into active segments for hex display
int CharEncoder(char);

//Declare function that simply clears the hex display
void ClearDisplay(void);

//----------------------------------------------------- Scrolling Functions -----------------------------------------------------//

//Declare function that sets up scrolling of new string
void ScrollSetup(int, char *, Mode *, Speed *);

//Declare function that generates information string depending on current mode set.
void GetInfoString(char *, Mode *, Speed *);

//Declare function that randomly selects Romeo and Juliet extract and stores in infoStr array
void GetJuliet(char *);

//Declare function that writes scrolling text to hex displays
void ScrollRun(Time *, int *, volatile int *, char *);

//Declare function that writes encoded display message to HEX displays
void ScrollOut(int);

#endif
