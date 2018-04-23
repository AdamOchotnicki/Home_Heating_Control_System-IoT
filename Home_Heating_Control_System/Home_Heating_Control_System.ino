/*
 Group E1 - Internet of Things.
 Home Heating Control System - college project 2.
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
    
    "Examples of other possible commands:" - THESE COMMANDS ARE NOT ACTIVE, THE PROJECT DOES NOT PROVIDE THEIR USE!
    "/arduino/digital/13"     -> digitalRead(13)
    "/arduino/digital/13/1"   -> digitalWrite(13, HIGH)
    "/arduino/analog/2/123"   -> analogWrite(2, 123)
    "/arduino/analog/2"       -> analogRead(2)
    "/arduino/mode/13/input"  -> pinMode(13, INPUT)
    "/arduino/mode/13/output" -> pinMode(13, OUTPUT)
*/

const int SENSOR_PIN = A2;          // grove - temperature sensor connect to A2
const int BUTTON_PIN = 2;           // button connected to D2
const int PROGRAM_ON_LED_PIN = 3;   // program ON LED (green) connected to D3
const int HEATING_LED_PIN = 4;      // heating LED (red) connected to D4
const int SET_TIME_REMINDER = 3;    // after 3 minutes from boot a reminder should be sent to set the time

const int B=4275;            // B value of the thermistor

int threshold = 27;          // setting temperature when the LED should turn off
int buttonState = 0;         // variable for button state it is off by default
int programRun = 0;          // variable for program loop it is off by default
int sensorValue;             // variable to store the value coming from the sensor
float temperature;           // variable for temperature value
float R;                     // another variable for temperature calculation
long runningTime = 0;        // variable for running time (seconds)
long temporaryTime = 0;      // variable to observe the change in time
int seconds = 0;             // seconds variable
int minutes = 0;             // minutes variable
int hours = 0;               // hours variable
int setTimeCounter = 0;      // we can see if the time has been set

#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>

// Listen to the default port 5555, the Yún webserver
// will forward there all the HTTP requests you send
BridgeServer server;

