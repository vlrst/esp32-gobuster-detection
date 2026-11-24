#include <WiFi.h>
#include <WebServer.h>
#include "Globals.h"


void getCommands() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim(); 
    cmd.toLowerCase(); // returns void, not the string in lowercase
    if (cmd == "count") {
      if (ip_counter.empty()) {
        Serial.println("NO REQUESTS YET...");
      } else {
        for (const auto& [key, value] : ip_counter) {
          Serial.print(key.c_str());
          Serial.print(" : ");
          Serial.println(value);
        }
      }
    }
    if (cmd == "alert") {
      if (ip_alert_counter.empty()) {
        Serial.println("NO ALERTS YET...");
      } else {
        for (const auto& [key, value] : ip_alert_counter) {
          Serial.print(key.c_str()); // key is std::string so conversion to c_tr() is required
          Serial.print(" : ");
          Serial.println(value);
        }
      }
    }
  }
}