Group E1 - Internet of Things.

# Home Heating Control System - college project 2.
 
 Author: Adam Ochotnicki
 email: adam.ochotnicki@mail.itsligo.ie


Description:

The heating is switched on and off by pressing the button.
    WARNING!
    The heating must be turned off by the user.
    Also disables temperature control heating: clock control, boost or constant if they are on.
    The button uses "forcedon" feature.
    
The green LED indicates when the temperature control heating is working.

The temperature sensor reads the ambient temperature, which is converted to degrees Celsius.
    If the ambient temperature exceeds the level specified in "threshold",
    the red LED imitating the heating switch is switched off.
    The red LED (heating switch) will be turned on again when the temperature
    drops below the level given in "threshold".
    
The program also includes a clock.
    The temperature control heating can be :
    - constant,
    - clock controled - between start time and end time, available only when the clock has been set,
    - boost - for a certain amount of time, for example: 30 minutes or 2 and a half hours.
    The lowest priority is clock control, then boost, then constant.
    Examples :
      If the boost completes or has been turned off, the program will return to clock control if it was previously set.
      If constant has been turned off, the program will return to clock control if it has been set before,
      even if constant was enabled during the boost.
    
The program includes the "forcedon" function.
    It can be turned on / off via a LAN browser or by using the button mentioned above.
    It allows you to permanently turn on / off the heating without temperature control.
    WARNING!
    The heating must be turned off by the user.
    Also disables temperature control heating: clock control, boost or constant if they are on.


    Commands to communicate with and control the program / device in the LAN via WiFi :
    
    "/arduino/onoff/0"       -> shows if the program is running
    "/arduino/onoff/0/1"     -> turning ON the program
    "/arduino/onoff/0/0"     -> turning OFF the program

    "/arduino/temp/0"        -> shows the current temperature
    
    "/arduino/settemp/0"     -> shows the current value of "threshold"
    "/arduino/settemp/0/25"  -> assigns a value of 25 to the variable "threshold"

    "/arduino/showtime/0"    -> shows the current time on the device