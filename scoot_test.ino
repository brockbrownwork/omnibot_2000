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


// STARTS AT PIN 9

const int LEFT_MOTOR_FORWARD_PIN = 9;
const int LEFT_MOTOR_BACKWARD_PIN = 10;
const int RIGHT_MOTOR_FORWARD_PIN = 11;
const int RIGHT_MOTOR_BACKWARD_PIN = 12;
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
  delay(10000); 
}

void forward(int duration) {
  Serial.println("Moving forward...");
  digitalWrite(RIGHT_MOTOR_FORWARD_PIN, HIGH);
  digitalWrite(LEFT_MOTOR_FORWARD_PIN, HIGH);
  delay(duration);
  digitalWrite(RIGHT_MOTOR_FORWARD_PIN, LOW);
  digitalWrite(LEFT_MOTOR_FORWARD_PIN, LOW);
}

// the loop function runs over and over again forever
void loop() {
    for (int motorPin = LEFT_MOTOR_FORWARD_PIN; motorPin <= RIGHT_MOTOR_BACKWARD_PIN; motorPin++) {
      forward(5000);
      Serial.print("Messing with pin: ");
      Serial.println(motorPin);
      digitalWrite(motorPin, HIGH);   // turn the LED on (HIGH is the voltage level)
      digitalWrite(LED_BUILTIN, HIGH);
      delay(5000);                       // wait for a second
      digitalWrite(motorPin, LOW);
      digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
      delay(1000);                       // wait for a second
    }
    delay(3000);
}
