#include "SPI.h"
#include "nRF24L01.h"
#include "RF24.h"
#include "Servo.h"
#include "GyverMotor2.h"  // https://github.com/GyverLibs/GyverMotor/blob/main/examples/GyverMotor2/demo/demo.ino
#include "TimeOut.h"

// timer
#define SECOND 1000UL
#define MINUTE (60 * SECOND)
TimeOut timeout;

// DC motors
GMotor2<DRIVER2WIRE> motor(6, 5);
int speed = 0;

// radio
const uint64_t pipe = 0xF0F1F2F3F4LL;  // transmission identifier
RF24 radio(7, 8);                      // For MEGA2560 radio(9,53);
byte recieved_data[7];

// servo
Servo tail;
Servo leftWingServo;
Servo rightWingServo;

int leftWingServoState;
int tailState;
int rightWingServoState;

int leftWingAngle = 0;
int rightWingAngle = 0;
int tailAngle = 0;

// state
bool printed = false;
bool noSignal = false;

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
  motor.setMinDuty(70);
  Serial.println("Motors init!");
  Serial.println("-----------------------");
  delay(2000);
}

void printResponse(byte data[7]) {
  if (!printed) {
    printed = true;
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
    delay(1000);
    printed = false;
  }
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

void rotateMotors(int value) {
  speed = value;
  motor.setSpeed(speed);
}

void returnAction() {
  noSignal = false;
  Serial.println("Return action!");
  delay(2000);
}

void noSignalActon() {
  if (noSignal) return;
  noSignal = true;
  Serial.println("No signal!");
  // TODO: timer
  timeout.timeOut(2000, returnAction);
}

void loop() {
  if (radio.available()) {
    radio.read(&recieved_data, sizeof(recieved_data));
    // timer
    timeout.cancel();

    if (speed != recieved_data[2]) rotateMotors(recieved_data[2]);

    // TODO: adjustment is required: if (tailState != recieved_data[4] && tailState + 5 < recieved_data[4] && tailState - 5 > recieved_data[4]) {
    if (tailState != recieved_data[4]) moveTail(recieved_data[4]);

    // TODO: adjustment is required: if (rightWingServoState != recieved_data[5] && rightWingServoState + 5 < recieved_data[5] && rightWingServoState - 5 > recieved_data[5]) {
    if (rightWingServoState != recieved_data[5]) moveRightWing(recieved_data[5]);

    // TODO: adjustment is required: if (leftWingServoState != recieved_data[3] && leftWingServoState + 5 < recieved_data[3] && leftWingServoState - 5 > recieved_data[3]) {
    if (leftWingServoState != recieved_data[3]) moveLeftWing(recieved_data[3]);

    printResponse(recieved_data);

  } else {
    noSignalActon();
    timeout.handler();
  }
}
