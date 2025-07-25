#include "accelerometer.h"
#include "flash_storage.h"
#include "bluetooth.h"
#include "rtc_clock.h"

#include <Wire.h>


// Interupt Flags
extern bool sensor_interrupt = false;
bool timer_interrupt = true;

//TIMESTAMP
int year = 2002;
int month = 11;
int day = 14;
int hour = 12;
int minute = 12;
int second = 12;

uint8_t state = 0;
int32_t previousTime = -1;
int32_t newTime = -1;


int32_t waitTime = 30*8;  // 30 seconds * 8 Hz


unsigned long time=0;

void setup() {
  Serial.begin(9600);
  delay(4000);
  Serial.println("Feather Equipment Monitor V0.0");


  Serial.println("TEST");
  // Initialize all modules
  configureFlash();
  initRTC();
  configureBLE();
  Serial.println("Setup complete!");



 
  //Get previous time
  startRTC();
  previousTime = readRTC();
  Serial.println("Previous Time:");
  Serial.println(previousTime);

    //Convert nest time to nest UNIX TIME
  time = toUnixTimestamp(year,month,day,hour,minute,second);
  // Write nest timestamp to time.txt
  deleteLogs("/time.txt");
  deleteLogs("/state.txt");
  writeLogEntry(time, 0, 0, 0,"/time.txt", 0);
  writeLogEntry(0, 1, 1, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 2, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 3, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 4, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 5, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 6, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 7, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 8, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 9, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 10, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 11, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 12, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 13, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 14, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 15, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 16, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 17, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 18, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 19, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 20, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 21, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 22, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 23, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 24, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 25, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 26, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 27, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 28, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 29, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 30, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 31, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 32, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 33, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 34, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 35, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 36, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 37, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 38, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 39, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 40, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 41, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 42, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 43, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 44, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 45, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 46, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 47, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 48, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 49, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 50, 0, "/state.txt", 1);
    writeLogEntry(0, 1, 1, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 2, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 3, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 4, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 5, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 6, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 7, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 8, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 9, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 10, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 11, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 12, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 13, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 14, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 15, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 16, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 17, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 18, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 19, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 20, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 21, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 22, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 23, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 24, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 25, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 26, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 27, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 28, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 29, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 30, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 31, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 32, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 33, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 34, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 35, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 36, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 37, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 38, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 39, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 40, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 41, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 42, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 43, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 44, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 45, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 46, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 47, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 48, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 49, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 50, 0, "/state.txt", 1);
    writeLogEntry(0, 1, 1, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 2, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 3, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 4, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 5, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 6, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 7, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 8, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 9, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 10, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 11, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 12, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 13, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 14, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 15, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 16, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 17, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 18, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 19, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 20, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 21, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 22, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 23, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 24, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 25, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 26, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 27, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 28, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 29, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 30, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 31, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 32, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 33, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 34, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 35, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 36, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 37, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 38, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 39, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 40, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 41, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 42, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 43, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 44, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 45, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 46, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 47, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 48, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 49, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 50, 0, "/state.txt", 1);
    writeLogEntry(0, 1, 1, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 2, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 3, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 4, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 5, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 6, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 7, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 8, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 9, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 10, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 11, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 12, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 13, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 14, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 15, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 16, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 17, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 18, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 19, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 20, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 21, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 22, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 23, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 24, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 25, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 26, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 27, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 28, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 29, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 30, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 31, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 32, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 33, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 34, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 35, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 36, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 37, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 38, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 39, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 40, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 41, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 42, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 43, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 44, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 45, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 46, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 47, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 48, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 49, 0, "/state.txt", 1);
  writeLogEntry(0, 1, 50, 0, "/state.txt", 1);

  readLogs("/time.txt");
  readLogs("/state.txt");
   
}

void loop() {
  
  // Handle timer interrupt for BLE advertising
  if (timer_interrupt) {
    timer_interrupt = false;  // Clear flag
    Serial.println("Timer interrupt - starting BLE advertising");
    updateSensorData();  // Update characteristic with fresh data before advertising
    startPeriodicAdvertising();
  }
  
  // Check if connected during advertising window
  if (isAdvertising && Bluefruit.connected()) {
    Serial.println("🔗 Connected! Sending stored data immediately...");
    
    // Stop advertising since we're connected
    Bluefruit.Advertising.stop();
    
    // Send stored data immediately (don't wait for ACK)
    // TODO: Replace this with actual data sending via notify
    readLogs("/state.txt");  // This just prints to serial
    
    // Now wait for ACK to confirm Particle received the data
    unsigned long ackWaitStart = millis();
    while (Bluefruit.connected() && (millis() - ackWaitStart < 5000)) {
      if (ackReceived) {
        Serial.println("✅ ACK received - Particle confirmed data receipt!");
        ackReceived = false;  // Reset for next time
        break;
      }
      delay(100);
    }
    
    if (!ackReceived && !Bluefruit.connected()) {
      Serial.println("⚠️ M-SoM disconnected without confirming data receipt");
    }
    
    // Mark advertising as done
    isAdvertising = false;
    digitalWrite(LED_BUILTIN, LOW);
  }
  
  // Check if advertising window timed out (30 seconds)
  if (isAdvertising && (millis() - advertiseStartTime >= 30000)) {
    Serial.println("📡 Advertising window ended - no connection");
    isAdvertising = false;
    Bluefruit.Advertising.stop();
    digitalWrite(LED_BUILTIN, LOW);  // Turn off LED
    //GO BACK TO SLEEP
  }



  // Handle sensor interrupt for logging
  if (sensor_interrupt) {
    sensor_interrupt = false;  // Clear flag
    Serial.println("Sensor interrupt - logging state change");
    
    // Log State Data
    newTime = readRTC();
    writeLogEntry(0, state,previousTime,newTime,"/state.txt", 1);
    previousTime = newTime;
  }
  
  
  // Add proper delay to allow BLE callbacks to process
  delay(1000);

  if (readRTC() - waitTime >= 0) {  // 30 seconds (30seconds*8tickspersecond)
    Serial.println("ITS BEEN 30 SECONDS");
    
    waitTime = 30*8;  // Start with -5 minutes to trigger immediate advertising
    resetRTC();
    //Convert nest time to nest UNIX TIME
    time = toUnixTimestamp(year,month,day,hour,minute,second);
    // Write nest timestamp to time.txt
    writeLogEntry(time, 0, 0, 0,"/time.txt", 0);
  }
}