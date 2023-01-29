#include <Keypad.h>

#define speakerPin 11
#define ledPin 13

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

// connect the pins from right to left to pin 2, 3, 4, 5,6,7,8,9
byte rowPins[ROWS] = { 5, 4, 3, 2 };  //connect to the row pinouts of the keypad
byte colPins[COLS] = { 9, 8, 7, 6 };  //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  pinMode(speakerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  randomSeed(analogRead(0));
  Serial.begin(9600);
}

void phrase1() {
  int k = random(1000, 2000);
  digitalWrite(ledPin, HIGH);
  for (int i = 0; i <= random(100, 2000); i++) {

    tone(speakerPin, k + (-i * 2));
    delay(random(.9, 2));
  }
  digitalWrite(ledPin, LOW);
  for (int i = 0; i <= random(100, 1000); i++) {

    tone(speakerPin, k + (i * 10));
    delay(random(.9, 2));
  }
}

void phrase2() {

  int k = random(1000, 2000);
  digitalWrite(ledPin, HIGH);
  for (int i = 0; i <= random(100, 2000); i++) {

    tone(speakerPin, k + (i * 2));
    delay(random(.9, 2));
  }
  digitalWrite(ledPin, LOW);
  for (int i = 0; i <= random(100, 1000); i++) {

    tone(speakerPin, k + (-i * 10));
    delay(random(.9, 2));
  }
}

void phrase3(int count) {
  int K = 2000;

  for (int i = 0; i <= count; i++) {
    digitalWrite(ledPin, HIGH);
    tone(speakerPin, K + random(-1700, 2000));
    delay(random(70, 170));
    digitalWrite(ledPin, LOW);
    noTone(speakerPin);
    delay(random(0, 30));
  }
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    Serial.print("Key ");
    Serial.println(key);
  } else {
    return;
  }

  switch (key) {
    case '*':
    case '#':
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case '1':
    case '0':
      phrase1();
      break;
    case '2':
      phrase2();
      break;
    case '3':
      phrase3(3);
      break;
    case '4':
      phrase3(4);
      break;
    case '5':
      phrase3(5);
      break;
    case '6':
      phrase3(6);
      break;
    case '7':
      phrase3(7);
      break;
    case '8':
      phrase3(8);
    case '9':
      phrase3(9);
      break;
  }

  noTone(speakerPin);
}