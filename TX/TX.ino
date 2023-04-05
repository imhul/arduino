#include "SPI.h"
#include "nRF24L01.h"
#include "RF24.h"

#define SW_pin_left 2  // digital pin connected to switch output
#define X_pin_left 2   // analog pin connected to X output
#define Y_pin_left 3   // analog pin connected to Y output

#define SW_pin_right 4  // digital pin connected to switch output
#define X_pin_right 0   // analog pin connected to X output
#define Y_pin_right 1   // analog pin connected to Y output

const uint64_t pipe = 0xF0F1F2F3F4LL;  // transmission identifier
RF24 radio(9, 10);

byte potent = 5;

int valX_left;
int valY_left;

int valX_right;
int valY_right;

byte transmit_data[7];
byte latest_data[7];
boolean flag;

void setup() {
  Serial.begin(9600);

  pinMode(SW_pin_left, INPUT);
  pinMode(X_pin_left, INPUT);
  pinMode(Y_pin_left, INPUT);
  pinMode(SW_pin_right, INPUT);
  pinMode(X_pin_right, INPUT);
  pinMode(Y_pin_right, INPUT);
  digitalWrite(SW_pin_left, HIGH); 
  digitalWrite(SW_pin_right, HIGH); 

  radio.begin();
  radio.setChannel(0);

  // speed: RF24_250KBPS, RF24_1MBPS or RF24_2MBPS
  radio.setDataRate(RF24_250KBPS);
  // power: RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM, RF24_PA_HIGH, RF24_PA_MAX
  radio.setPALevel(RF24_PA_HIGH);

  radio.openWritingPipe(pipe);
  radio.stopListening();
  Serial.println("Radio init!");
  Serial.println("-----------------------");
}

void printData(byte data[7]) {
  Serial.print("button left: ");
  Serial.println(data[0]);
  Serial.print("button right: ");
  Serial.println(data[1]);
  Serial.print("potent: ");
  Serial.println(data[2]);
  Serial.print("X left: ");
  Serial.println(data[3]);
  Serial.print("Y left: ");
  Serial.println(data[4]);
  Serial.print("X right: ");
  Serial.println(data[5]);
  Serial.print("Y right: ");
  Serial.println(data[6]);
  Serial.println("-----------------------");
  delay(1000);
}

void loop() {
  valX_left = analogRead(X_pin_left);
  valY_left = analogRead(Y_pin_left);
  valX_right = analogRead(X_pin_right);
  valY_right = analogRead(Y_pin_right);

  transmit_data[0] = digitalRead(SW_pin_left);
  transmit_data[1] = digitalRead(SW_pin_right);
  transmit_data[2] = map(analogRead(potent), 0, 1023, 0, 255);
  transmit_data[3] = map(valX_left, 0, 1023, 0, 90);
  transmit_data[4] = map(valY_left, 0, 1023, 0, 90);
  transmit_data[5] = map(valX_right, 0, 1023, 0, 90);
  transmit_data[6] = map(valY_right, 0, 1023, 0, 90);

  for (int i = 0; i < 7; i++) {
    if (transmit_data[i] != latest_data[i]) {
      flag = 1;
      latest_data[i] = transmit_data[i];
    }
  }

  if (flag == 1) {
    radio.powerUp();
    radio.write(&transmit_data, sizeof(transmit_data));
    flag = 0;
    radio.powerDown();
  }

  printData(transmit_data);
}
