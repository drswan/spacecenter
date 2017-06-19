# spacecenter
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
