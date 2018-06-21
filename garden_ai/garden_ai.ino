/*
  Version : 1.0.1.1
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

int _re_a_pin[] = {3, 2};
int led_pin_pw = 7;
int lcd_pin_pw = 8;
int _soil_pin_pw[] = {9, 5};
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
  pinMode(rtc_pin_pw, OUTPUT);     //RTC
  digitalWrite(rtc_pin_pw, HIGH);
  int i;
  for (i = 0; i < (sizeof(_soil_pin_pw) / sizeof(int)); i++) {
    pinMode(_soil_pin_pw[i], OUTPUT);     //Soil
    digitalWrite(_soil_pin_pw[i], HIGH);
  }
  for (i = 0; i < (sizeof(_re_a_pin) / sizeof(int)); i++) {
    pinMode(_re_a_pin[i], OUTPUT);    //Set Pin connected to Relay as an OUTPUT
    digitalWrite(_re_a_pin[i], LOW);  //Set Pin to LOW to turn Relay OFF
  }

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
  lcd.print("1.0.1.1");
  lcd.setCursor(0, 1);
  lcd.print("Initialize LCD");

  Serial.println("Reading From the Sensor ...");
  delay(3000);
}

void loop() {
  int _pin_soil_data[] = {A0, A1};//Pin of Soil Moisture Sensor (analog)
  char _str_temp[40] = "";
  int _sm_chk[] = {50, 50};//Condition check percentage moisture value
  int _valve_status[] = {0, 0};
  int sm_s_ov ;         //Soil Moisture Sensor output value (analog value 0-1023)
  int sm_s_ov_temp;

  DateTime now = RTC.now();
  _str_temp[0] = (now.year() / 1000) % 10 + '0'; //To get 1st digit from year()
  _str_temp[1] = (now.year() / 100) % 10 + '0'; //To get 2nd digit from year()
  _str_temp[2] = (now.year() / 10) % 10 + '0'; //To get 3rd digit from year()
  _str_temp[3] = now.year() % 10 + '0'; //To get 4th digit from year()
  _str_temp[4] = '/';
  _str_temp[5] = now.month() / 10 + '0'; //To get 1st digit from month()
  _str_temp[6] = now.month() % 10 + '0'; //To get 2nd digit from month()
  _str_temp[7] = '/';
  _str_temp[8] = now.day() / 10 + '0'; //To get 1st digit from day()
  _str_temp[9] = now.day() % 10 + '0'; //To get 2nd digit from day()
  _str_temp[10] = ' ';
  _str_temp[11] = now.hour() / 10 + '0'; //To get 1st digit from hour()
  _str_temp[12] = now.hour() % 10 + '0'; //To get 2nd digit from hour()
  _str_temp[13] = ':';
  _str_temp[14] = now.minute() / 10 + '0'; //To get 1st digit from minute()
  _str_temp[15] = now.minute() % 10 + '0'; //To get 2nd digit from minute()
  _str_temp[16] = ':';
  _str_temp[17] = now.second() / 10 + '0'; //To get 1st digit from second()
  _str_temp[18] = now.second() % 10 + '0'; //To get 2nd digit from second()
  Serial.print("DateTime = ");
  Serial.println(_str_temp);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Moisture   %   %");
  lcd.setCursor(0, 1);
  lcd.print("Valve 1    2   ");

  int i;
  for (i = 0 ; i < (sizeof(_pin_soil_data) / sizeof(int)) ; i++) {
    sm_s_ov = analogRead(_pin_soil_data[i]);       //Get value from Soil Moisture Sensor
    sm_s_ov_temp = sm_s_ov;
    sm_s_ov = map(sm_s_ov, 1023, 200, 0, 100); //Map Moisture value to percentage, 550(550-1023) = 0%(dry soil) and 0 = 100%(moisture soil)
    _str_temp[19] = ',';
    _str_temp[20] = (sm_s_ov_temp / 1000) % 10 + '0';
    _str_temp[21] = (sm_s_ov_temp / 100) % 10 + '0';
    _str_temp[22] = (sm_s_ov_temp / 10) % 10 + '0';
    _str_temp[23] = sm_s_ov_temp % 10 + '0';
    _str_temp[24] = ',';
    _str_temp[25] = (sm_s_ov / 10) % 10 + '0';
    _str_temp[26] = sm_s_ov % 10 + '0';

    Serial.print("Moisture value(0-1023) = ");
    Serial.println(sm_s_ov_temp);
    Serial.print("Moisture value of percentage = ");
    Serial.println(sm_s_ov);

    if (i == 0) {
      lcd.setCursor(9, 0);
    } else {
      lcd.setCursor(13, 0);
    }
    lcd.print(sm_s_ov);

    if (i == 0) {
      lcd.setCursor(7, 1);
    } else {
      lcd.setCursor(12, 1);
    }
    if (sm_s_ov < _sm_chk[i]) {
      digitalWrite(_re_a_pin[i], HIGH);
      lcd.print("ON");
      _valve_status[i] = 1;
      _str_temp[27] = ',';
      _str_temp[28] = ' ';
      _str_temp[29] = 'O';
      _str_temp[30] = 'N';
      Serial.println(i + " : Relay is ON. Solenoid Valve is ON.");
    } else {
      digitalWrite(_re_a_pin[i], LOW);
      lcd.print("OFF");
      _valve_status[i] = 0;
      _str_temp[27] = ',';
      _str_temp[28] = 'O';
      _str_temp[29] = 'F';
      _str_temp[30] = 'F';
      Serial.println(i + " : Relay is OFF. Solenoid Valve is OFF.");
    }

    _str_temp[31] = ',';
    _str_temp[32] = (i + 1) / 10 + '0';
    _str_temp[33] = (i + 1) % 10 + '0';

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
        Serial.println("Data = " + String(_str_temp));
        _file.println(_str_temp);
        _file.close();
        Serial.println("Close file.");
      } else {
        // if the file didn't open, print an error:
        Serial.println("Error opening file.");
      }
    }
  }
  for (i = 0; i < (sizeof(_valve_status)/sizeof(int)); i++) {
    if (_valve_status[i] == 1) {
      lcd.backlight();
      break;
    }
    if (i == (sizeof(_valve_status) - 1)) {
      lcd.noBacklight();
    }
  }
  Serial.println("Delay 5 sec.");
  delay(5000);
}
