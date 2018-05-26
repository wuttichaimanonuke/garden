/*
Version : 1.0.0.1
*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>        //Declare Library screen I2C

LiquidCrystal_I2C lcd(0x27, 16, 2);   //Set the LCD address to 0x27 for a 16 chars and 2 line display

int sm_s_p = A0;      //Pin of Soil Moisture Sensor (analog)
int sm_s_ov ;         //Soil Moisture Sensor output value (analog value 0-1023)
int sm_s_ov_temp;
int re_p = 8;         //Pin of Relay Module (digital)
int sm_chk = 50;      //Condition check percentage moisture value

void setup(){
  Serial.begin(9600);
  pinMode(re_p, OUTPUT);    //Set Pin connected to Relay as an OUTPUT
  digitalWrite(re_p, LOW);  //Set Pin to LOW to turn Relay OFF
  
  lcd.begin();                    // initialize the LCD
  lcd.print("Initialize LCD");
  
  Serial.println("Reading From the Sensor ...");
  delay(5000);
}

void loop(){
  do {
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
      digitalWrite(re_p, HIGH);   //Turn Relay ON. Valve ON.
      lcd.setCursor(11, 1);
      lcd.print("ON");
      Serial.println("Relay is ON. Solenoid Valve is ON.");
    } else {
      digitalWrite(re_p, LOW);        //Turn Relay OFF. Valve OFF.
      lcd.setCursor(11, 1);
      lcd.print("OFF");
      Serial.println("Relay is OFF. Solenoid Valve is OFF.");
    }
    delay(5000);
  } while (sm_s_ov <= sm_chk);
  lcd.noBacklight();
  Serial.println("Exit Do While Loop.");
  delay(10000);
}
