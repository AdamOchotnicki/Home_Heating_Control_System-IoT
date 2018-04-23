Group E1 - Internet of Things.

# Home Heating Control System - college project 2.
 
 Author: Adam Ochotnicki
 email: adam.ochotnicki@mail.itsligo.ie

 Description:
    The program is switched on and off by pressing the button.
    The green LED indicates when the program is working.
    The temperature sensor reads the ambient temperature, which is converted to degrees Celsius.
    If the ambient temperature exceeds the level specified in "threshold",
    the red LED imitating the heating switch is switched off.
    The red LED (heating switch) will be turned on again when the temperature
    drops below the level given in "threshold".
    The program also includes a clock.

    It is possible to communicate with and control the program / device in the LAN via WiFi.
    Commands:
    "/arduino/onoff/0"       -> shows if the program is running
    "/arduino/onoff/0/1"     -> turning ON the program
    "/arduino/onoff/0/0"     -> turning OFF the program

    "/arduino/temp/0"        -> shows the current temperature
    
    "/arduino/settemp/0"     -> shows the current value of "threshold"
    "/arduino/settemp/0/25"  -> assigns a value of 25 to the variable "threshold"

    "/arduino/showtime/0"    -> shows the current time on the device