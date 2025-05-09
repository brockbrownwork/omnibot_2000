/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink
*/

//strtok = string tokenize


// STARTS AT PIN 9

const int RIGHT_MOTOR_BACKWARD_PIN = 9;
const int RIGHT_MOTOR_FORWARD_PIN = 10;
const int LEFT_MOTOR_FORWARD_PIN = 11;
const int LEFT_MOTOR_BACKWARD_PIN = 12;

const int LEFT_MOTOR_PWM_PIN = 13;
const int RIGHT_MOTOR_PWM_PIN = 8;

// the setup function runs once when you press reset or power the board
void setup() {
  int dutyCycle = 255;
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  Serial.println("Started!");
  digitalWrite(LEFT_MOTOR_PWM_PIN, dutyCycle);
  digitalWrite(RIGHT_MOTOR_PWM_PIN, dutyCycle);
  // delay(10000); 
}

void forward(int duration) {
  Serial.println("Moving forward...");
  digitalWrite(RIGHT_MOTOR_FORWARD_PIN, HIGH);
  digitalWrite(LEFT_MOTOR_FORWARD_PIN, HIGH);
  delay(duration);
  digitalWrite(RIGHT_MOTOR_FORWARD_PIN, LOW);
  digitalWrite(LEFT_MOTOR_FORWARD_PIN, LOW);
  delay(1000);
}

void backward(int duration) {
  Serial.println("Moving backward...");
  digitalWrite(RIGHT_MOTOR_BACKWARD_PIN, HIGH);
  digitalWrite(LEFT_MOTOR_BACKWARD_PIN, HIGH);
  delay(duration);
  digitalWrite(RIGHT_MOTOR_BACKWARD_PIN, LOW);
  digitalWrite(LEFT_MOTOR_BACKWARD_PIN, LOW);
  delay(1000);
}

void turnRight(int duration) {
  Serial.println("Turning right...");
  digitalWrite(RIGHT_MOTOR_BACKWARD_PIN, HIGH);
  digitalWrite(LEFT_MOTOR_FORWARD_PIN, HIGH);
  delay(duration);
  digitalWrite(RIGHT_MOTOR_BACKWARD_PIN, LOW);
  digitalWrite(LEFT_MOTOR_FORWARD_PIN, LOW);
  delay(1000);
}

void turnLeft(int duration) {
  Serial.println("Turning left...");
  digitalWrite(LEFT_MOTOR_BACKWARD_PIN, HIGH);
  digitalWrite(RIGHT_MOTOR_FORWARD_PIN, HIGH);
  delay(duration);
  digitalWrite(LEFT_MOTOR_BACKWARD_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_FORWARD_PIN, LOW);
  delay(1000);
}

const int timeToRun = 5000;
// the loop function runs over and over again forever
void loop() {
  // Check if data is available to read from the serial port
  if (Serial.available() > 0) {
    // Read the incoming byte:
    char incomingByte = Serial.read();

    // Print the received byte to the serial monitor
    Serial.print("Received: ");
    Serial.println(incomingByte);

    // Check the received byte and call the corresponding function
    switch (incomingByte) {
      case 'f':
        forward(timeToRun);
        break;
      case 'b':
        backward(timeToRun);
        break;
      case 'l':
        turnLeft(timeToRun);
        break;
      case 'r':
        turnRight(timeToRun);
        break;
      default:
        // Handle unknown commands
        Serial.println("Unknown command. Use f, b, l, or r.");
        break;
    }
  }
}
