#include "SPI.h"
#include "nRF24L01.h"
#include "RF24.h"
#include "Servo.h"
// #include "GyverMotor2.h"  // https://github.com/GyverLibs/GyverMotor/blob/main/examples/GyverMotor2/demo/demo.ino
#include <timeout.h>

// timer
#define SECOND 1000UL
#define MINUTE (60 * SECOND)

// DC motors
// GMotor2<DRIVER2WIRE> motor(6, 5);
int motors = 9;
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

int servoSpeed = servoSpeed;  // in ms
int defaultAngle = 45;

// state
bool printed = false;
bool noSignal = false;
bool isReturnAction = false;
bool timerStarted = false;

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
  delay(SECOND);
  Serial.println("Radio init!");
  Serial.println("-----------------------");

  leftWingServo.attach(2);   // brown
  rightWingServo.attach(4);  // violet
  tail.attach(3);            // white

  // set servos to center
  leftWingServo.write(defaultAngle);
  rightWingServo.write(defaultAngle);
  tail.write(defaultAngle);
  delay(SECOND);
  Serial.println("Servos init!");
  Serial.println("-----------------------");
  delay(SECOND);

  // set motors
  // motor.setMinDuty(70);
  pinMode(motors, OUTPUT);
  Serial.println("Motors init!");
  Serial.println("-----------------------");
  delay(SECOND);
}

void printResponse(byte data[7]) {
  if (!printed) {
    printed = true;
    Serial.print("button left: ");
    Serial.println(data[0]);
    Serial.print("button right: ");
    Serial.println(data[1]);
    Serial.print("motor speed: ");
    Serial.println(data[2]);
    Serial.print("left joystick X: ");
    Serial.println(data[3]);
    Serial.print("left joystick Y: ");
    Serial.println(data[4]);
    Serial.print("right joystick X: ");
    Serial.println(data[5]);
    Serial.print("right joystick Y: ");
    Serial.println(data[6]);
    Serial.println("-----------------------");
    delay(SECOND);
    printed = false;
  }
}

void moveTail(int value) {
  tailState = value;

  if (tailState > defaultAngle) {
    // right
    int tailAngle = defaultAngle;
    for (tailAngle; tailAngle < tailState; tailAngle++) {
      tail.write(tailAngle);
      delay(servoSpeed);
    }
  } else if (tailState < defaultAngle) {
    // left
    int tailAngle = defaultAngle;
    for (tailAngle; tailAngle > tailState; tailAngle--) {
      tail.write(tailAngle);
      delay(servoSpeed);
    }
  }
}

void moveLeftWing(int value) {
  leftWingServoState = value;

  if (leftWingServoState > defaultAngle) {
    // up
    for (leftWingAngle; leftWingAngle < leftWingServoState; leftWingAngle++) {
      leftWingServo.write(leftWingAngle);
      Serial.println("moveLeftWing up!");
      delay(servoSpeed);
    }
  } else if (leftWingServoState < defaultAngle) {
    // down
    for (leftWingAngle; leftWingAngle > leftWingServoState; leftWingAngle--) {
      leftWingServo.write(leftWingAngle);
      Serial.println("moveLeftWing down!");
      delay(servoSpeed);
    }
  } else {
    // middle
    Serial.println("moveLeftWing middle!");
    if (leftWingAngle > defaultAngle) {
      for (leftWingAngle; leftWingAngle > defaultAngle; leftWingAngle--) {
        leftWingServo.write(leftWingAngle);
        delay(servoSpeed);
      }
    } else {
      for (leftWingAngle; leftWingAngle < defaultAngle; leftWingAngle++) {
        leftWingServo.write(leftWingAngle);
        delay(servoSpeed);
      }
    }
  }
}

void moveRightWing(int value) {
  rightWingServoState = value;
  int absValue = abs(rightWingServoState - 90);

  if (rightWingServoState > defaultAngle) {
   // up
    for (rightWingAngle = 45; rightWingAngle > absValue; rightWingAngle--) {
      rightWingServo.write(rightWingAngle);
      Serial.println("up!");
      delay(servoSpeed);
    }
  } else if (rightWingServoState < defaultAngle) {
    // down
    for (rightWingAngle = 45; rightWingAngle < absValue; rightWingAngle++) {
      rightWingServo.write(rightWingAngle);
      Serial.println("down!");
      delay(servoSpeed);
    }
  } else {
    // middle
    Serial.println("middle!");
    if (rightWingAngle > defaultAngle) {
      for (rightWingAngle; rightWingAngle > defaultAngle; rightWingAngle--) {
        rightWingServo.write(rightWingAngle);
        delay(servoSpeed);
      }
    } else {
      for (rightWingAngle; rightWingAngle < defaultAngle; rightWingAngle++) {
        rightWingServo.write(rightWingAngle);
        delay(servoSpeed);
      }
    }
  }
}

void rotateMotors(int value) {
  speed = value;
  // motor.setSpeed(speed);
  analogWrite(motors, speed);
}

void returnAction() {
  noSignal = false;
  isReturnAction = true;
  Serial.println("Return action start!");
  delay(SECOND * 3);
  Serial.println("Return action done!");
  radio.startListening();
  timerStarted = false;
  isReturnAction = false;
}

void noSignalActon() {
  if (!noSignal) {
    noSignal = true;
    Serial.println("No signal!");
  }

  if (noSignal && !timerStarted) {
    // start here
    timerStarted = true;
  }
}

void loop() {
  if (isReturnAction) {
    Serial.println("Return Action in progress!");
    delay(SECOND);
    return;
  } else if (!isReturnAction && radio.available()) {
    radio.read(&recieved_data, sizeof(recieved_data));
    // timeout.cancel();

    if (recieved_data[0] == 0 && recieved_data[1] == 0 && recieved_data[2] == 0 && recieved_data[3] == 0 && recieved_data[4] == 0 && recieved_data[5] == 0 && recieved_data[6] == 0) {
      Serial.println("inner else");
      // noSignalActon();
      return;
    }

    noSignal = false;

    if (speed != recieved_data[2]) rotateMotors(recieved_data[2]);

    if (tailState != recieved_data[4] /* && abs(recieved_data[4] - speed) > 5 */) moveTail(recieved_data[4]);

    if (rightWingServoState != recieved_data[5] /* && abs(recieved_data[5] - rightWingServoState) > 5 */) moveRightWing(recieved_data[5]);

    if (leftWingServoState != recieved_data[3] /* && abs(recieved_data[3] - leftWingServoState) > 5 */) moveLeftWing(recieved_data[3]);

    printResponse(recieved_data);
  }
  //  else {
  // delay(2000);
  // Serial.println("outer else");
  // resetActon();
  // }
}
