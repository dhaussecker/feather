#ifndef BLUETOOTH_H
#define BLUETOOTH_H


#include <bluefruit.h>

// UUID that matches your Particle hub
#define SERVICE_UUID                   "12345678-1234-1234-1234-123456789abc"
#define CHARACTERISTIC_UUID            "87654321-4321-4321-4321-cba987654321"
#define ACK_CHARACTERISTIC_UUID        "11223344-5566-7788-99aa-bbccddeeff00"

// BLE Service and Characteristics
BLEService        testService;
BLECharacteristic testCharacteristic;
BLECharacteristic ackCharacteristic;


// Device info
const char* DEVICE_NAME = "nRF_Sensor_01";
int counter = 0;

bool ackReceived = false;
unsigned long lastAckTime = 0;
bool isAdvertising = false;
unsigned long advertiseStartTime = 0;

void updateSensorData() {
  counter++;
  
  // Simulate some sensor readings
  int temperature = random(18, 35);  // Random temp 18-35°C
  int humidity = random(30, 80);     // Random humidity 30-80%
  int motion = random(0, 2);         // Random motion 0 or 1
  
  // Create simple data string (keep under 20 bytes)
  String data = "T:" + String(temperature) + ",H:" + String(humidity) + ",M:" + String(motion);
  
  // Always update the characteristic data (ready for when connected)
  testCharacteristic.write(data.c_str());
  
  Serial.print("📊 Sensor data updated: " + data);
  Serial.print(" (Cycle #");
  Serial.print(counter);
  Serial.println(")");
  
  // Reset ACK status for new cycle
  ackReceived = false;
}

void startPeriodicAdvertising() {
  Serial.println("📡 Starting 30-second advertising window");
  isAdvertising = true;
  advertiseStartTime = millis();
  Bluefruit.Advertising.start(30);  // Advertise for exactly 30 seconds
}

void sendLogDataViaNotify() {
  if (!Bluefruit.connected()) {
    Serial.println("No connection - cannot send log data");
    return;
  }
  
  Serial.println("📤 Sending stored log data via BLE notifications...");
  
  // Send start marker
  String startMsg = "LOG_START";
  testCharacteristic.notify(startMsg.c_str());
  delay(50);
  
  int totalBytesSent = 0;
  
  // First send time data
  if (InternalFS.exists("/time.txt")) {
    File timeFile = InternalFS.open("/time.txt", FILE_O_READ);
    if (timeFile) {
      Serial.println("Sending time.txt contents:");
      while (timeFile.available()) {
        String line = timeFile.readStringUntil('\n');
        if (line.length() > 0) {
          // Add newline back
          line += "\n";
          
          // Send complete lines up to 18 bytes
          while (line.length() > 0) {
            String chunk = line.substring(0, min(18, (int)line.length()));
            line = line.substring(chunk.length());
            
            testCharacteristic.notify(chunk.c_str());
            totalBytesSent += chunk.length();
            Serial.print("Sent chunk: ");
            Serial.print(chunk);
            delay(50); // Optimized delay for maximum speed
          }
        }
      }
      timeFile.close();
    }
  }
  
  // Then send state data
  if (InternalFS.exists("/state.txt")) {
    File stateFile = InternalFS.open("/state.txt", FILE_O_READ);
    if (stateFile) {
      Serial.println("Sending state.txt contents:");
      while (stateFile.available()) {
        String line = stateFile.readStringUntil('\n');
        if (line.length() > 0) {
          // Add newline back
          line += "\n";
          
          // Send complete lines up to 18 bytes
          while (line.length() > 0) {
            String chunk = line.substring(0, min(18, (int)line.length()));
            line = line.substring(chunk.length());
            
            testCharacteristic.notify(chunk.c_str());
            totalBytesSent += chunk.length();
            Serial.print("Sent chunk: ");
            Serial.print(chunk);
            delay(50); // Optimized delay for maximum speed
          }
        }
      }
      stateFile.close();
    }
  }
  
  // Send end marker
  String endMsg = "LOG_END";
  testCharacteristic.notify(endMsg.c_str());
  delay(50);
  
  Serial.print("📤 Log data transmission complete. Total bytes sent: ");
  Serial.println(totalBytesSent);
}

void startAdv(void) {
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include the service UUID in advertising packet
  Bluefruit.Advertising.addService(testService);

  // Include Name
  Bluefruit.Advertising.addName();
  
  /* Setup Advertising for periodic use
   * - DON'T restart on disconnect (we control when to advertise)
   * - Fast interval for quick discovery
   * - We'll manually start/stop advertising
   */
  Bluefruit.Advertising.restartOnDisconnect(false);  // Manual control
  Bluefruit.Advertising.setInterval(32, 244);        // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);          // 30 seconds fast mode
}

