// Define the joystick pins
const int VRX_PIN = 26; // Analog pin for the horizontal axis (X)
const int VRY_PIN = 27; // Analog pin for the vertical axis (Y)
const int SW_PIN = 25;  // Digital pin for the switch

// Define threshold values to determine joystick position
// These values may need to be adjusted based on your specific joystick module
const int CENTER_THRESHOLD_LOW = 1800;
const int CENTER_THRESHOLD_HIGH = 2200;
const int MOVEMENT_THRESHOLD_LOW = 500;  // Lower threshold for movement detection
const int MOVEMENT_THRESHOLD_HIGH = 3500; // Upper threshold for movement detection

void setup() {
  // Initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  // Set the switch pin as an input with an internal pull-up resistor
  // The switch is typically active LOW (connects to ground when pressed)
  pinMode(SW_PIN, INPUT_PULLUP);

  Serial.println("Joystick sketch started. Move the joystick and press the button.");
}

void loop() {
  // Read the analog values from the joystick axes
  int xValue = analogRead(VRX_PIN);
  int yValue = analogRead(VRY_PIN);

  // Read the digital value from the switch
  int switchState = digitalRead(SW_PIN);

  // Determine the horizontal position (X axis)
  String xPosition;
  if (xValue < MOVEMENT_THRESHOLD_LOW) {
    xPosition = "LEFT";
  } else if (xValue > MOVEMENT_THRESHOLD_HIGH) {
    xPosition = "RIGHT";
  } else if (xValue >= CENTER_THRESHOLD_LOW && xValue <= CENTER_THRESHOLD_HIGH) {
    xPosition = "CENTER";
  } else {
    // If not clearly left, right, or center, might be in transition
    xPosition = "MIDDLE_X"; // Or handle as needed
  }

  // Determine the vertical position (Y axis)
  String yPosition;
  if (yValue < MOVEMENT_THRESHOLD_LOW) {
    yPosition = "UP";
  } else if (yValue > MOVEMENT_THRESHOLD_HIGH) {
    yPosition = "DOWN";
  } else if (yValue >= CENTER_THRESHOLD_LOW && yValue <= CENTER_THRESHOLD_HIGH) {
    yPosition = "CENTER";
  } else {
    // If not clearly up, down, or center, might be in transition
    yPosition = "MIDDLE_Y"; // Or handle as needed
  }

  // Determine the switch state
  String switchStatus;
  if (switchState == LOW) {
    switchStatus = "PRESSED";
  } else {
    switchStatus = "NOT PRESSED";
  }

  // Print the joystick position and switch state to the serial monitor
  Serial.print("X: ");
  Serial.print(xValue);
  Serial.print(" (");
  Serial.print(xPosition);
  Serial.print("), Y: ");
  Serial.print(yValue);
  Serial.print(" (");
  Serial.print(yPosition);
  Serial.print("), Switch: ");
  Serial.println(switchStatus);

  // Add a small delay to make the output readable
  delay(100);
}
