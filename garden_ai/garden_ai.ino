/*
  Version : 1.0.0.3
*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>        //Declare Library screen I2C
#include "Wire.h"
#include "SPI.h"                      // not used here, but needed to prevent a RTClib compile error
#include "RTClib.h"
/*
  The circuit:
    SD card attached to SPI bus as follows:
  CS      -> 4
  SCK     -> 13
  MOSI    -> 11
  MISO    -> 12
  VCC     -> 5V
  GND     -> GND
*/
#include <SPI.h>                      //Declare Library Micro SD
#include <SD.h>                       //Declare Library Micro SD

RTC_DS3231 RTC;
LiquidCrystal_I2C lcd(0x27, 16, 2);   //Set the LCD address to 0x27 for a 16 chars and 2 line display

int re_a_p = 6;       //Pin of Relay Module (digital)
int led_pin_pw = 7;
int lcd_pin_pw = 8;
int soil_pin_pw = 9;
int rtc_pin_pw = 10;

const int chipSelect = 4;             //Declare Library Micro SD
boolean isFoundSd = false;            //Declare Check Micro SD

void setup() {
  Serial.begin(9600);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    isFoundSd = false;
  } else {
    Serial.println("card initialized.");
    isFoundSd = true;
  }

  pinMode(led_pin_pw, OUTPUT);      //LED
  digitalWrite(led_pin_pw, HIGH);
  pinMode(lcd_pin_pw, OUTPUT);      //LCD
  digitalWrite(lcd_pin_pw, HIGH);
  pinMode(soil_pin_pw, OUTPUT);     //Soil
  digitalWrite(soil_pin_pw, HIGH);
  pinMode(rtc_pin_pw, OUTPUT);     //RTC
  digitalWrite(rtc_pin_pw, HIGH);

  pinMode(re_a_p, OUTPUT);    //Set Pin connected to Relay as an OUTPUT
  digitalWrite(re_a_p, LOW);  //Set Pin to LOW to turn Relay OFF

  Wire.begin();
  RTC.begin();

  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  DateTime now = RTC.now();

  lcd.begin();                    // initialize the LCD
  lcd.setCursor(3, 0);
  lcd.print("1.0.0.3");
  lcd.setCursor(0, 1);
  lcd.print("Initialize LCD");

  Serial.println("Reading From the Sensor ...");
  delay(3000);
}

void loop() {
  int sm_s_p = A0;      //Pin of Soil Moisture Sensor (analog)
  String str_temp = "";
  int sm_s_ov ;         //Soil Moisture Sensor output value (analog value 0-1023)
  int sm_s_ov_temp;
  int sm_chk = 50;      //Condition check percentage moisture value
  DateTime now = RTC.now();
  str_temp += String(now.year(), DEC);
  str_temp += "/";
  str_temp += String(now.month(), DEC);
  str_temp += "/";
  str_temp += String(now.day(), DEC);
  str_temp += " ";
  str_temp += String(now.hour(), DEC);
  str_temp += ":";
  str_temp += String(now.minute(), DEC);
  str_temp += ":";
  str_temp += String(now.second(), DEC);
  Serial.println("DateTime = " + str_temp);

  sm_s_ov = analogRead(sm_s_p);       //Get value from Soil Moisture Sensor
  sm_s_ov_temp = sm_s_ov;
  sm_s_ov = map(sm_s_ov, 1023, 200, 0, 100); //Map Moisture value to percentage, 550(550-1023) = 0%(dry soil) and 0 = 100%(moisture soil)
  str_temp += " , ";
  str_temp += String(sm_s_ov_temp);
  str_temp += " , ";
  str_temp += String(sm_s_ov);
  Serial.println("Moisture value(0-1023) = " + String(sm_s_ov_temp));
  Serial.println("Moisture value of percentage = " + String(sm_s_ov));

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Moisture =     %");
  lcd.setCursor(11, 0);
  lcd.print(sm_s_ov);
  lcd.setCursor(0, 1);
  lcd.print("Valve    = ");

  if (sm_s_ov < sm_chk) {
    /*
      Dry soil
      Soil Moisture Sensor output value less than condition percentage moisture value
      Open Solenoid Valve
    */
    lcd.backlight();
    digitalWrite(re_a_p, HIGH);   //Turn Relay ON. Valve ON.
    lcd.setCursor(11, 1);
    lcd.print("ON");
    str_temp += " , ";
    str_temp += "ON";
    Serial.println("Relay is ON. Solenoid Valve is ON.");
  } else {
    digitalWrite(re_a_p, LOW);        //Turn Relay OFF. Valve OFF.
    lcd.setCursor(11, 1);
    lcd.print("OFF");
    str_temp += " , ";
    str_temp += "OFF";
    Serial.println("Relay is OFF. Solenoid Valve is OFF.");
    lcd.noBacklight();
  }

  if (isFoundSd) {
    char _fn[] = "20001231.log";
    _fn[0] = (now.year() / 1000) % 10 + '0'; //To get 1st digit from year()
    _fn[1] = (now.year() / 100) % 10 + '0'; //To get 2nd digit from year()
    _fn[2] = (now.year() / 10) % 10 + '0'; //To get 3rd digit from year()
    _fn[3] = now.year() % 10 + '0'; //To get 4th digit from year()
    _fn[4] = now.month() / 10 + '0'; //To get 1st digit from month()
    _fn[5] = now.month() % 10 + '0'; //To get 2nd digit from month()
    _fn[6] = now.day() / 10 + '0'; //To get 1st digit from day()
    _fn[7] = now.day() % 10 + '0'; //To get 2nd digit from day()
    _fn[8] = '.';
    _fn[9] = 'l';
    _fn[10] = 'o';
    _fn[11] = 'g';
    Serial.println(_fn);
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    File _file = SD.open(_fn, FILE_WRITE);
    // if the file opened okay, write to it:
    if (_file) {
      Serial.println("Writing to file.");
      Serial.println("Data = " + str_temp);
      _file.println(str_temp);
      _file.close();
      Serial.println("Close file.");
    } else {
      // if the file didn't open, print an error:
      Serial.println("Error opening file.");
    }
  }
  Serial.println("Delay 5 sec.");
  delay(5000);
}
