#include <Servo.h>

#define speed 15;

Servo servo1;
int angle = 0;
int valX;
int valY;
const int SW_pin = 2; // digital pin connected to switch output
const int X_pin = 0; // analog pin connected to X output
const int Y_pin = 1; // analog pin connected to Y output

void setup() {
  pinMode(SW_pin, INPUT);
  servo1.attach(8);
  servo1.write(angle);
  digitalWrite(SW_pin, HIGH);
  Serial.begin(115200);
}

void loop() {
  // // scan from 0 to 90 degrees
  // for(angle = 0; angle < 90; angle++) {                                  
  //   servo.write(angle);               
  //   delay(10);                   
  // } 
  // // now scan back from 90 to 0 degrees
  // for(angle = 90; angle > 0; angle--) {                                
  //   servo.write(angle);           
  //   delay(10);       
  // } 

  valX = analogRead(X_pin);
  // valY = analogRead(Y_pin);

  valX = map(valX, 0, 1023, 0, 90);
  // valY = map(valY, 0, 1023, 0, 90);

  servo1.write(valX);
  delay(10); 
  
  // Serial.print("Switch:  ");
  // Serial.print(digitalRead(SW_pin));
  // Serial.print("\n");
  // Serial.print("X-axis: ");
  // Serial.print(analogRead(X_pin));
  // Serial.print("\n");
  // Serial.print("Y-axis: ");
  // Serial.println(analogRead(Y_pin));
  // Serial.print("\n\n");
  // delay(500);
}