// Callback for when ACK is received
void ack_write_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len) {
  digitalWrite(LED_BUILTIN, LOW);  // Flash LED immediately
  delay(50);
  digitalWrite(LED_BUILTIN, HIGH);
  
  Serial.println("================================");
  Serial.println("🚨 ACK CALLBACK TRIGGERED! 🚨");
  Serial.println("================================");
  Serial.print("ACK received! Length: ");
  Serial.print(len);
  Serial.print(" Data: ");
  
  for (int i = 0; i < len; i++) {
    Serial.print("0x");
    if (data[i] < 0x10) Serial.print("0");
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  
  // Check if it's our expected ACK pattern [0xAC, 0xCE, 0x55] + timestamp
  if (len >= 3 && data[0] == 0xAC && data[1] == 0xCE && data[2] == 0x55) {
    Serial.println("✅ Valid ACK pattern received!");
    
    // If we have timestamp data (7 bytes total)
    if (len >= 7) {
      // Parse UTC timestamp (4 bytes, little-endian)
      uint32_t timestamp = ((uint32_t)data[6] << 24) | 
                          ((uint32_t)data[5] << 16) | 
                          ((uint32_t)data[4] << 8) | 
                          data[3];
      
      Serial.print("📅 Particle UTC timestamp: ");
      Serial.println(timestamp);
      
      // Convert to human-readable format
      // Basic conversion (days since Jan 1, 1970)
      uint32_t days = timestamp / 86400;
      uint32_t hours = (timestamp % 86400) / 3600;
      uint32_t minutes = (timestamp % 3600) / 60;
      uint32_t seconds = timestamp % 60;
      
      Serial.print("📅 Particle time: Day ");
      Serial.print(days);
      Serial.print(" ");
      Serial.print(hours);
      Serial.print(":");
      if (minutes < 10) Serial.print("0");
      Serial.print(minutes);
      Serial.print(":");
      if (seconds < 10) Serial.print("0");
      Serial.println(seconds);
    }
    
    ackReceived = true;
    lastAckTime = millis();
    
    // Now send the stored log data back to Particle
    sendLogDataViaNotify();
  }
}

void setupBLEService(void) {
  // Create service with your UUID
  testService = BLEService(SERVICE_UUID);
  testService.begin();

  // Create data characteristic (readable and notifiable)
  testCharacteristic = BLECharacteristic(CHARACTERISTIC_UUID);
  testCharacteristic.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY);
  testCharacteristic.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  testCharacteristic.setMaxLen(20);
  testCharacteristic.begin();

  // Create ACK characteristic (writable)
  Serial.println("Creating ACK characteristic...");
  ackCharacteristic = BLECharacteristic(ACK_CHARACTERISTIC_UUID);
  ackCharacteristic.setProperties(CHR_PROPS_WRITE | CHR_PROPS_WRITE_WO_RESP);
  ackCharacteristic.setPermission(SECMODE_OPEN, SECMODE_OPEN);
  ackCharacteristic.setMaxLen(20);
  ackCharacteristic.setWriteCallback(ack_write_callback);
  uint8_t result = ackCharacteristic.begin();
  Serial.print("ACK characteristic begin() returned: ");
  Serial.println(result);
  Serial.print("ACK UUID: ");
  Serial.println(ACK_CHARACTERISTIC_UUID);

  // Set initial value
  String initialValue = "XIAO Ready";
  testCharacteristic.write(initialValue.c_str());
}

// Callback invoked when a connection is established
void connect_callback(uint16_t conn_handle) {
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  char central_name[32] = { 0 };
  connection->getPeerName(central_name, sizeof(central_name));

  Serial.print("Connected to ");
  Serial.println(central_name);
  Serial.print("Connection handle: ");
  Serial.println(conn_handle);
  
  // Reset ACK status on new connection
  ackReceived = false;
  
  // Keep LED on when connected
  digitalWrite(LED_BUILTIN, HIGH);
}

/**
 * Callback invoked when a connection is dropped
 * @param conn_handle connection where this event happens
 * @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
 */
void disconnect_callback(uint16_t conn_handle, uint8_t reason) {
  (void) conn_handle;
  (void) reason;

  Serial.print("Disconnected at millis=");
  Serial.print(millis());
  Serial.print(", reason = 0x"); 
  Serial.println(reason, HEX);
  Serial.println("Advertising will restart automatically");
  
  // Check if we ever got ACK
  if (ackReceived) {
    Serial.println("Note: ACK was received before disconnect");
  }
  
  // Reset ACK status
  ackReceived = false;
  
  // Turn off LED when disconnected
  digitalWrite(LED_BUILTIN, LOW);
}

void configureBLE()
{
  Serial.println("Initializing the Bluefruit nRF52 module");
  Bluefruit.begin();
  // Set max tx power for better range
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
  // Set device name
  Bluefruit.setName(DEVICE_NAME);
  // Set connect/disconnect callbacks
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);
  // Setup the test service
  Serial.println("Configuring the Test Service");
  setupBLEService();
  // Setup the advertising packet
  Serial.println("Setting up the advertising payload");
  startAdv();
  Serial.println("Initial advertising started");
  Serial.print("Device name: ");
  Serial.println(DEVICE_NAME);
  Serial.print("Service UUID: ");
  Serial.println(SERVICE_UUID);

  // Stop initial advertising - will restart periodically
  Bluefruit.Advertising.stop();
  Serial.println("Initial advertising stopped - will advertise every 5 minutes");

}



#endif // BLUETOOTH_H