#include <ESP8266WiFi.h>

// Enter your wifi network name and Wifi Password
const char* ssid = "Nokia5";
const char* password = "1234554321";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// These variables store current output state of LED
String outputRedState = "OFF";
String outputGreenState = "OFF";
String outputYellowState = "OFF";
String outputStatusLedState = "OFF";

String consoleRedAddr = "/D0/off";
String consoleGreenAddr = "/D1/off";
String consoleYellowAddr = "/D2/off";
String consoleStatusLedAddr = "/LED_BUILTIN/off";
// Assign output variables to GPIO pins
const int RLED = 5;
const int GLED = 4;
const int YLED = 0;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(RLED, OUTPUT);
  pinMode(GLED, OUTPUT);
  pinMode(YLED, OUTPUT);
  // Set outputs to LOW
  digitalWrite(RLED, LOW);
  digitalWrite(GLED, LOW);
  digitalWrite(YLED, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available(); // Listen for incoming clients

  if (client) { // If a new client connects,
    Serial.println("New Client."); // print a message out in the serial port
    String currentLine = ""; // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();
      if (client.available()) { // if there's bytes to read from the client,
        char c = client.read(); // read a byte, then
        Serial.write(c); // print it out the serial monitor
        header += c;
        if (c == '\n') { // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // turns the GPIOs on and off
            if (header.indexOf("GET /D0/on") >= 0) {
              Serial.println("RED LED is on");
              outputRedState = "ON";
              digitalWrite(RLED, HIGH);
            } else if (header.indexOf("GET /D0/off") >= 0) {
              Serial.println("RED LED is off");
              outputRedState = "OFF";
              digitalWrite(RLED, LOW);
            } else if (header.indexOf("GET /D1/on") >= 0) {
              Serial.println("Green LED is on");
              outputGreenState = "ON";
              
              digitalWrite(GLED, HIGH);
            } else if (header.indexOf("GET /D1/off") >= 0) {
              Serial.println("Green LED is off");
              outputGreenState = "OFF";
              
              digitalWrite(GLED, LOW);
            } else if (header.indexOf("GET /D2/on") >= 0) {
              Serial.println("Yellow LED is on");
              outputYellowState = "ON";
              digitalWrite(YLED, HIGH);
            } else if (header.indexOf("GET /D2/off") >= 0) {
              Serial.println("Yellow LED is off");
              outputYellowState = "OFF";
              digitalWrite(YLED, LOW);
            }

            if (outputRedState == "OFF") {
              consoleRedAddr = "/D0/on";
            } else {
              consoleRedAddr = "/D0/off";
            }

            if (outputGreenState == "OFF") {
              consoleGreenAddr = "/D1/on";
            } else {
              consoleGreenAddr = "/D1/off";
            }

            if (outputYellowState == "OFF") {
              consoleYellowAddr = "/D2/on";
            } else {
              consoleYellowAddr = "/D2/off";
            }

            String const html = "<!DOCTYPE html><html lang='en'><head> <meta charset='UTF-8'> <meta name='viewport' content='width=device-width, initial-scale=1.0'> <script src='https://code.jquery.com/jquery-3.6.3.slim.min.js' integrity='sha256-ZwqZIVdD3iXNyGHbSYdsmWP//UBokj2FHAxKuSBKDSo=' crossorigin='anonymous'></script> <style> html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center; } h1.red { color: #D11D53 } .buttonGreen { background-color: #4CAF50; border: 2px solid #000000; color: white; padding: 15px 32px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; margin: none; cursor: pointer; } .buttonRed { background-color: #D11D53; border: 2px solid #000000; color: white; padding: 15px 32px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; margin: none; cursor: pointer; } .buttonYellow { background-color: #FFD700; border: 2px solid #000000; color: white; padding: 15px 32px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; margin: none; cursor: pointer; } </style></head><body> <center> <span class='favicon'> <img src='https://i.postimg.cc/TYFB4nBQ/Elecrow-LOGO.png'> </span> </center> <h1 class='bt'>WiFi Control Platform</h1> <center> <a id='redButton' class='buttonRed' href=" 
              + consoleRedAddr 
              + "> <span id='consoleRedStatus'>RLED: " 
              + outputRedState + "</span> </a> <br><br> </center> <center> <a id='greenButton' class='buttonGreen' href=" 
              + consoleGreenAddr 
              + "> <span>GLED: " 
              + outputGreenState 
              + "</span> </a> <br><br> </center> <center> <a id='blueButton' class='buttonYellow' href=" 
              + consoleYellowAddr 
              + "> <span>YLED: " 
              + outputYellowState 
              + "</span> </a> </center> <script> $(document).ready(function () { $('.bt').click(function () { $(this).toggleClass('red'); }); }); </script></body></html>";

            // Display the HTML web page
            client.println(html);

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') { // if you got anything else but a carriage return character,
          currentLine += c; // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
