#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

SoftwareSerial BTSerial(A0, A1); // RX | TX
LiquidCrystal_I2C lcd(0x27,16,2);

const int MPU_addr=0x68; 

boolean down = false;
boolean autoMode = false;

char commands[10];

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

  setupMPU();
  
}

void setupMPU(){
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}

long readMPU(){
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x43);  // starting with gyro reg
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,6,true);  // request a total of 14 registers
  while(Wire.available() < 6);
  long gyroX = Wire.read()<<8|Wire.read(); //Store first two bytes into accelX
  long gyroY = Wire.read()<<8|Wire.read(); //Store middle two bytes into accelY
  long gyroZ = Wire.read()<<8|Wire.read(); //Store last two bytes into accelZ

  gyroX = gyroX / 131.0;
  gyroY = (gyroY / 131.0) - 0.55;
  gyroZ = gyroZ / 131.0;

  return gyroZ;
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
    if ( BTSerial.available() > 6){
       int x = 0;
       while(x <= 6){
         commands[x] = BTSerial.read();
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
      autoDriveModeLine();
     
    }

     if(!autoMode){
       manualDriveMode();
     }
// autoDriveMode();

}

void updateLCD(){

     if(digitalRead(10) == LOW || digitalRead(11) == LOW){
       if(down){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("U kan");
        lcd.setCursor(0,1);
        lcd.print("beginnen;)");
        down = false;
       }
    }else{
      if(!down){
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Zet mij neer");
          lcd.setCursor(0,1);
          lcd.print("om te starten");
          down = true;
      }
    }
    
}

void autoDriveMode(){
   long z = readMPU();

   if(z > 20){
    analogWrite(9, 0); 
    analogWrite(3, 100);

   }

   if(z < -20){
    analogWrite(3, 0);
    analogWrite(9, 100);

   }

   if(z <= 20 && z >= -20){
     analogWrite(3, 80);
     analogWrite(9, 82);
   }
   delay(150);
  
}

void autoDriveModeLine(){
  
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
      analogWrite(3, 255);
      analogWrite(9, 255);

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
      analogWrite(9, 160);
      analogWrite(3, 0);
    }

    //Rechts
    if(commands[3] == '1'){
      analogWrite(3, 160);
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

