//#include "I2C_LED.h"

//Library from BlinkM
#include "BlinkM_funcs.h"



// ===============================================
String statusArray [3] = {"off", "on", "error"};
String colorArray [8] = {"white", "red", "green", "blue", "cyan", "magenta", "yellow", "black"};
String actionArray [3]= {"solid", "blink", "breathe"};


// defining array of "status"
struct channel {
  String status[3];
  String color[3];
  String action[3];
};

channel IPC_LED_1 = {
  {"off", "on", "error"},
  {"white", "green", "red"},
  {"solid", "blink", "breathe"}
};

// Serial: this is the COMMAND variable
// it holds the most recent command from the serial_1 port
String serial1Command = "";

// set flag once a '*' is recieved:
// the '*' represents the start of a new command
// all commands need to start with an '*'
bool serialCommandFlag = false;



// ===============================================
void setup() {

    // Open serial port (via USB port)
    Serial.begin(9600);

    // Open serial port 1 (via Tx/Rx pins)
    Serial1.begin(115200);

    // start I2C com with the BlinkM smart LED
    BlinkM_begin();
    BlinkM_stopScript(0x00);
}



// ===============================================
// runs continuously
void loop() {

}



// ===============================================
// Interrupt for serial 0 (USB)
// called when data is rady to be read
void serialEvent()
{
    char c = Serial.read();
    //Serial.println(c);
    Serial1.write(c);
}

// Interrupt for serial 1 (Tx/Rx)
// called when data is rady to be read
void serialEvent1()
{
    char c = Serial1.read();

    if (c == '*')
    {
      serial1Command = "";
      serialCommandFlag = true;
    }
    else if (serialCommandFlag == true && c == ':')
    {
      parseSerialCommand(serial1Command);
      serial1Command = "";
      serialCommandFlag = false;
      Serial.println("");
    }
    else if (serialCommandFlag == true)
    {
      serial1Command = serial1Command + c;
      Serial.print(c);
    }
}

// Parse the command read over serial1
void parseSerialCommand(String message)
{
  // print the new message ot the serial port 0 (USB)
  Serial.print("\n==============================");
  Serial.print("\nNew command is== ");
  Serial.print(message);

  // parse message
  char command = message[1];
  int value = message.substring(2).toInt();

  Serial.print("/n");
  Serial.print(command);
  Serial.print(",");
  Serial.print(value);

  // VOLTAGE
  if (command == "H")
  {
    // BlinkM_fadeToRGB(0x00, 0x00, 0xff, 0xff);
    updateLED(0x02, 0xff, 0xff);
    Serial.print("\nNew VOLTAGE is: ");
    Serial.print(command);
  }
  // CURRENT
  else if (command == "c")
  {
    updateLED(0x09, 0xff, 0xff);
    Serial.print("\nNew CURRENT is: ");
    Serial.print(command);
  }
  else if (command == "error")
  {
    updateLED(0x03, 0xff, 0xff);
  }
  else
  {
    updateLED(0x0A, 0xff, 0xff);
  }
}

// Update LED script based on the new serial command recieved
void updateLED(byte script_id, byte fadespeed, byte timeadj) {
  BlinkM_stopScript(0x00);
  BlinkM_playScript(0x00, script_id, 0x00, 0x00);
  BlinkM_setFadeSpeed(0x00, fadespeed);
  BlinkM_setTimeAdj(0x00, timeadj);
}








/*
// defining array of colors

// MY: defining random array of colors
//String myColors[2] = {"blue", "red"};

// define the LEDcolor variable
String LEDcolor = "";

int reading = 0;
double volts = 0.0;
// define the A5 as the photoresistor
int photoR = A0;

// runs once at the beginning
void setup() {

    // Open serial port
    Serial.begin(9600);

    // Defining IoT functions
    Particle.function("color", changeColor);

    // Defining IoT variables
    // Particle.variable("newColor", &LEDcolor, STRING);

    // Publish LED chanage of color
    // particle.publish("newColor")


    // Defining internet variables for reading the light level
    Spark.variable("analog", &reading, INT);
    Spark.variable("volts", &volts, DOUBLE);

    // join i2c bus
    Wire.begin();

    // turn off any blinkm scripts
    Wire.beginTransmission(0x00);
    Wire.write('o');
    Wire.endTransmission();

    // Running a built-in scripts
    Wire.beginTransmission(0x00);
    // fade to a color
    // Wire.write('c');

    // set fade speed
    BlinkM_setFadeSpeed(0x00, 0xFF);

    //Wire.write('f');
    // set the RGB bytes
    //Wire.write(0x09);


    // set tiem adjust
    Wire.write('t');
    // set the RGB bytes
    Wire.write(0xAA);

    Wire.write('p');
    // set the RGB bytes
    Wire.write(0x05);
    Wire.write(0x00);
    Wire.write(0x00);
    // send the transmission
    Wire.endTransmission();
}

// runs continuously
void loop() {
    reading = analogRead(photoR);
    volts = reading * 3.3 / 4096.0;
    Serial.print(volts);
    Serial.print(" - ");
    delay(500);
}


// Print the color over serial port to the terminal
void printSerial(String color) {
    Serial.println("The user selected the color ");
    Serial.print(color);
    Serial.print('\n');
}

// transmit to the I2C
void transmitColor(byte red, byte green, byte blue) {
  // turn off any blinkm scripts
  Wire.beginTransmission(0x00);
  Wire.write('o');
  Wire.endTransmission();

  // begin transmission to 0x00, which will send to all connected I2Cs
    Wire.beginTransmission(0x00);
    // fade to a color
    Wire.write('c');
    // set the RGB bytes
    Wire.write(red);
    Wire.write(green);
    Wire.write(blue);
    // send the transmission
    Wire.endTransmission();
}

// LED blinking
//void LEDblink(String color, int duration) {

//}

// LED breething

// set color based on name
int changeColor(String color_name) {

    // publish
    Particle.publish("newColor");

    // console update
    printSerial(color_name);

    if (color_name == "red") {
        transmitColor(0xff, 0x00, 0x00);
    } else if (color_name == "green") {
        transmitColor(0x00, 0x80, 0x00);
    } else if (color_name == "blue") {
        transmitColor(0x00, 0x00, 0xff);
    } else if (color_name == "cyan") {
        transmitColor(0x00, 0xff, 0xff);
    } else if (color_name == "white") {
        transmitColor(0xff, 0xff, 0xff);
    } else if (color_name == "oldlace") {
        transmitColor(0xfd, 0xf5, 0xe6);
    } else if (color_name == "warmwhite") {
        transmitColor(0xfd, 0xf5, 0xe6);
    } else if (color_name == "purple") {
        transmitColor(0x80, 0x00, 0x80);
    } else if (color_name == "magenta") {
        transmitColor(0xff, 0x00, 0xff);
    } else if (color_name == "yellow") {
        transmitColor(0xff, 0xff, 0x00);
    } else if (color_name == "orange") {
        transmitColor(0xff, 0xa5, 0x00);
    } else if (color_name == "pink") {
        transmitColor(0xff, 0xc0, 0xcb);
    } else if (color_name == "dim") {
        transmitColor(0x04, 0x04, 0x04);
    }
}
*/
