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
 
// Define LED and pushbutton state booleans
bool buttonDown = false;
bool ledOn = false;
 
// Define LED and pushbutton pins
#define STATUS_LED 15
#define STATUS_BUTTON 5
 
String command = ""; // String to hold incoming commands
bool commandComplete = false; // Flag to indicate if a command is received

 
void formatMacAddress(const uint8_t *macAddr, char *buffer, int maxLength)
// Formats MAC Address
{
  snprintf(buffer, maxLength, "%02x:%02x:%02x:%02x:%02x:%02x", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
}
 
 
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
  formatMacAddress(macAddr, macStr, 18);

  // Send Debug log message to the serial port
  Serial.printf("Received message from: %s - %s\n", macStr, buffer);

  // Check switch status
  if (strcmp("on", buffer) == 0)
  {
    ledOn = true;
  }
  else
  {
    ledOn = false;
  }
  digitalWrite(STATUS_LED, ledOn);
}
 
 
void sentCallback(const uint8_t *macAddr, esp_now_send_status_t status)
// Called when data is sent
{
  char macStr[18];
  formatMacAddress(macAddr, macStr, 18);
  // Serial.print("Last Packet Sent to: ");
  // Serial.println(macStr);
  // Serial.print("Last Packet Send Status: ");
  // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
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
    // Serial.println("Broadcast message success");
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
  Serial.setTimeout(10); // fix the bullshit with the laggy serial response
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

  Serial.println("Arduino Ready"); // Signal to Python that Arduino is ready
}
 
void loop()
{
  // Read incoming serial data
  if (Serial.available() > 0) {
    String msg = Serial.readString();
    msg.trim();
    // Serial.println("Message received:");
    // Serial.println(msg);
    if (msg == "FOO") {
      Serial.println("BAR");
    }

    broadcast(msg);
  }
  if (digitalRead(STATUS_BUTTON))
  {
    // Detect the transition from low to high
    if (!buttonDown)
    {
      buttonDown = true;
      
      // Toggle the LED state
      ledOn = !ledOn;
      digitalWrite(STATUS_LED, ledOn);
      
      // Send a message to all devices
      if (ledOn)
      {
        broadcast("on");
      }
      else
      {
        broadcast("off");
      }
    }
  
    delay(10);
  }
  else
  {
    // Reset the button state
    buttonDown = false;
  }
  String command = "";
  bool commandComplete = false;
}
