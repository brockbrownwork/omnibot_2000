/*
 Wifi Robot Motor Control

 This sketch is used to control the motors of the robot via WiFi.

 The robot is controlled by a ESP32 board.

 The robot has 2 motors, 1 on the left and 1 on the right.

 The motors are controlled by a L298N motor driver.
 */

#include <WiFi.h>

const char *ssid = "Akron Police Surveillance Unit";
const char *password = "godfrommachine";

NetworkServer server(80);

// Define your motor control pins here

const int LEFT_MOTOR_PWM_PIN = 14;
const int LEFT_MOTOR_BACKWARD_PIN = 27;
const int LEFT_MOTOR_FORWARD_PIN = 26;
const int RIGHT_MOTOR_FORWARD_PIN = 25;
const int RIGHT_MOTOR_BACKWARD_PIN = 33;
const int RIGHT_MOTOR_PWM_PIN = 32;

// PWM settings
const int freq = 500; // PWM frequency in Hz (e.g., 5 kHz)
const int resolution = 8; // PWM resolution (8-bit gives 0-255 range, like analogWrite)

// Motor speed
int motorSpeed = 255;

// passive buzzer
const int BUZZER_PIN = 23;

// Function to change the motor speed
void changeMotorSpeed(int speed) {
  motorSpeed = speed;
  ledcWrite(LEFT_MOTOR_PWM_PIN, speed);
  ledcWrite(RIGHT_MOTOR_PWM_PIN, speed);
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
  Serial.println("\nMoving Forward");
  stopMotors(); // Stop any previous movement
  digitalWrite(RIGHT_MOTOR_FORWARD_PIN, HIGH);
  digitalWrite(LEFT_MOTOR_FORWARD_PIN, HIGH);
}

// Function to move the robot backward
void moveBackward() {
  Serial.println("\nMoving Backward");
  stopMotors(); // Stop any previous movement
  digitalWrite(RIGHT_MOTOR_BACKWARD_PIN, HIGH);
  digitalWrite(LEFT_MOTOR_BACKWARD_PIN, HIGH);
}

// Function to turn the robot right
// Right motor backward, Left motor forward
void turnRight() {
  Serial.println("\nTurning Right");
  stopMotors(); // Stop any previous movement
  digitalWrite(RIGHT_MOTOR_BACKWARD_PIN, HIGH);
  digitalWrite(LEFT_MOTOR_FORWARD_PIN, HIGH);
}

// Function to turn the robot left
// Left motor backward, Right motor forward
void turnLeft() {
  Serial.println("\nTurning Left");
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
  pinMode(RIGHT_MOTOR_PWM_PIN, OUTPUT);
  pinMode(LEFT_MOTOR_PWM_PIN, OUTPUT);
  stopMotors();

  // Configure LEDC for left motor
  ledcAttach(LEFT_MOTOR_PWM_PIN, freq, resolution);

  // Configure LEDC for right motor
  ledcAttach(RIGHT_MOTOR_PWM_PIN, freq, resolution);
  // Set the duty cycle to 1
  changeMotorSpeed(255);

  // set the pwm pins to high for now until we figure out what's up with that
  // digitalWrite(LEFT_MOTOR_PWM_PIN, HIGH);
  // digitalWrite(RIGHT_MOTOR_PWM_PIN, HIGH);

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
            // client.println("HTTP/1.1 200 OK");
            // client.println("Content-type:text/html");
            // client.println();

            // client.print("<center>");
            // client.print("  <p style=\"font-size: 15vw;\"><a href=\"/turnLeft\">LEFT</a></p>");
            // client.print("  <p style=\"font-size: 15vw;\"><a href=\"/turnRight\">RIGHT</a></p>");
            // client.print("  <p style=\"font-size: 15vw;\"><a href=\"/moveForward\">FORWARD</a></p>");
            // client.print("  <p style=\"font-size: 15vw;\"><a href=\"/moveBackward\">BACKWARD</a></p>");
            // client.print("  <p style=\"font-size: 15vw;\"><a href=\"/stopMotors\">STOP</a></p>");
            // client.print("</center>");

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

          // 1. Constrain speed to be between 0 and 100 (for user input)
          speed = constrain(speed, 0, 100);

          // 2. Scale the speed from 0-100 to 90-255 for changeMotorSpeed function
          int scaledSpeed = map(speed, 0, 100, 90, 255);

          changeMotorSpeed(scaledSpeed); // Use the scaled speed here
          Serial.print("Setting motor speed to: ");
          Serial.println(scaledSpeed); // Print the scaled speed for verification
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
