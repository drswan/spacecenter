/*
                                               __
   _________  ____ _________  ________  ____  / /____  _____
  / ___/ __ \/ __ `/ ___/ _ \/ ___/ _ \/ __ \/ __/ _ \/ ___/
 (__  ) /_/ / /_/ / /__/  __/ /__/  __/ / / / /_/  __/ /    
/____/ .___/\__,_/\___/\___/\___/\___/_/ /_/\__/\___/_/     
    /_/ V1.1
       
  Arduino SpaceBucket Climate Controller V1.1
  Created by David Rule - /u/dr_swan
  Required Libraries: TFT, SPI, Adafruit_Sensor, DHT
  UNO Pins Used:
                                      +-----+
         +----[PWR]-------------------| USB |--+
         |                            +-----+  |
         |         GND/RST2  [ ][ ]            |
         |       MOSI2/SCK2  [ ][ ]  A5/SCL[ ] |
         |          5V/MISO2 [ ][ ]  A4/SDA[ ] |
         |                             AREF[ ] |
         |                              GND[ ] |
         | [ ]N/C                    SCK/13[ ] |
         | [ ]IOREF                 MISO/12[ ] |
         | [ ]RST                   MOSI/11[ ]~|
         | [ ]3V3    +---+           TFT 10[*]~|
         | [ ]5v    -| A |-           TFT 9[*]~|
         | [ ]GND   -| R |-               8[*] |
         | [ ]GND   -| D |-                    |
         | [ ]Vin   -| U |-               7[ ] |
         |          -| I |-               6[ ]~|
         | [ ]A0    -| N |-               5[ ]~|
         | [ ]A1    -| O |-               4[ ] |
         | [ ]A2     +---+      N PWM OUT 3[*]~|
         | [ ]A3               DHT22 DATA 2[*] |
         | [ ]A4      RST SCK MISO        1[ ] |
         | [ ]A5      [ ] [ ] [ ]         0[ ] |
         |            [ ] [ ] [ ]              |
         |  UNO_R3    GND MOSI 5V  ____________/
          \_______________________/
Future Additions: Soil Moisture Graphic
*/

// Arduino LCD libraries
#include <TFT.h>
#include <SPI.h>

// DHT Sensor libraries
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// Pin Definitions
#define cs   10
#define dc   9
#define rst  8
#define DHTPIN 2
#define DHTTYPE DHT22

// PWM fan
const int PWMPin = 3;
int DefaultSpeed = 30;   // default fan speed duty cycle (0-79)

// Climate
int MinTemp = 21;
int MaxTemp = 25;

// create an instance of the library
TFT TFTscreen = TFT(cs, dc, rst);

// char array to print to the screen (not sure why this is needed actually)
char tempPrintout[6];
char humPrintout[6];
char moiPrintout[6];

DHT_Unified dht(DHTPIN, DHTTYPE);

// sensor delay
uint32_t delayMS;

void setup() {
  TFTscreen.begin();

  // clear the screen with a black background
  TFTscreen.background(0, 0, 0);

  // write the static text to the screen
  // set the font color to white
  TFTscreen.stroke(255, 255, 255);
  // set the font size
  TFTscreen.setTextSize(1);
  // write static text
  TFTscreen.text("Temperature", 0, 0);
  TFTscreen.text(" *C", 42, 10);
  TFTscreen.text("Humidity", 0, 26);
  TFTscreen.text(" %", 42, 36);
  TFTscreen.text("Soil Moisture", 83, 0);
  TFTscreen.text("Fan ", 0, 54);

  // set the font color to something else
  TFTscreen.stroke(255, 255, 255);
  TFTscreen.text("Dry:    0-300", 83, 26);
  TFTscreen.text("Humid:300-700", 83, 36);
  TFTscreen.text("Wet:  700-950", 83, 46);
  // ste the font size very large for the loop
  TFTscreen.setTextSize(2);

  // serial monitor
  Serial.begin(9600);
  // initialize dht sensor
  dht.begin();
  Serial.println("SpaceCenter V1.0");

  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("DHT Temp");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" *C");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" *C");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" *C");
  Serial.println("------------------------------------");

  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("DHT Hum");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println("%");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println("%");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println("%");
  Serial.println("------------------------------------");

  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;

  // generate 25kHz PWM pulse rate on Pin 3
  pinMode(PWMPin, OUTPUT);   // OCR2B sets duty cycle
  // Set up Fast PWM on Pin 3
  TCCR2A = 0x23;     // COM2B1, WGM21, WGM20
  // Set prescaler
  TCCR2B = 0x0A;   // WGM21, Prescaler = /8
  // Set TOP and initialize duty cycle to zero(0)
  OCR2A = 79;    // TOP DO NOT CHANGE, SETS PWM PULSE RATE
  OCR2B = DefaultSpeed;    // default duty cycle for Pin 3 (0-79) generates 1 500nS pulse even when 0 :(
}

void loop() {
  //serialmonitor(); //for debug & serial monitoring, device resets faster without it
  tftsensorprint();
  fan();
}

void serialmonitor() {
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println("Error reading temperature!");
  }
  else {
    Serial.print("Temperature: ");
    Serial.print(event.temperature);
    Serial.println(" *C");
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("Error reading humidity!");
  }
  else {
    Serial.print("Humidity: ");
    Serial.print(event.relative_humidity);
    Serial.println("%");
  }

}

void tftsensorprint() {
  // Read the value of the sensor
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  String tempVal = String(event.temperature);

  dht.humidity().getEvent(&event);
  String humVal = String(event.relative_humidity);
  String moiVal = String(analogRead(A0));

  // convert the reading to a char array
  tempVal.toCharArray(tempPrintout, 5);
  humVal.toCharArray(humPrintout, 5);
  moiVal.toCharArray(moiPrintout, 5);

  // set the font color
  TFTscreen.stroke(0, 200, 255);
  // print the sensor value
  TFTscreen.text(tempPrintout, 0, 10);
  TFTscreen.text(humPrintout, 0, 36);
  TFTscreen.text(moiPrintout, 126, 10);
  // wait for a moment
  delay(delayMS);
  // erase the text you just wrote
  TFTscreen.stroke(0, 0, 0);
  TFTscreen.text(humPrintout, 0, 36);
  TFTscreen.text(tempPrintout, 0, 10);
  TFTscreen.text(moiPrintout, 126, 10);
  TFTscreen.text("OFF", 0, 64);
  TFTscreen.text("HI", 0, 64);
  TFTscreen.text("LO", 0, 64);

}

void fan() {
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  float CurrentTemp = event.temperature;
  TFTscreen.stroke(0, 200, 255);
  if (CurrentTemp < MinTemp) {
    OCR2B = 0; // (0-79)
    TFTscreen.text("OFF", 0, 64);
  }
  else if (CurrentTemp > MaxTemp) {
    OCR2B = 79; // (0-79)
    TFTscreen.text("HI", 0, 64);
  }
  else {
    OCR2B = DefaultSpeed;
    TFTscreen.text("LO", 0, 64);
  }
}

