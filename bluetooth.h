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

  bool waitForConnectionOrInterrupt(uint32_t sensor_interrupt) {
    uint32_t start = millis();

    while (millis() - start < timeout_ms) {
      // Check for interrupt
      if (wakeReason_interrupt) {
        Serial.println("Interrupt detected - aborting BLE");
        Bluefruit.Advertising.stop();
        return false;  // Exit early
      }

      // Check for connection
      if (Bluefruit.connected()) {
        return true;  // Connected!
      }

      // Sleep between checks (low power)
      sd_app_evt_wait();  // Wakes on any event
    }

    return false;  // Timeout
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
  
  // Check if it's our expected ACK pattern [0xAC, 0xCE, 0x55]
  if (len >= 3 && data[0] == 0xAC && data[1] == 0xCE && data[2] == 0x55) {
    Serial.println("✅ Valid ACK pattern received!");
    ackReceived = true;
    lastAckTime = millis();
    
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
  ackCharacteristic = BLECharacteristic(ACK_CHARACTERISTIC_UUID);
  ackCharacteristic.setProperties(CHR_PROPS_WRITE | CHR_PROPS_WRITE_WO_RESP);
  ackCharacteristic.setPermission(SECMODE_OPEN, SECMODE_OPEN);
  ackCharacteristic.setMaxLen(20);
  ackCharacteristic.setWriteCallback(ack_write_callback);
  ackCharacteristic.begin();

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

  Serial.print("Disconnected, reason = 0x"); Serial.println(reason, HEX);
  Serial.println("Advertising will restart automatically");
  
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