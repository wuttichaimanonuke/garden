/*
Version : 1.0.0.2
*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>        //Declare Library screen I2C

#include "Wire.h"
#include "SPI.h"                      // not used here, but needed to prevent a RTClib compile error
#include "RTClib.h"

RTC_DS3231 RTC;

LiquidCrystal_I2C lcd(0x27, 16, 2);   //Set the LCD address to 0x27 for a 16 chars and 2 line display

int sm_s_p = A0;      //Pin of Soil Moisture Sensor (analog)
int sm_s_ov ;         //Soil Moisture Sensor output value (analog value 0-1023)
int sm_s_ov_temp;
int re_a_p = 6;         //Pin of Relay Module (digital)
int sm_chk = 50;      //Condition check percentage moisture value

String str_temp;

int led_pin_pw = 13;
int lcd_pin_pw = 12;
int soil_pin_pw = 11;
int rtc_pin_pw = 10;

void setup(){
  Serial.begin(9600);
  
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
  lcd.print("V 1.0.0.1");
  lcd.setCursor(0, 1);
  lcd.print("Initialize LCD");
  
  Serial.println("Reading From the Sensor ...");
  delay(3000);
}

void loop(){
  DateTime now;// = RTC.now();
  do {
    now = RTC.now();
    str_temp = "";
    str_temp = str_temp + String(now.year(),DEC);
    str_temp = str_temp + "/";
    str_temp = str_temp + String(now.month(), DEC);
    str_temp = str_temp + "/";
    str_temp = str_temp + String(now.day(), DEC);
    str_temp = str_temp + " ";
    str_temp = str_temp + String(now.hour(), DEC);
    str_temp = str_temp + ":";
    str_temp = str_temp + String(now.minute(), DEC);
    str_temp = str_temp + ":";
    str_temp = str_temp + String(now.second(), DEC);
    Serial.println("DateTime = "+str_temp);
    
    sm_s_ov = analogRead(sm_s_p);       //Get value from Soil Moisture Sensor
    sm_s_ov_temp = sm_s_ov;
    sm_s_ov = map(sm_s_ov,1023,200,0,100); //Map Moisture value to percentage, 550(550-1023) = 0%(dry soil) and 0 = 100%(moisture soil)
    Serial.println("Moisture value(0-1023) = "+String(sm_s_ov_temp));
    Serial.println("Moisture value of percentage = "+String(sm_s_ov));
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Moisture =     %");    
    lcd.setCursor(11, 0);
    lcd.print(sm_s_ov);
    lcd.setCursor(0, 1);
    lcd.print("Valve    = ");
    
    if(sm_s_ov < sm_chk) {
      /*
      Dry soil 
      Soil Moisture Sensor output value less than condition percentage moisture value
      Open Solenoid Valve 
      */
      lcd.backlight();
      digitalWrite(re_a_p, HIGH);   //Turn Relay ON. Valve ON.
      lcd.setCursor(11, 1);
      lcd.print("ON");
      Serial.println("Relay is ON. Solenoid Valve is ON.");
    } else {
      digitalWrite(re_a_p, LOW);        //Turn Relay OFF. Valve OFF.
      lcd.setCursor(11, 1);
      lcd.print("OFF");
      Serial.println("Relay is OFF. Solenoid Valve is OFF.");
    }
    delay(3000);
  } while (sm_s_ov <= sm_chk);
  lcd.noBacklight();
  Serial.println("Exit Do While Loop.");
  delay(5000);
}
