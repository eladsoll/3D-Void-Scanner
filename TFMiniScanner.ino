#include <Servo.h>
#include <arduino.h><br>#include "TeensyThreads.h"
Servo yaw, pitch;
char val;
volatile int liDARval;
double x, y, z;
float longitude, latitude;
#define MATH_PI 3.1415f
#define msec 10
#define totalTime 180000
unsigned int pitchPos, yawPos;
unsigned int flag;
unsigned long yawCurrentTime, yawPreviousTime, pitchCurrentTime, pitchPreviousTime;
unsigned long tfminiCurrentTime, tfminiPreviousTime;
unsigned long sysCurrentTime,sysPreviousTime;
void setup() {
  establishContect();
  //Initialize variables:  
  flag = 0;
  liDARval = 0;
  yawPos = 0;
  pitchPos = 0;
  //Initialize Timers,Leds,servo motors:
  updateTimers();
  initLeds();
  TurnOffLeds();
  initServos();
  //TFmini initializing:
  Serial.begin(115200);  
  // wait for serial port to connect. Needed for native USB port only
  while(!Serial);
  Serial1.begin(115200); // HW Serial for TFmini
  // Set to Standard Output mode
  initSerial1();  
}
void loop() {  
  updateTimers();
  //TFmini LIDAR Measurement:
  if(pitchPos == 70){
    if((sysCurrentTime-sysPreviousTime) >= totalTime){ 
      sysPreviousTime = sysCurrentTime;
      done();          
    }
    else{      
      pitchPos = 0;
    }
  }
  else{
      if(readLIDAR()==true){
        greenLed();//Serial communication is available.
      }
      else if(readLIDAR()==false){
        yellowLed();//If Serial communication isn't avavilable, turn-on yellow led.
      }            
  } 
  if(yawPos < 100){
    yawCCW();
  }
  if(yawPos == 100){
    pitchCCW();
    yawPos = 50;
  }  
}
//-------LIDAR function---------------
void initSerial1(){
  Serial1.write(0x42);
  Serial1.write(0x57);
  Serial1.write(0x02);
  Serial1.write(0x00);
  Serial1.write(0x00);
  Serial1.write(0x00);
  Serial1.write(0x01);
  Serial1.write(0x06);
}
boolean readLIDAR(){  
  // Data Format of  TFmini
  // =========================
  // 9 bytes total per message:
  // 1) 0x59
  // 2) 0x59
  // 3) Dist_L (low 8bit)
  // 4) Dist_H (high 8bit)
  // 5) Strength_L (low 8bit)
  // 6) Strength_H (high 8bit)
  // 7) Reserved bytes
  // 8) Original signal quality degree
  // 9) Checksum parity bit (low 8bit), Checksum = Byte1 + Byte2 +...+Byte8. This is only a low 8bit though  
    while(Serial1.available()>=9) // When at least 9 bytes of data available (expected number of bytes for 1 signal), then read
    {      
      if((0x59 == Serial1.read()) && (0x59 == Serial1.read())) // byte 1 and byte 2
      {
        unsigned int t1 = Serial1.read(); // byte 3 = Dist_L
        unsigned int t2 = Serial1.read(); // byte 4 = Dist_H
        t2 <<= 8;
        t2 += t1;
        liDARval = t2;
        t1 = Serial1.read(); // byte 5 = Strength_L
        t2 = Serial1.read(); // byte 6 = Strength_H
        t2 <<= 8;
        t2 += t1;
        for(int i=0; i<3; i++)Serial1.read(); // byte 7, 8, 9 are ignored
      }
    }
    longitude = (float)yawPos * PI / 180.0f;
//    latitude = (float)pitchPos * PI / 180.0f;
//    latitude = (float)(180-pitchPos) * PI / 180.0f;
    y = liDARval * sin(longitude) * cos(latitude);
    z = liDARval * sin(latitude) * sin(longitude);    
    x = liDARval * cos(longitude);
  if((tfminiCurrentTime-tfminiPreviousTime) >= 10){  // Don't want to read too often as TFmini samples at 100Hz
      Serial.println(String(x, 5) + " " + String(-y, 5) + " " + String(-z, 5));                
      tfminiPreviousTime = tfminiCurrentTime;
  }
  return true;
}
//-------Servo function--------
void initServos(){
  //Servos pin assigment:
  pitch.attach(9);
  yaw.attach(8);
  //Servos initializing:
  yaw.write(50);
  pitch.write(0);
}
void pitchCW() {
  //Pitch clock-wise rotation
  if ((pitchCurrentTime - pitchPreviousTime) >= msec) {
    pitch.write(360 - (pitchPos += 1));
    pitchPreviousTime = pitchCurrentTime;
  }
}
void pitchCCW() {
  //Pitch counter clock wise rotation
  if ((pitchCurrentTime - pitchPreviousTime) >= msec) {
    pitch.write(pitchPos += 1);
    pitchPreviousTime = pitchCurrentTime;
  }
}
void yawCW() {
  //Yaw clock wise rotation
  if ((yawCurrentTime - yawPreviousTime) >= (2*msec)) {
    yaw.write(360 - (yawPos += 1));
    yawPreviousTime = yawCurrentTime;
  }  
}
void yawCCW() {
  //Yaw counter clock wise rotation
  if ((yawCurrentTime - yawPreviousTime) >= (2*msec)) {
    yaw.write((yawPos += 1));
    yawPreviousTime = yawCurrentTime;
  }
}
//-------Led functions--------------
void initLeds(){
  //Define led control pins
  pinMode(24, OUTPUT); //red
  pinMode(26, OUTPUT); //yellow
  pinMode(28, OUTPUT); //green 
}
void TurnOffLeds() {
  //Turn off all leds.
  digitalWrite(28, LOW);
  digitalWrite(26, LOW);
  digitalWrite(24, LOW);
}
void greenLed() {
  //Turn-on green led.
  digitalWrite(28, HIGH);
  digitalWrite(26, LOW);
  digitalWrite(24, LOW);
}
void redLed() {
  //Turn-on red led.
  digitalWrite(24, HIGH);
  digitalWrite(26, LOW);
  digitalWrite(28, LOW);
}
void yellowLed() {
  //Turn-on yellow led.
  digitalWrite(26, HIGH);
  digitalWrite(28, LOW);
  digitalWrite(24, LOW);
}
boolean establishContect(){
  if(Serial.available() > 0){
    val = Serial.read();
    return true;
  }
}
void updateTimers(){
  //Update timmer on setup and at each loop iteration.
  if(flag==0){
    yawCurrentTime = 0;
  yawPreviousTime = 0;
  pitchCurrentTime = 0;
  pitchPreviousTime = 0;
  tfminiCurrentTime = 0;
  tfminiPreviousTime = 0;
  sysCurrentTime = 0;
  sysPreviousTime = 0;
  flag = 1;
  }
  if(flag==1){
    yawCurrentTime = millis();
    pitchCurrentTime = millis();
    tfminiCurrentTime = millis();
    sysCurrentTime = millis();
  }
}
void done(){
  //End off measurment,detach servo motors and turn-on red led.
  while(1){
    yaw.detach();
    pitch.detach();    
    redLed();
  }
}
