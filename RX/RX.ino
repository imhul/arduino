#include "SPI.h"
#include "nRF24L01.h"
#include "RF24.h"
#include "Servo.h"

const uint64_t pipe = 0xF0F1F2F3F4LL;  // transmission identifier
RF24 radio(7, 8);                      // For MEGA2560 radio(9,53);
byte recieved_data[7];

Servo tail;
Servo leftWingServo;
Servo rightWingServo;

int leftWingServoState;
int tailState;
int rightWingServoState;

int leftWingAngle = 0;
int rightWingAngle = 0;
int tailAngle = 0;

int motors = 9;  // D9
int speed = 0;

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.setChannel(0);

  // speed: RF24_250KBPS, RF24_1MBPS or RF24_2MBPS
  radio.setDataRate(RF24_250KBPS);
  // power: RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM, RF24_PA_HIGH, RF24_PA_MAX
  radio.setPALevel(RF24_PA_HIGH);
  radio.openReadingPipe(1, pipe);
  radio.powerUp();
  radio.startListening();
  delay(1000);
  Serial.println("Radio init!");
  Serial.println("-----------------------");

  leftWingServo.attach(2);   // brown
  rightWingServo.attach(4);  // violet
  tail.attach(3);            // white

  // set servos to center
  leftWingServo.write(45);
  rightWingServo.write(45);
  tail.write(45);
  delay(1000);
  Serial.println("Servos init!");
  Serial.println("-----------------------");
  delay(1000);

  // set motors
  pinMode(motors, OUTPUT);
  // analogWrite(motors, speed);
  delay(1000);
  Serial.println("Motors init!");
  Serial.println("-----------------------");
  delay(1000);
}

void printResponse(byte data[7]) {
  Serial.print("button left: ");
  Serial.println(recieved_data[0]);
  Serial.print("button right: ");
  Serial.println(recieved_data[1]);
  Serial.print("potent (motor speed): ");
  Serial.println(recieved_data[2]);
  Serial.print("X left: ");
  Serial.println(recieved_data[3]);
  Serial.print("Y left: ");
  Serial.println(recieved_data[4]);
  Serial.print("X right: ");
  Serial.println(recieved_data[5]);
  Serial.print("Y right: ");
  Serial.println(recieved_data[6]);
  Serial.println("-----------------------");
}

void moveTail(int value) {
  tailState = value;

  // TODO: does not return after moving to the left!

  if (tailState > 45) {
    // right
    for (tailAngle; tailAngle < tailState; tailAngle++) {
      tail.write(tailAngle);
      delay(25);
    }
  } else if (tailState < 45) {
    // left
    for (tailAngle; tailAngle > tailState; tailAngle--) {
      tail.write(tailAngle);
      delay(25);
    }
  } else {
    // middle
    if (tailAngle > 45) {
      for (tailAngle; tailAngle > 45; tailAngle--) {
        tail.write(tailAngle);
        delay(25);
      }
    } else {
      for (tailAngle; tailAngle < 45; tailAngle++) {
        tail.write(tailAngle);
        delay(25);
      }
    }
  }
}

void moveLeftWing(int value) {
  leftWingServoState = value;
  
  if (leftWingServoState > 45) {
    // up
    for (leftWingAngle; leftWingAngle < leftWingServoState; leftWingAngle++) {
      leftWingServo.write(leftWingAngle);
      delay(25);
    }
  } else if (leftWingServoState < 45) {
    // down
    for (leftWingAngle; leftWingAngle > leftWingServoState; leftWingAngle--) {
      leftWingServo.write(leftWingAngle);
      delay(25);
    }
  } else {
    // middle
    if (leftWingAngle > 45) {
      for (leftWingAngle; leftWingAngle > 45; leftWingAngle--) {
        leftWingServo.write(leftWingAngle);
        delay(25);
      }
    } else {
      for (leftWingAngle; leftWingAngle < 45; leftWingAngle++) {
        leftWingServo.write(leftWingAngle);
        delay(25);
      }
    }
  }
}

void moveRightWing(int value) {
  rightWingServoState = value;

  if (rightWingServoState > 45) {
    // up
    for (rightWingAngle; rightWingAngle < rightWingServoState; rightWingAngle++) {
      rightWingServo.write(rightWingAngle);
      delay(25);
    }
  } else if (rightWingServoState < 45) {
    // down
    for (rightWingAngle; rightWingAngle > rightWingServoState; rightWingAngle--) {
      rightWingServo.write(rightWingAngle);
      delay(25);
    }
  } else {
    // middle
    if (rightWingAngle > 45) {
      for (rightWingAngle; rightWingAngle > 45; rightWingAngle--) {
        rightWingServo.write(rightWingAngle);
        delay(25);
      }
    } else {
      for (rightWingAngle; rightWingAngle < 45; rightWingAngle++) {
        rightWingServo.write(rightWingAngle);
        delay(25);
      }
    }
  }
}

void loop() {
  if (radio.available()) {
    radio.read(&recieved_data, sizeof(recieved_data));
    printResponse(recieved_data);

    speed = recieved_data[2];

    if (tailState != recieved_data[4]) {
      moveTail(recieved_data[4]);
    }

    if (rightWingServoState != recieved_data[5]) {
      moveRightWing(recieved_data[5]);
    }

    if (leftWingServoState != recieved_data[3]) {
      moveLeftWing(recieved_data[3]);
    }

  } else {
    Serial.println("No signal!");
  }

  analogWrite(motors, speed);
}
