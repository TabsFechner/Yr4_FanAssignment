EE30186 Integrated Engineering: Fan Control Assignment, v1.0 12.12.19

---
GENERAL USAGE
--

  - General System Overview
  
        - The program is intended to control the speed of a single, 3-pin fan, provided by the University
          of Bath. 
          
        - The program, written in C, is to be used in conjunction with an Altera DE1-SoC University 
          Program Board.
          
  - Modes of Use
  
        - There are two modes of operation used to control the fan: open-loop control and closed-loop 
          control. These are outlined in more detail below.

  - Software Outline
  
        - The program is split primarily into three sections: fan functions, display functions, and
          and miscellaneous functions.
          
        - The program spends almost all of its time continuously looping through the following cycle:
        
              - Set target speed
              - Measure fan speed
              - Calculate control
              - Set speed output
              - Update display

---
SYSTEM REQUIREMENTS
--

  - Hardware

         - Altera DE1-SoC University Program Board
         - 3-pin fan (with integrated tachometer)
         - Power supply
         - Extension board
         - 40 pin ribbon cable

  - Software

        - C Debugging Environment able to build the .axf executable (preferred: Eclipse)
        - SoC EDS Command Line Version 18.1 or above
        - Windows OS (preferred: Version 10.0 or above)

  - Files

        - CustomTypes.h
        - DisplayFunctions.c
        - DisplayFunctions.h
        - FanFunctions.c
        - FanFunctions.h
        - Makefile
        - MiscFunctions.c
        - MiscFunctions.h
        - main.c

---
SETUP
--

  - Check the De1 Soc Board has power and that the board is plugged in to the PC via a grey USB cable.
  - A power supply of 12V should be connected to the red (+12V) and black (0V) sockets on the extension 
    board.
  - A white, 3-pin connector handles the tachometer signal from the 12V fan and supplies the fan with
    power.
  - Launch Eclipse (or preferred application) from the SoC EDS Command Shell and open the workspace with
    all required files present.
  - In the Eclipse development environment, build the solution and see that there are no build failures.
  - Turn on the FPGA witht eh red button and load the code onto the board by clicking 'Connect' within
    the debugger.
  - You should see the display on the Board change.

---
USER MANUAL
--
  
  - Switch 0 and switch 1 are used to change the mode of operation.
  
        - If switch 1 is raised, the system will be in temperature mode, which is not implemented yet.
        - With switch 1 low, you can toggle between closed loop and open loop control using switch 0.
        
  - Key 0 is used to turn off the system.
  
  - The grey rotary knob on the extension board is used to control the fan speed.
      
        - See the fan speed change in the display.
        
  - If there is no user input for over 2 minutes, the displays will show random extracts from William 
    Shakespeare's 'Romeo and Juliet'
    
  - Have fun!
