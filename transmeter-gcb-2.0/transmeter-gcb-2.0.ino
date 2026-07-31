// Transmitter Code (Corrected)
// This code runs on the ESP32 connected to the LSM6DSO accelerometer.

#include <Wire.h>
#include <Adafruit_LSM6DSOX.h> // Correct library is included
#include <esp_now.h>
#include <WiFi.h>

// ==> IMPORTANT: REPLACE WITH THE MAC ADDRESS OF YOUR ROBOT'S ESP32
uint8_t broadcastAddress[] = {0xC4, 0x4F, 0x33, 0x6A, 0x3F, 0x69};

// This structure must be the same on the receiver
typedef struct struct_message {
    char command[10]; // "forward", "back", "left", "right", "stop"
} struct_message;

// Create a struct_message object
struct_message gestureCommand;

// Create a peer interface
esp_now_peer_info_t peerInfo;

// ***************************************************************
// ==> FIX #1: Create an object for the correct sensor (LSM6DSOX)
// ***************************************************************
Adafruit_LSM6DSOX lsm6ds;

// ***************************************************************
// ==> FIX #2: Updated the callback function for modern ESP32 Core
// ***************************************************************
// This is the OLD signature required by your current ESP32 library version
void OnDataSent(const wifi_tx_info_t *tx_info, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(115200);

  // Initialize I2C for the accelerometer
  // Make sure to use the correct I2C address (0x6B) from our previous debugging
  if (!lsm6ds.begin_I2C(0x6B)) {
    Serial.println("Failed to find LSM6DSOX chip");
    while (1) { delay(10); }
  }
  Serial.println("LSM6DSOX Found!");

  // Set accelerometer range (2G is good for tilt)
  lsm6ds.setAccelRange(LSM6DS_ACCEL_RANGE_2_G);

  // Set ESP32 to Wi-Fi Station mode
  WiFi.mode(WIFI_STA);
  Serial.print("Controller MAC Address: ");
  Serial.println(WiFi.macAddress());

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the send callback
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  lsm6ds.getEvent(&accel, &gyro, &temp);

  float accelX = accel.acceleration.x;
  float accelY = accel.acceleration.y;

  // This variable will hold the last command to avoid sending duplicates
  static char lastCommand[10] = "stop";

  // Determine the current command based on hand tilt.
  // NOTE: You may need to adjust these threshold values (e.g., 4.0)
  if (accelY > 4.0) {
    strcpy(gestureCommand.command, "forward");
  } else if (accelY < -4.0) {
    strcpy(gestureCommand.command, "back");
  } else if (accelX > 4.0) {
    strcpy(gestureCommand.command, "left");
  } else if (accelX < -4.0) {
    strcpy(gestureCommand.command, "right");
  } else {
    strcpy(gestureCommand.command, "stop");
  }

  // Only send the command if it has changed from the last one
  if (strcmp(lastCommand, gestureCommand.command) != 0) {
    esp_now_send(broadcastAddress, (uint8_t *) &gestureCommand, sizeof(gestureCommand));
    
    // Print the command being sent for debugging
    Serial.print("Command Sent: ");
    Serial.println(gestureCommand.command);
    
    // Update the last command
    strcpy(lastCommand, gestureCommand.command);
  }

  delay(50); // Check for new gestures more frequently
}