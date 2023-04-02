#include "SPI.h"
#include "nRF24L01.h"
#include "RF24.h"
#include "Servo.h"

const uint64_t pipe = 0xF0F1F2F3F4LL;  // id
RF24 radio(7, 8);                      // For MEGA2560 radio(9,53);
byte recieved_data[7];

Servo leftWingServo;
Servo servo2;
Servo servo3;

int leftWingServoState;
int servo2State;
int servo3State;
int angle = 0;
bool start = 1;

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.setChannel(0);

  // speed: RF24_250KBPS, RF24_1MBPS or RF24_2MBPS
  radio.setDataRate(RF24_2MBPS);
  // power: RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM, RF24_PA_HIGH, RF24_PA_MAX
  radio.setPALevel(RF24_PA_HIGH);

  radio.openReadingPipe(1, pipe);
  radio.powerUp();
  radio.startListening();
  Serial.println("Radio init!");
  Serial.println("-----------------------");

  leftWingServo.attach(2);  // brown
  leftWingServo.write(45);
  // servo2.attach(3); // white
  // servo3.attach(4); // violet

  delay(2000);
  Serial.println("Servos init!");
  Serial.println("-----------------------");
}

void printResponse(byte data[7]) {
  Serial.print("button left: ");
  Serial.println(recieved_data[0]);
  Serial.print("button right: ");
  Serial.println(recieved_data[1]);
  Serial.print("potent: ");
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

void loop() {
  if (radio.available()) {
    radio.read(&recieved_data, sizeof(recieved_data));
    printResponse(recieved_data);


    if (servo2State != recieved_data[4]) {
      servo2State = recieved_data[4];
    }

    if (servo3State != recieved_data[5]) {
      servo3State = recieved_data[5];
    }

    if (leftWingServoState != recieved_data[3]) {
      leftWingServoState = recieved_data[3];

      if (leftWingServoState > 45) {
        Serial.println("-------up---------");

        for (angle; angle < leftWingServoState; angle++) {
          leftWingServo.write(angle);

          // Serial.print("in UP FOR ---- ");
          // Serial.print("Value: ");
          // Serial.print(leftWingServoState);
          // Serial.print(", Angle: ");
          // Serial.print(angle);
          // Serial.print(", Calculated: ");
          // Serial.println(leftWingServoState - angle);

          delay(50);
        }
      } else if (leftWingServoState < 45) {
        Serial.println("-------down---------");

        for (angle; angle > leftWingServoState; angle--) {
          leftWingServo.write(angle);

          // Serial.print("in DOWN FOR ---- ");
          // Serial.print("Value: ");
          // Serial.print(leftWingServoState);
          // Serial.print(", Angle: ");
          // Serial.print(angle);
          // Serial.print(", Calculated: ");
          // Serial.println(angle - leftWingServoState);

          delay(50);
        }
      } else {
        Serial.println("Value = 45");
        angle = 45;
        leftWingServo.write(angle);
      }
    } else {
      Serial.println("Recieved value = Current value");
    }
  } else {
    Serial.println("No signal!");
    // Serial.print("Value: ");
    // Serial.print(leftWingServoState);
    // Serial.print(", Angle: ");
    // Serial.println(angle);
  }

  // servo2.write(servo2State);
  // servo3.write(servo3State);
}
