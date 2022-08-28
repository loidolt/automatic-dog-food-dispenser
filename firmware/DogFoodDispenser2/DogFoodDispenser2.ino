//Dog Food Dispenser V2

// include the library code:
#include <LiquidCrystal.h>
#include <Wire.h>      // needed for the RTC libraty
#include <DS3231.h>

DS3231 Clock;

// initialize the library with the numbers of the interface pins dor the LCD
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

#define PIN_MOTOR 5

int pos = 0;
volatile boolean TurnDetected;
volatile boolean up;
const int PinCLK=2;                   // Used for generating interrupts using CLK signal
const int PinDT=3;                    // Used for reading DT signal
const int PinSW=4;                    // Used for the push button switch of the Rotary Encoder
const int buttonPin = A3;             // the number of the pushbutton pin for manual feed 13
int buttonState = 0;                  // variable for reading the manual feed pushbutton status
int feed1hour = 8;                   // variables for feeding times and quantity
int feed1minute = 00;
int feed2hour = 18;
int feed2minute = 30;
int feedQty = 6;

byte Year;
byte Month;
byte Date;
byte DoW;
byte Hour;
byte Minute;
byte Second;

void isr ()  {                    // Interrupt service routine is executed when a HIGH to LOW transition is detected on CLK
 if (digitalRead(PinCLK))         // this keeps an eye out for the rotary encoder being turned regardless of where the program is
   up = digitalRead(PinDT);       // currently exectuting - in other words, during the main loop this ISR will always be active
 else
   up = !digitalRead(PinDT);
 TurnDetected = true;
}

void GetDateStuff(byte& Year, byte& Month, byte& Day, byte& DoW, 
    byte& Hour, byte& Minute, byte& Second) {
  // Call this if you notice something coming in on 
  // the serial port. The stuff coming in should be in 
  // the order YYMMDDwHHMMSS, with an 'x' at the end.
  boolean GotString = false;
  char InChar;
  byte Temp1, Temp2;
  char InString[20];

  byte j=0;
  while (!GotString) {
    if (Serial.available()) {
      InChar = Serial.read();
      InString[j] = InChar;
      j += 1;
      if (InChar == 'x') {
        GotString = true;
      }
    }
  }
  Serial.println(InString);
  // Read Year first
  Temp1 = (byte)InString[0] -48;
  Temp2 = (byte)InString[1] -48;
  Year = Temp1*10 + Temp2;
  // now month
  Temp1 = (byte)InString[2] -48;
  Temp2 = (byte)InString[3] -48;
  Month = Temp1*10 + Temp2;
  // now date
  Temp1 = (byte)InString[4] -48;
  Temp2 = (byte)InString[5] -48;
  Day = Temp1*10 + Temp2;
  // now Day of Week
  DoW = (byte)InString[6] - 48;   
  // now Hour
  Temp1 = (byte)InString[7] -48;
  Temp2 = (byte)InString[8] -48;
  Hour = Temp1*10 + Temp2;
  // now Minute
  Temp1 = (byte)InString[9] -48;
  Temp2 = (byte)InString[10] -48;
  Minute = Temp1*10 + Temp2;
  // now Second
  Temp1 = (byte)InString[11] -48;
  Temp2 = (byte)InString[12] -48;
  Second = Temp1*10 + Temp2;
}

void printDigits(int digits){   // utility function for digital clock display: prints leading 0
   if(digits < 10)
    lcd.print('0');
   lcd.print(digits);
 }
 
 void feed() {
   
   lcd.setCursor(17,0);
   lcd.print(" Feeding Now!");
    for (int positionCounter = 0; positionCounter < 16; positionCounter++) {
     // scroll one position left:
     lcd.scrollDisplayLeft(); 
     // wait a bit:
     delay(150);
         }

   int i = (feedQty * 1000);
 
   digitalWrite(PIN_MOTOR, HIGH);
   delay(i);
   digitalWrite(PIN_MOTOR, LOW);
             }

