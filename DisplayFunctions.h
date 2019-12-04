
#ifndef DISPLAYFUNCTIONS_H
#define DISPLAYFUNCTIONS_H

//----------------------------------------------------- Display Functions -----------------------------------------------------//

//Declare function that updates diplay based on current status of display and recent system changes
void UpdateDisplay(volatile int*);

//Declare function that takes multi digit value and encodes into active segments for hex display HexA
int MultiDigitEncoder (int);

//Declare function that takes char from info string and encodes into active segments for hex display
int CharEncoder(char);

//Declare function that simply clears the hex display
void ClearDisplay(void);

//----------------------------------------------------- Scrolling Functions -----------------------------------------------------//

//Declare function that sets up scrolling of new string
void ScrollSetup(int);

//Declare function that generates information string depending on current mode set.
void GetInfoString(int, int);

//Declare function that randomly selects Romeo and Juliet extract and stores in infoStr array
void GetJuliet(void);

//Declare function that writes scrolling text to hex displays
void ScrollRun(void);

//Declare function that writes encoded display message to HEX displays
void ScrollOut(int);

#endif
