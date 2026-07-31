// Robot (Receiver) Code - Updated for a modern motor driver shield (e.g., TB6612FNG)
// This code runs on the ESP32 controlling the robot's motors.

#include <esp_now.h>
#include <WiFi.h>

//======================================================================
// ==> IMPORTANT: Pin Definitions for a common ESP32 Motor Shield (like TB6612FNG)
// Please verify these pins against the labels on your specific motor driver board.
//======================================================================

// Standby Pin - This must be HIGH to enable the motors
const int STBY = 25; // Standby pin

// Motor A (e.g., Right Motor)
const int PWMA = 14; // Speed control (PWM)
const int AIN1 = 27; // Direction control
const int AIN2 = 26; // Direction control

// Motor B (e.g., Left Motor)
const int PWMB = 12; // Speed control (PWM)
const int BIN1 = 32; // Direction control
const int BIN2 = 33; // Direction control

// Define a motor speed (0-255)
const int motorSpeed = 200; // Adjusted for better control, you can set it back to 255 if needed

// Variables for the continuous status check
unsigned long lastRecvTime = 0;
unsigned long lastCheckTime = 0;
const long checkInterval = 2000;      // Check status every 2 seconds
const long connectionTimeout = 3000; // Consider connection lost after 3 seconds

// This structure must be the same on the transmitter
typedef struct struct_message {
    char command[10];
} struct_message;

// Create a struct_message object
struct_message receivedCommand;

// Flag to track if we have successfully received data once
bool peerConnected = false;

// Function to control motors based on command
void motorControl(const char* command) {
    // Enable the motor driver board
    digitalWrite(STBY, HIGH);

    if (strcmp(command, "forward") == 0) {
        // Motor A (Right) Forward
        digitalWrite(AIN1, HIGH);
        digitalWrite(AIN2, LOW);
        analogWrite(PWMA, motorSpeed);
        // Motor B (Left) Forward
        digitalWrite(BIN1, HIGH);
        digitalWrite(BIN2, LOW);
        analogWrite(PWMB, motorSpeed);
    } else if (strcmp(command, "back") == 0) {
        // Motor A (Right) Backward
        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, HIGH);
        analogWrite(PWMA, motorSpeed);
        // Motor B (Left) Backward
        digitalWrite(BIN1, LOW);
        digitalWrite(BIN2, HIGH);
        analogWrite(PWMB, motorSpeed);
    } else if (strcmp(command, "left") == 0) {
        // Motor A (Right) Forward (to turn left)
        digitalWrite(AIN1, HIGH);
        digitalWrite(AIN2, LOW);
        analogWrite(PWMA, motorSpeed);
        // Motor B (Left) Backward
        digitalWrite(BIN1, LOW);
        digitalWrite(BIN2, HIGH);
        analogWrite(PWMB, motorSpeed);
    } else if (strcmp(command, "right") == 0) {
        // Motor A (Right) Backward (to turn right)
        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, HIGH);
        analogWrite(PWMA, motorSpeed);
        // Motor B (Left) Forward
        digitalWrite(BIN1, HIGH);
        digitalWrite(BIN2, LOW);
        analogWrite(PWMB, motorSpeed);
    } else if (strcmp(command, "stop") == 0) {
        // Stop both motors (coast)
        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, LOW);
        analogWrite(PWMA, 0);
        digitalWrite(BIN1, LOW);
        digitalWrite(BIN2, LOW);
        analogWrite(PWMB, 0);

        // You can also brake the motors by setting both IN pins HIGH
        // Or disable the driver completely by setting STBY to LOW
        // digitalWrite(STBY, LOW);
    }
}

// Callback function that will be executed when data is received
void OnDataRecv(const esp_now_recv_info_t * recv_info, const uint8_t *incomingData, int len) {
  // Update the timestamp every time we receive data
  lastRecvTime = millis();

  // Show a one-time connection message
  if (!peerConnected) {
    Serial.println("\n------------------------------------");
    Serial.println("Peering successful! Receiving data. ✅");
    Serial.println("------------------------------------");
    peerConnected = true;
  }
  
  memcpy(&receivedCommand, incomingData, sizeof(receivedCommand));
  Serial.print("Command Received: ");
  Serial.println(receivedCommand.command);

  // Control motors based on the received command
  motorControl(receivedCommand.command);
}

void setup() {
  Serial.begin(115200);

  // Set motor control pins as OUTPUTs
  pinMode(STBY, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  Serial.println("Robot Receiver Initialized.");
  Serial.print("My MAC Address is: ");
  Serial.println(WiFi.macAddress()); // Print MAC for the transmitter
  Serial.println("------------------------------------");


  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register for the receive callback function
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // Check if 2 seconds have passed since the last status check
  if (millis() - lastCheckTime > checkInterval) {
    lastCheckTime = millis(); // Update the last check time

    // ==> This logic provides feedback on the connection status.
    if (peerConnected) {
      // If we are connected, check if the connection is still active
      if (millis() - lastRecvTime < connectionTimeout) {
        Serial.println("[Status] Connection OK ✅");
      } else {
        Serial.println("[Status] WARNING: Connection lost! ❌");
        // Stop the robot for safety if connection is lost
        motorControl("stop");
      }
    } else {
      // If we have never connected, keep showing this message
      Serial.println("[Status] Waiting for initial connection...");
    }
  }
}