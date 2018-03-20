#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

SoftwareSerial BTSerial(A0, A1); // RX | TX
LiquidCrystal_I2C lcd(0x27,16,2);

boolean down = false;
boolean autoMode = false;

char commands[8];

void setup()
{
  Serial.begin(9600);
  BTSerial.begin(38400);

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, INPUT);

  //Sensor Voor
  pinMode(13, OUTPUT); // Sets the trigPin as an Output
  pinMode(12, INPUT); // Sets the echoPin as an Input

  lcd.init();
  lcd.backlight();
  
}

int getDistance(){
  // Clears the trigPin
  digitalWrite(13, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(13, HIGH);
  delayMicroseconds(10);
  digitalWrite(13, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(12, HIGH);
  // Calculating the distance
  return duration*0.034/2;
}

void loop()
{  
    if (BTSerial.available() > 6){
       int x = 0;
       while(x < 7){
         commands[x] = BTSerial.read();
         BTSerial.println(commands[x]);
         x++;
       } 
       BTSerial.flush();    
    }

    if(commands[4] == '1'){
      autoMode = true;
    }

     if(commands[4] == '0'){
      autoMode = false;
    }

    if(autoMode){
      autoDriveMode();
    }

     if(!autoMode){
       manualDriveMode();
     }


}

void updateLCD(){

     if(digitalRead(10) == LOW || digitalRead(11) == LOW){
       if(down){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Thank you");
        lcd.setCursor(0,1);
        lcd.print("Daddy ;)");
        down = false;
       }
    }else{
      if(!down){
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Put me down");
          lcd.setCursor(0,1);
          lcd.print("daddy");
          down = true;
      }
    }
    
}

void autoDriveMode(){
  
  updateLCD();
  
  int distance = getDistance();
  
  if(digitalRead(10)== LOW && digitalRead(11) == LOW && distance > 10)
  {
    analogWrite(3, 80);
    analogWrite(9, 80);

     digitalWrite(2, LOW);
     digitalWrite(4, LOW);
  }
  else
  {
    // Controlleer als de linker sensor iets ziet en laat het linker wiel rijden.
    if(digitalRead(11) == LOW && digitalRead(10) == HIGH)
    {
      analogWrite(3, 100);
     
      digitalWrite(4, HIGH);
    }
    else
    {
      digitalWrite(3, LOW);
      
      digitalWrite(4, LOW);
    }
  
    // Controlleer als de rechter sensor iets ziet en laat het rechter wiel rijden.
    if(digitalRead(10) == LOW && digitalRead(11) == HIGH)
    {
      analogWrite(9, 100);
  
      digitalWrite(2, HIGH);
    }
    else
    {
      digitalWrite(9, LOW);
      
      digitalWrite(2, LOW);
    }
  }
  
}

void manualDriveMode(){

    if(commands[0] == '1'){
      analogWrite(3, 100);
      analogWrite(9, 100);

      analogWrite(2, 0);
      analogWrite(4, 0);
    }

    if(commands[1] == '1'){
      analogWrite(3, 0);
      analogWrite(9, 0);
      
      analogWrite(2, 255);
      analogWrite(4, 255);
    }
  
    //Links
    if(commands[2] == '1'){
      analogWrite(9, 100);
      analogWrite(3, 0);
    }

    //Rechts
    if(commands[3] == '1'){
      analogWrite(3, 100);
      analogWrite(9, 0);
    }

    //STOP
    if(commands[0] == '0' && commands[1] == '0' && commands[2] == '0' && commands[3] == '0' && commands[4] == '0' && commands[5] == '0' && commands[6] == '0'){
      analogWrite(3, 0);
      analogWrite(9, 0);

       analogWrite(2, 0);
      analogWrite(4, 0);
    }
  
}

