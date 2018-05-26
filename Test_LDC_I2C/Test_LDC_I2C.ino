#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 in PCF8574 by NXP and Set to 0x3F in PCF8574A by Ti
LiquidCrystal_I2C lcd(0x27, 16, 2); // จอกว้าง 16 ตัวอักษร 2 บรรทัด รหัสประจำตัว 0x3F

void setup() {
  lcd.begin();
  lcd.print("LCDisplay"); // แสดงผลคำว่า Hello, world! ออกหน้าจอ
  lcd.setCursor(0, 1); // เลื่อนเคเซอร์ไปบรรทัดที่ 2 ลำดับที่ 0 (ก่อนหน้าตัวอักษรแรก)
  lcd.print("www.ioxhop.com"); // แสดงผลคำว่า www.ioxhop.com
  delay(3000); // หน่วงเวลา 3 วินาที
  lcd.clear(); // ล้างหน้าจอ
}

void loop() {
  lcd.setCursor(3, 0);
  lcd.print("InFunction ");
  lcd.setCursor(2, 1);
  lcd.print("void loop(){ ");
  delay(500); // หน่วงเวลา 0.5 วินาที
  lcd.clear(); // ล้างหน้าจอ
  delay(500); // หน่วงเวลา 0.5 วินาที
}
