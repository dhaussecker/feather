#include "accelerometer.h"
#include "flash_storage.h"
#include "bluetooth.h"
#include "rtc_clock.h"

#include <Wire.h>

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


int32_t lastUpdate = 30*8;  // Start with -5 minutes to trigger immediate advertising


unsigned long time=0;

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("Feather Equipment Monitor V0.0");
  delay(1000);
  Serial.println("TEST");
  // Initialize all modules
  configureFlash();
  initRTC();
  configureBLE();
  delay(5000);
  Serial.println("Setup complete!");

  if (timer_interrupt==true)
  {
    startPeriodicAdvertising();
  }

  //DEBUG
  readLogs("/time.txt");
  deleteLogs("/time.txt");
  readLogs("/state.txt");
  deleteLogs("/state.txt");
  //END DEBUG


  //Get previous time
  startRTC();
  previousTime = readRTC();
  Serial.println("Previous Time:");
  Serial.println(previousTime);

    //Convert nest time to nest UNIX TIME
  time = toUnixTimestamp(year,month,day,hour,minute,second);
  // Write nest timestamp to time.txt
  writeLogEntry(time, 0, 0, 0,"/time.txt", 0);

}

void loop() {
  
  //DUMMY FUNCTION to update sensor data
  //updateSensorData();



  // Log State Data
  newTime = readRTC();
  writeLogEntry(0, state,previousTime,newTime,"/state.txt", 1);
  previousTime = newTime;
  
  
  delay(2000);
  
  
  Serial.print("RTC:");
  Serial.println(readRTC());
  Serial.print("LastUpdate");
  Serial.println(lastUpdate);
  Serial.print("SUBTRACTION:");
  Serial.println(readRTC()-lastUpdate);

  if (readRTC() - lastUpdate >= 0) {  // 30 seconds (30secons*8tickspersecond) instead of 5 minutes
    Serial.println("ITS BEEN 30 SECONDS");
    
      // Check if advertising window is over (30 seconds)
  if (isAdvertising && (millis() - advertiseStartTime >= 30000)) {
    Serial.println("📡 Advertising window ended");
    isAdvertising = false;
    Bluefruit.Advertising.stop();
    digitalWrite(LED_BUILTIN, LOW);  // Turn off LED
  
  readLogs("/time.txt");
  deleteLogs("/time.txt");
  readLogs("/state.txt");
  deleteLogs("/state.txt");
  lastUpdate = 30*8;  // Start with -5 minutes to trigger immediate advertising
  resetRTC();
    //Convert nest time to nest UNIX TIME
  time = toUnixTimestamp(year,month,day,hour,minute,second);
  // Write nest timestamp to time.txt
  writeLogEntry(time, 0, 0, 0,"/time.txt", 0);
  
  }

  }
}