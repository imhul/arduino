#define joystickY 4
#define joystickX 5

void setup() {
  Serial.begin(9600);
  pinMode(joystickY, INPUT);
  pinMode(joystickX, INPUT);
}

void Logger(int x, int y, int x2, int y2) {
  if(x!=analogRead(joystickX) or y!=analogRead(joystickY)) {
    String serialOutput = String("X: ") + x + " (" + x2 + ") " + "Y: " + y + " (" + y2 + ") ";
    Serial.println(serialOutput);
  }
}

void loop() {
  int input_x = analogRead(joystickX);
  int input_y = analogRead(joystickY);
  Logger(input_x, input_y, input_x, input_y);
} 
