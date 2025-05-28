/*
  ESP-NOW Multi Unit Demo
  esp-now-multi.ino
  Broadcasts control messages to all devices in network
  Load script on multiple devices
 
  DroneBot Workshop 2022
  https://dronebotworkshop.com
*/
 
// Include Libraries
#include <WiFi.h>
#include <esp_now.h>
#define ESP_NOW_MAX_DATA_LEN 250
// Define LED and pushbutton state booleans
bool buttonDown = false;
bool ledOn = false;
 
// Define LED and pushbutton pins
#define STATUS_LED 15
#define STATUS_BUTTON 5
 
 
void formatMacAddress(const uint8_t *macAddr, char *buffer, int maxLength)
// Formats MAC Address
{
  snprintf(buffer, maxLength, "%02x:%02x:%02x:%02x:%02x:%02x", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
}

 
// Define your motor control pins here
const int LEFT_MOTOR_FORWARD_PIN = 13;
const int LEFT_MOTOR_BACKWARD_PIN = 12;
const int RIGHT_MOTOR_FORWARD_PIN = 27;
const int RIGHT_MOTOR_BACKWARD_PIN = 26;
// passive buzzer
const int BUZZER_PIN = 22;


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

// Function to start the beep
void beepOn() {
  // Start the tone on the buzzer pin at 220Hz
  tone(BUZZER_PIN, 220);
}

// Function to stop the beep
void beepOff() {
  // Stop the tone playing on the buzzer pin
  noTone(BUZZER_PIN);
}


// Callback function executed when data is received via ESP-NOW
void receiveCallback(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int dataLen)
{
  // Get the sender's MAC address from the info structure
  const uint8_t *macAddr = esp_now_info->src_addr;

  // Only allow a maximum of 250 characters in the message + a null terminating byte
  char buffer[ESP_NOW_MAX_DATA_LEN + 1];
  int msgLen = min(ESP_NOW_MAX_DATA_LEN, dataLen);
  strncpy(buffer, (const char *)data, msgLen);

  // Make sure we are null terminated
  buffer[msgLen] = 0;

  // Format the MAC address
  char macStr[18];
  // Assuming you have a function called formatMacAddress defined elsewhere
  // formatMacAddress(macAddr, macStr, 18);

  // Send Debug log message to the serial port
  // Serial.printf("Received message from: %s - %s\n", macStr, buffer);
  Serial.printf("Received message: %s\n", buffer); // Simpler print if formatMacAddress is not available

  // Check the received broadcast and control motors
  if (strcmp("forward", buffer) == 0) {
    moveForward();
  } else if (strcmp("backward", buffer) == 0) {
    moveBackward();
  } else if (strcmp("right", buffer) == 0) {
    turnRight();
  } else if (strcmp("left", buffer) == 0) {
    turnLeft();
  } else if (strcmp("stop", buffer) == 0) {
    stopMotors();
  } else if (strcmp("beep on", buffer) == 0) {
    beepOn();
  } else if (strcmp("beep off", buffer) == 0) {
    beepOff();
  } 
  // You can add other commands here if needed, similar to the original ledOn/off
  // else if (strcmp("on", buffer) == 0)
  // {
  //   ledOn = true;
  // }
  // else if (strcmp("off", buffer) == 0)
  // {
  //   ledOn = false;
  // }
  // digitalWrite(STATUS_LED, ledOn);
}

// Remember to initialize your motor pins as OUTPUT in your setup() function
// e.g., pinMode(RIGHT_MOTOR_FORWARD_PIN, OUTPUT);
// and also initialize ESP-NOW.
 
void sentCallback(const uint8_t *macAddr, esp_now_send_status_t status)
// Called when data is sent
{
  char macStr[18];
  formatMacAddress(macAddr, macStr, 18);
  Serial.print("Last Packet Sent to: ");
  Serial.println(macStr);
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 
void broadcast(const String &message)
// Emulates a broadcast
{
  // Broadcast a message to every device in range
  uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  esp_now_peer_info_t peerInfo = {};
  memcpy(&peerInfo.peer_addr, broadcastAddress, 6);
  if (!esp_now_is_peer_exist(broadcastAddress))
  {
    esp_now_add_peer(&peerInfo);
  }
  // Send message
  esp_err_t result = esp_now_send(broadcastAddress, (const uint8_t *)message.c_str(), message.length());
 
  // Print results to serial monitor
  if (result == ESP_OK)
  {
    Serial.println("Broadcast message success");
  }
  else if (result == ESP_ERR_ESPNOW_NOT_INIT)
  {
    Serial.println("ESP-NOW not Init.");
  }
  else if (result == ESP_ERR_ESPNOW_ARG)
  {
    Serial.println("Invalid Argument");
  }
  else if (result == ESP_ERR_ESPNOW_INTERNAL)
  {
    Serial.println("Internal Error");
  }
  else if (result == ESP_ERR_ESPNOW_NO_MEM)
  {
    Serial.println("ESP_ERR_ESPNOW_NO_MEM");
  }
  else if (result == ESP_ERR_ESPNOW_NOT_FOUND)
  {
    Serial.println("Peer not found.");
  }
  else
  {
    Serial.println("Unknown error");
  }
}
 
void setup()
{
  // Set up Serial Monitor
  Serial.begin(115200);
  delay(1000);

  
  // Set ESP32 in STA mode to begin with
  WiFi.mode(WIFI_STA);
  Serial.println("ESP-NOW Broadcast Demo");
 
  // Print MAC address
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
 
  // Disconnect from WiFi
  WiFi.disconnect();
 
  // Initialize ESP-NOW
  if (esp_now_init() == ESP_OK)
  {
    Serial.println("ESP-NOW Init Success");
    esp_now_register_recv_cb(receiveCallback);
    esp_now_register_send_cb(sentCallback);
  }
  else
  {
    Serial.println("ESP-NOW Init Failed");
    delay(3000);
    ESP.restart();
  }
 
  // Pushbutton uses built-in pullup resistor
  pinMode(STATUS_BUTTON, INPUT_PULLUP);
 
  // LED Output
  pinMode(STATUS_LED, OUTPUT);

  // Buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  // Test the beep
  //beepOn();
  //delay(500);
  //beepOff();
 

  // Start your engines
  pinMode(RIGHT_MOTOR_FORWARD_PIN, OUTPUT);
  pinMode(RIGHT_MOTOR_BACKWARD_PIN, OUTPUT);
  pinMode(LEFT_MOTOR_FORWARD_PIN, OUTPUT);
  pinMode(LEFT_MOTOR_BACKWARD_PIN, OUTPUT);
  stopMotors();

}
 
void loop()
{
  delay(10);
}
