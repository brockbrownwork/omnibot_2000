/*
 WiFi Web Server LED Blink

 A simple web server that lets you blink an LED via the web.
 This sketch will print the IP address of your WiFi Shield (once connected)
 to the Serial monitor. From there, you can open that address in a web browser
 to turn on and off the LED on pin 5.

 If the IP address of your shield is yourAddress:
 http://yourAddress/H turns the LED on
 http://yourAddress/L turns it off

 This example is written for a network using WPA2 encryption. For insecure
 WEP or WPA, change the Wifi.begin() call and use Wifi.setMinSecurity() accordingly.

 Circuit:
 * WiFi shield attached
 * LED attached to pin 5

 created for arduino 25 Nov 2012
 by Tom Igoe

ported for sparkfun esp32
31.01.2017 by Jan Hendrik Berlin

 */

#include <WiFi.h>

const char *ssid = "Akron Police Surveillance Unit";
const char *password = "godfrommachine";

NetworkServer server(80);

// Define your motor control pins here
const int LEFT_MOTOR_FORWARD_PIN = 13;
const int LEFT_MOTOR_BACKWARD_PIN = 12;
const int RIGHT_MOTOR_FORWARD_PIN = 27;
const int RIGHT_MOTOR_BACKWARD_PIN = 26;
const int LEFT_MOTOR_PWM_PIN = 28;
const int RIGHT_MOTOR_PWM_PIN = 29;

// PWM settings
const int freq = 5000; // PWM frequency in Hz (e.g., 5 kHz)
const int resolution = 8; // PWM resolution (8-bit gives 0-255 range, like analogWrite)

// LEDC channels
const int leftMotorChannel = 0;
const int rightMotorChannel = 1;

// Motor speed
int motorSpeed = 255;

// passive buzzer
const int BUZZER_PIN = 22;

// Function to change the motor speed
void changeMotorSpeed(int speed) {
  motorSpeed = speed;
  ledcWrite(leftMotorChannel, speed);
  ledcWrite(rightMotorChannel, speed);
}

// Function to stop all motors
void stopMotors() {
  digitalWrite(RIGHT_MOTOR_FORWARD_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_BACKWARD_PIN, LOW);
  digitalWrite(LEFT_MOTOR_FORWARD_PIN, LOW);
  digitalWrite(LEFT_MOTOR_BACKWARD_PIN, LOW);
}

// Function to move the robot forward
void moveForward() {
  Serial.println("Moving Forward");
  stopMotors(); // Stop any previous movement
  digitalWrite(RIGHT_MOTOR_FORWARD_PIN, HIGH);
  digitalWrite(LEFT_MOTOR_FORWARD_PIN, HIGH);
}

// Function to move the robot backward
void moveBackward() {
  Serial.println("Moving Backward");
  stopMotors(); // Stop any previous movement
  digitalWrite(RIGHT_MOTOR_BACKWARD_PIN, HIGH);
  digitalWrite(LEFT_MOTOR_BACKWARD_PIN, HIGH);
}

// Function to turn the robot right
// Right motor backward, Left motor forward
void turnRight() {
  Serial.println("Turning Right");
  stopMotors(); // Stop any previous movement
  digitalWrite(RIGHT_MOTOR_BACKWARD_PIN, HIGH);
  digitalWrite(LEFT_MOTOR_FORWARD_PIN, HIGH);
}

// Function to turn the robot left
// Left motor backward, Right motor forward
void turnLeft() {
  Serial.println("Turning Left");
  stopMotors(); // Stop any previous movement
  digitalWrite(LEFT_MOTOR_BACKWARD_PIN, HIGH);
  digitalWrite(RIGHT_MOTOR_FORWARD_PIN, HIGH);
}

void setup() {
  Serial.begin(115200);

  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(WiFi.status());
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
  // Buzzer
  pinMode(BUZZER_PIN, OUTPUT);

  // Start your engines
  pinMode(RIGHT_MOTOR_FORWARD_PIN, OUTPUT);
  pinMode(RIGHT_MOTOR_BACKWARD_PIN, OUTPUT);
  pinMode(LEFT_MOTOR_FORWARD_PIN, OUTPUT);
  pinMode(LEFT_MOTOR_BACKWARD_PIN, OUTPUT);
  stopMotors();

  // Configure LEDC for left motor
  ledcAttachPin(LEFT_MOTOR_PWM_PIN, leftMotorChannel);
  ledcSetup(leftMotorChannel, freq, resolution);

  // Configure LEDC for right motor
  ledcAttachPin(RIGHT_MOTOR_PWM_PIN, rightMotorChannel);
  ledcSetup(rightMotorChannel, freq, resolution);

  // Set the duty cycle to 1
  changeMotorSpeed(255);
}

void loop() {
  NetworkClient client = server.accept();  // listen for incoming clients

  if (client) {                     // if you get a client,
    Serial.println("New Client.");  // print a message out the serial port
    String currentLine = "";        // make a String to hold incoming data from the client
    while (client.connected()) {    // loop while the client's connected
      if (client.available()) {     // if there's bytes to read from the client,
        char c = client.read();     // read a byte, then
        Serial.write(c);            // print it out the serial monitor
        if (c == '\n') {            // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            client.print("<h3>Click <a href=\"/moveForward\">here</a> to move the robot forward.</h3>");
            client.print("<h3>Click <a href=\"/moveBackward\">here</a> to move the robot backward.</h3>");
            client.print("<h3>Click <a href=\"/turnLeft\">here</a> to turn the robot left.</h3>");
            client.print("<h3>Click <a href=\"/turnRight\">here</a> to turn the robot right.</h3>");
            client.print("<h3>Click <a href=\"/stopMotors\">here</a> to stop all motors.</h3>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {  // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check for motor control commands
        if (currentLine.endsWith("GET /moveForward")) {
          moveForward(); 
        }
        if (currentLine.endsWith("GET /moveBackward")) {
          moveBackward();
        }
        if (currentLine.endsWith("GET /turnLeft")) {
          turnLeft();
        }
        if (currentLine.endsWith("GET /turnRight")) {
          turnRight();
        }
        if (currentLine.endsWith("GET /stopMotors")) {
          stopMotors();
        }
        if (currentLine.indexOf("GET /speed/") != -1) {
          // This handles direct links like /speed/128
          int speedIndex = currentLine.indexOf("/speed/");
          String speedString = currentLine.substring(speedIndex + 7); // "/speed/" has 7 characters
          speedString = speedString.substring(0, speedString.indexOf(' ')); // Get the value before the next space
          int speed = speedString.toInt();
          // Constrain speed to be between 0 and 255
          speed = constrain(speed, 0, 255);
          changeMotorSpeed(speed);
          Serial.print("Setting motor speed to: ");
          Serial.println(speed);
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
