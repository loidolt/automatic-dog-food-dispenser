#include "Wire.h"
#include <LiquidCrystal.h>

const int motor = 5;
const int manualBtn = 3;
const int enableToggle = 4;

int buttonState = 0;
int enableState = 0;

int feedQty = 30;
int feedDelay = 0;

int feedTimeHour1 = 7;
int feedTimeMin1 = 0;

int feedTimeHour2 = 17;
int feedTimeMin2 = 0;

#define DS1307_I2C_ADDRESS 0x68
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
 
byte bcdToDec(byte val)
{
  return ( (val/16*10) + (val%16) );
}
 
void getDateDs1307(byte *second,byte *minute,byte *hour,byte *dayOfWeek,byte *dayOfMonth,byte *month,byte *year)
{
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();
 
  Wire.requestFrom(DS1307_I2C_ADDRESS, 7);
 
  *second     = bcdToDec(Wire.read() & 0x7f);
  *minute     = bcdToDec(Wire.read());
  *hour       = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek  = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month      = bcdToDec(Wire.read());
  *year       = bcdToDec(Wire.read());
}

byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
 
void setup()
{
  pinMode(motor, OUTPUT);

  pinMode(manualBtn, INPUT_PULLUP);
  pinMode(enableToggle, INPUT_PULLUP);
  
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  
  Wire.begin();
  // AMEND IF YOUR USING A DIFFERENT LCD SCREEN //
   lcd.begin(16, 2);

  feedDelay = feedQty * 1000;
}
 
void loop()
{
  buttonState = digitalRead(manualBtn);
  enableState = digitalRead(enableToggle);

  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  String s, m, d, mth, h;
   
  getDateDs1307(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
 
  if (second < 10) { s = "0" + String(second); } else { s = String(second); }
  if (minute < 10) { m = "0" + String(minute); } else { m = String(minute); }
  h = String(hour);
  if (dayOfMonth < 10) { d = "0" + String(dayOfMonth); } else { d = String(dayOfMonth); }
  if (month < 10) { mth = "0" + String(month); } else { mth = String(month); }
   
  char* days[] = { "NA", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };


if (enableState == LOW){
  if (hour == feedTimeHour1 && minute == feedTimeMin1 && second < 20) {
    lcd.clear();
    lcd.print("--Feeding Now!--");
    digitalWrite(motor, HIGH);
    delay(feedDelay);
  }
  else if (hour == feedTimeHour2 && minute == feedTimeMin2 && second < 20) {
    lcd.clear();
    lcd.print("--Feeding Now!--");
    digitalWrite(motor, HIGH);
    delay(feedDelay);
  }
  else if (buttonState == LOW) {
    lcd.clear();
    lcd.print("--Manual Feed--");
    digitalWrite(motor, HIGH);
    delay(feedDelay);
  }
  else {
    digitalWrite(motor, LOW);
  }
}
else if (buttonState == LOW) {
    lcd.clear();
    lcd.print("--Manual Feed--");
    digitalWrite(motor, HIGH);
    delay(feedDelay);
  }
else {
  digitalWrite(motor, LOW);
}
   
  lcd.clear();
  // JUMP TO CENTER ON A 16X2 SCREEN //
  lcd.setCursor(4,0);
  // CHANGE THE FOLLOWING TO SET THE DATE IN TO YOUR PREFERED ORDER //
  lcd.print(h + ":" + m + ":" + s);
  // NEXT LINE, 1 SPACE IN FROM THE LEFT //
  lcd.setCursor(1,1);
  // PREFIX THE 20 AS THE RTC CHIP ONLY USES 2 DIGITS FOR THE YEAR //
  lcd.print(String(days[dayOfWeek]) + " " + d + "/" + mth + "/20" + year);
  delay(1000); // Wait 1 second
}