void setup ()  {
   // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // setup the Rotary encoder
 pinMode(PinCLK,INPUT);
 pinMode(PinDT,INPUT);  
 pinMode(PinSW,INPUT);
 pinMode(buttonPin, INPUT);

 Wire.begin();
 
 attachInterrupt (0,isr,FALLING);   // interrupt 0 is always connected to pin 2 on Arduino UNO
    lcd.setCursor(0,0);            
    lcd.print("Food Dispensor");  // A bit of fun smile emoticon 
        }
  
void loop ()  {  //Main program loop - most things in here!
    static long virtualPosition=0;    // without STATIC it does not count correctly!!!

    if (Serial.available()) {
    GetDateStuff(Year, Month, Date, DoW, Hour, Minute, Second);

    Clock.setClockMode(false);  // set to 24h
    //setClockMode(true); // set to 12h

    Clock.setYear(Year);
    Clock.setMonth(Month);
    Clock.setDate(Date);
    Clock.setDoW(DoW);
    Clock.setHour(Hour);
    Clock.setMinute(Minute);
    Clock.setSecond(Second);
    
    lcd.setCursor(0, 0);
    printDigits(Hour); //call to print digit function that adds leading zeros that may be missing
    lcd.print(":");
    printDigits(Minute);
    lcd.print(":");
    printDigits(Second);
    lcd.print("  ");
    lcd.print("Qty ");
    lcd.print(feedQty);
    lcd.print(" ");
    lcd.setCursor(0,1);
    lcd.print("1)");
    printDigits(feed1hour);
    lcd.print(":");
    printDigits(feed1minute);
    lcd.print(" 2)");
    printDigits(feed2hour);
    lcd.print(":");
    printDigits(feed2minute);
    
    
// MAIN BREAKOUT "IF" SECION BELOW THAT MONITORS THE PUSH BUTTON AND ENTERS PROGRAMMING IF IT'S PUSHED 

 if (!(digitalRead(PinSW))) {      // check if pushbutton is pressed
                                   // if YES then enter the programming subroutine
    lcd.blink();   // Turn on the blinking cursor:
    lcd.setCursor(5,0);
    lcd.print(" SET");
    virtualPosition = Hour;  //needed or the hour will be zero each time you change the clock.
       do  {
            lcd.setCursor(0,0);   // put cursor at Time Hour  
            delay(500);   // Delay needed or same button press will exit do-while as while is checking for another button push!
            if (TurnDetected)  {        // do this only if rotation was detected
            if (up)
              virtualPosition--;
            else
            virtualPosition++;
            TurnDetected = false;          // do NOT repeat IF loop until new rotation detected
                               }
            // Here I change the hour of time -
            Hour = virtualPosition;
            Clock.setHour(Hour);
            lcd.setCursor(0, 0);
            printDigits(Hour);  // then re-print the hour on the LCD
         } while ((digitalRead(PinSW)));  // do this "do" loop while the PinSW button is NOT pressed
         lcd.noBlink();
         delay(1000);
       
       //   SET THE MINS
        
        lcd.blink();   // Turn on the blinking cursor:
       virtualPosition = Minute;  //needed or the minute will be zero each time you change the clock.
       do  {
       lcd.setCursor(3,0);   // put cursor at Time Mins
       delay(500);   // Delay needed or same button press will exit do-while as while is checking for another button push!
           if (TurnDetected)  {       // do this only if rotation was detected
            if (up)
              virtualPosition--;
            else
            virtualPosition++;
            TurnDetected = false;          // do NOT repeat IF loop until new rotation detected
                               }
            // Here I change the min of time -
            Minute = virtualPosition;
            Clock.setMinute(Minute);
            lcd.setCursor(3, 0);
            printDigits(Minute);  // then re-print the min on the LCD
         } while ((digitalRead(PinSW)));
         lcd.noBlink();
         delay(1000);
    
       //   SET THE QTY - Feed quantity
        
       lcd.blink();   // Turn on the blinking cursor:
       virtualPosition = feedQty;  //needed or the qty will be zero.
       do  {
       lcd.setCursor(14,0);   // put cursor at QTY
       delay(500);   // Delay needed or same button press will exit do-while as while is checking for another button push!
           if (TurnDetected)  {       // do this only if rotation was detected
            if (up)
              virtualPosition--;
            else
            virtualPosition++;
            TurnDetected = false;          // do NOT repeat IF loop until new rotation detected
                               }
            // Here I change the feed qty
            feedQty = virtualPosition;
            lcd.setCursor(14, 0);
            lcd.print(feedQty);
           } while ((digitalRead(PinSW)));
         lcd.noBlink();
         delay(1000);
         
     //   SET THE Feed1 Hour
        
       lcd.blink();   // Turn on the blinking cursor:
       virtualPosition = feed1hour;  //needed or will be zero to start with.
       do  {
       lcd.setCursor(2,1);   // put cursor at feed1hour
       delay(500);   // Delay needed or same button press will exit do-while as while is checking for another button push!
           if (TurnDetected)  {       // do this only if rotation was detected
            if (up)
              virtualPosition--;
            else
            virtualPosition++;
            TurnDetected = false;          // do NOT repeat IF loop until new rotation detected
                               }
            // Here I change the feed1 hour
            feed1hour = virtualPosition;
            lcd.setCursor(2,1);
            printDigits(feed1hour);
           } while ((digitalRead(PinSW)));
         lcd.noBlink();
         delay(1000);  
  
      //   SET THE Feed1 Mins
        
       lcd.blink();   // Turn on the blinking cursor:
       virtualPosition = feed1minute;  //needed or will be zero to start with.
       do  {
       lcd.setCursor(5,1);   // put cursor at feed1minute
       delay(500);   // Delay needed or same button press will exit do-while as while is checking for another button push!
           if (TurnDetected)  {       // do this only if rotation was detected
            if (up)
              virtualPosition--;
            else
            virtualPosition++;
            TurnDetected = false;          // do NOT repeat IF loop until new rotation detected
                               }
            // Here I change the feed1 minute
            feed1minute = virtualPosition;
            lcd.setCursor(5,1);
            printDigits(feed1minute);
           } while ((digitalRead(PinSW)));
         lcd.noBlink();
         delay(1000);   

   //   SET THE Feed2 Hour
        
       lcd.blink();   // Turn on the blinking cursor:
       virtualPosition = feed2hour;  //needed or will be zero to start with.
       do  {
       lcd.setCursor(10,1);   // put cursor at feed1hour
       delay(500);   // Delay needed or same button press will exit do-while as while is checking for another button push!
           if (TurnDetected)  {       // do this only if rotation was detected
            if (up)
              virtualPosition--;
            else
            virtualPosition++;
            TurnDetected = false;          // do NOT repeat IF loop until new rotation detected
                               }
            // Here I change the feed1 hour
            feed2hour = virtualPosition;
            lcd.setCursor(10,1);
            printDigits(feed2hour);
           } while ((digitalRead(PinSW)));
         lcd.noBlink();
         delay(1000);  
  
      //   SET THE Feed2 Mins
        
       lcd.blink();   // Turn on the blinking cursor:
       virtualPosition = feed2minute;  //needed or will be zero to start with.
       do  {
       lcd.setCursor(13,1);   // put cursor at feed1minute
       delay(500);   // Delay needed or same button press will exit do-while as while is checking for another button push!
           if (TurnDetected)  {       // do this only if rotation was detected
            if (up)
              virtualPosition--;
            else
            virtualPosition++;
            TurnDetected = false;          // do NOT repeat IF loop until new rotation detected
                               }
            // Here I change the feed1 minute
            feed2minute = virtualPosition;
            lcd.setCursor(13,1);
            printDigits(feed2minute);
           } while ((digitalRead(PinSW)));
         lcd.noBlink();
         delay(1000);                         
      
  }  // end of main IF rotary encoder push button checker
 
   // CHECK FOR MANUAL FEED BUTTON
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    feed();
  }
  // CHECK FEEDING TIME AND FEED IF MATCHED
  
  if (Hour == feed1hour && Minute == feed1minute && Second == 0)  {  // if I dont' check seconds are zero
    feed();                                                                   // then it'll feed continuously for 1 minute!
      }
  if (Hour == feed2hour && Minute == feed2minute && Second == 0)  {
    feed();
      }  
  
}   // End of main Loop
}


