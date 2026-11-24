#include <WiFi.h>
#include <WebServer.h>
#include <list> 
#include <unordered_map> 
#include <string>

#include "Globals.h" // tree root 
void getCommands(); 

#define SSID "ESP32-GOBUSTER-TARGET"
#define PASS "gobuster"

WebServer server(80);
const unsigned long WINDOW_MS = 10000; // 10 second window
const int SPIKE_THRESHOLD = 15; // 15 404s in 10s
const int MAX_TRACKED_CLIENTS= 10; // 


std::unordered_map<std::string, int> ip_alert_counter = {};
std::unordered_map<std::string, int> ip_counter = {};
struct ClientTracker {
  IPAddress ip;
  int counter404;
  unsigned long windowStart;
  bool alert;

};

ClientTracker trackers[MAX_TRACKED_CLIENTS];

void record404(IPAddress clientIP) {
  unsigned long now = millis();
  int targetIndex = -1;
  int oldestIndex = 0;

  for (int i = 0; i < MAX_TRACKED_CLIENTS; i++) {
    if (trackers[i].ip == clientIP) {
      targetIndex = i;
      break;
    }
    if (trackers[i].windowStart < trackers[oldestIndex].windowStart) {
      oldestIndex = i;
    }
  }
  if (targetIndex == -1) {
    targetIndex = oldestIndex;
    trackers[targetIndex] = {clientIP, 0, now, false};
  }
  // reset counter after 10ms
  if (now - trackers[targetIndex].windowStart > WINDOW_MS) {
    trackers[targetIndex].counter404 = 0;
    trackers[targetIndex].windowStart = now;
    trackers[targetIndex].alert = false;
  }
  trackers[targetIndex].counter404++;

  // alert
  if (ip_counter.contains(clientIP.toString().c_str())) {
    ip_counter[clientIP.toString().c_str()]++;
  } else { 
    ip_counter[clientIP.toString().c_str()] = 1;
  }
  if (trackers[targetIndex].counter404 >= SPIKE_THRESHOLD && !trackers[targetIndex].alert) {
    trackers[targetIndex].alert = true;
    Serial.printf("[ALERT] 404 Spike detected from IP: %s (%d requests in %lus)\n",
    clientIP.toString().c_str(),
    trackers[targetIndex].counter404,
    WINDOW_MS / 1000);
    Serial.println();
    if (ip_alert_counter.contains(clientIP.toString().c_str())) { 
      ip_alert_counter[clientIP.toString().c_str()]++;

    } else {
      ip_alert_counter[clientIP.toString().c_str()] = 1; 
    } 
  }
}



void handleRoot() {
  server.send(200, "text/plain", "ESP32-BUST IS ACTIVE");
}

void handleNotFound() {
  IPAddress clientIP = server.client().remoteIP();
  record404(clientIP);
  server.send(404, "text/plain", "Not found");
}


void setup() {
  Serial.begin(115200);
  WiFi.softAP(SSID, PASS);
  Serial.print("AP Started...");
  Serial.println(WiFi.softAPIP());
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.begin();
}

void loop() {
  server.handleClient();
  getCommands(); 
  
  
}