void setup()
{
  // Bridge startup
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  digitalWrite(13, HIGH);

  // Listen for incoming connection only from localhost
  // (no one from the external network could connect)
  server.listenOnLocalhost();
  server.begin();
  
  // declare buttonPin as an INPUT:
  pinMode(BUTTON_PIN, INPUT);
  
  // declare the ledPin as an OUTPUT:
  pinMode(HEATING_LED_PIN, OUTPUT);
  pinMode(PROGRAM_ON_LED_PIN, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop()
{
  // start counting the time
  runningTime = (millis() / 1000);
  
  // CLOCK
  // Comparing whether there has been a change in time.
  if (temporaryTime != runningTime)
  {
    temporaryTime = runningTime;
    seconds++;  // add second

    if (seconds == 60)
    {
      seconds = 0;  // reset seconds
      minutes++;  // add minute

      if (minutes == 60)
      {
        minutes = 0;  //reset minutes
        hours++;  // add hour

        if (hours == 24)
        {
          hours = 0;  // reset hours
        }
      }
    }
  }

  // reminder to set the time
  if (setTimeCounter == 0)
  {
    if (minutes == SET_TIME_REMINDER)
    {
      // use pushingbox to send an email with a reminder
    }
  }

  // Get clients coming from server
  BridgeClient client = server.accept();

  // There is a new client?
  if (client)
  {
    // Process request
    process(client);
    
    // Close connection and free resources.
    client.stop();
  }
  delay(50); // Poll every 50ms
  
  // read the state of the pushbutton value:
  buttonState = digitalRead(BUTTON_PIN);
  delay(270);        // delay in between reads for stability
  
  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH)
  {
     if(programRun == 0)
     {
         programRun = 1;
     }
     else
     {
         programRun = 0;
     }
  }

  if (programRun != 0)
  {
    // turn the program LED on (HIGH is the voltage level)
    digitalWrite(PROGRAM_ON_LED_PIN, HIGH);
    
    // read the input on analog pin A2
    sensorValue = analogRead(SENSOR_PIN);
    
    // temperature calculation
    R = (1023.0/((float)sensorValue)-1.0);
    temperature = 1.0/(log(R)/B+1/298.15)-273.15; //convert to temperature via datasheet
    
    // print out the value you read:
    Serial.print("temperature = ");
    Serial.println(temperature);
    delay(1000);        // delay in between reads for stability

    // the decision to turn the heating on / off
    if (temperature < threshold) 
    digitalWrite(HEATING_LED_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    else
    digitalWrite(HEATING_LED_PIN, LOW); // turn the LED off
  }
  else
  {
    // turning off everything
    digitalWrite(PROGRAM_ON_LED_PIN, LOW); // turn the LED off
    digitalWrite(HEATING_LED_PIN, LOW); // turn the LED off
  }
}

void process(BridgeClient client)
{
  // read the command
  String command = client.readStringUntil('/');

  // is "temp" command?
  if (command == "temp")
  {
    checkTemperature(client);
  }

  // is "onoff" command?
  if (command == "onoff")
  {
    onOff(client);
  }

    // is "settemp" command?
  if (command == "settemp")
  {
    setTemperature(client);
  }

  // is "sv" command?
  if (command == "sv")
  {
    showVariables(client);
  }

  // is "showtime" command?
  if (command == "showtime")
  {
    showTime(client);
  }

  /*// is "digital" command?
  if (command == "digital")
  {
    digitalCommand(client);
  }

  // is "analog" command?
  if (command == "analog")
  {
    analogCommand(client);
  }

  // is "mode" command?
  if (command == "mode")
  {
    modeCommand(client);
  }*/
}

void showTime(BridgeClient client)
{
  client.print(F("Time "));
  client.print(hours);
  client.print(F(" : "));
  client.print(minutes);
  client.print(F(" : "));
  client.println(seconds);
}

void showVariables(BridgeClient client)
{
    client.print(F("threshold : "));
    client.println(threshold);
    client.print(F("programRun : "));
    client.println(programRun);
    client.print(F("temperature : "));
    client.println(temperature);
    client.print(F("seconds : "));
    client.println(seconds);
    client.print(F("minutes : "));
    client.println(minutes);
    client.print(F("hours : "));
    client.println(hours);
    client.print(F("setTimeCounter : "));
    client.println(setTimeCounter);
    
}

void checkTemperature(BridgeClient client)
{
    // Send feedback to client
    if (programRun == 0)
    {
      client.print(F("Program is currently OFF."));
    }
    else
    {
    client.print(F("Current temperature is : "));
    client.print(temperature);
    }
}

void onOff(BridgeClient client)
{
  // fictitious number of pin
  int pin;

  // Read pin number
  pin = client.parseInt();
  
  // If the next character is a '/' it means we have an URL
  // with a value: "/onoff/0/1" or "/onoff/0/0"
  if (client.read() == '/')
  {
    // reading of the new temperature value
    programRun = client.parseInt();

    if (programRun == 1)
    {
    // Send feedback to client
    client.println(F("Turning on the program. "));
    client.println(F("Program is now ON."));
    }
    else
    {
    // Send feedback to client
    client.println(F("Turning off the program. "));
    client.println(F("Program is now OFF."));
    }
  }
  else
  {
    // checking if the program is running
    if (programRun == 1)
    {
    // Send feedback to client
    client.print(F("Program is ON."));
    }
    else
    {
    // Send feedback to client
    client.print(F("Program is OFF."));
    }
  }
}

void setTemperature(BridgeClient client)
{
  // fictitious number of pin
  int pin;

  // Read pin number
  pin = client.parseInt();
  
  // If the next character is a '/' it means we have an URL
  // with a value like: "/settemp/0/25"
  if (client.read() == '/')
  {
    // reading of the new temperature value
    threshold = client.parseInt();

    // Send feedback to client
    client.print(F("Threshold set to : "));
    client.print(threshold);
  }
  else
  {
    // Send feedback to client
    client.print(F("Threshold value is currently : "));
    client.print(threshold);
  }
}

/*void digitalCommand(BridgeClient client)
{
  int pin, value;

  // Read pin number
  pin = client.parseInt();

  // If the next character is a '/' it means we have an URL
  // with a value like: "/digital/13/1"
  if (client.read() == '/')
  {
    value = client.parseInt();
    digitalWrite(pin, value);
  }
  else
  {
    value = digitalRead(pin);
  }

  // Send feedback to client
  client.print(F("Pin D"));
  client.print(pin);
  client.print(F(" set to "));
  client.println(value);

  // Update datastore key with the current pin value
  String key = "D";
  key += pin;
  Bridge.put(key, String(value));
}

void analogCommand(BridgeClient client)
{
  int pin, value;

  // Read pin number
  pin = client.parseInt();

  // If the next character is a '/' it means we have an URL
  // with a value like: "/analog/5/120"
  if (client.read() == '/')
  {
    // Read value and execute command
    value = client.parseInt();
    analogWrite(pin, value);

    // Send feedback to client
    client.print(F("Pin D"));
    client.print(pin);
    client.print(F(" set to analog "));
    client.println(value);

    // Update datastore key with the current pin value
    String key = "D";
    key += pin;
    Bridge.put(key, String(value));
  }
  else
  {
    // Read analog pin
    value = analogRead(pin);

    // Send feedback to client
    client.print(F("Pin A"));
    client.print(pin);
    client.print(F(" reads analog "));
    client.println(value);

    // Update datastore key with the current pin value
    String key = "A";
    key += pin;
    Bridge.put(key, String(value));
  }
}

void modeCommand(BridgeClient client)
{
  int pin;

  // Read pin number
  pin = client.parseInt();

  // If the next character is not a '/' we have a malformed URL
  if (client.read() != '/')
  {
    client.println(F("error"));
    return;
  }

  String mode = client.readStringUntil('\r');

  if (mode == "input")
  {
    pinMode(pin, INPUT);
    // Send feedback to client
    client.print(F("Pin D"));
    client.print(pin);
    client.print(F(" configured as INPUT!"));
    return;
  }

  if (mode == "output")
  {
    pinMode(pin, OUTPUT);
    // Send feedback to client
    client.print(F("Pin D"));
    client.print(pin);
    client.print(F(" configured as OUTPUT!"));
    return;
  }

  client.print(F("error: invalid mode "));
  client.print(mode);
}*/
