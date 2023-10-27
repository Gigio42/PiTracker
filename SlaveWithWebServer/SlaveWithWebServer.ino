/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-esp-now-wi-fi-web-server/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <esp_now.h>
#include <WiFi.h>
#include "ESPAsyncWebServer.h"
#include <Arduino_JSON.h>

#define CHANNEL 1

// Replace with your network credentials (STATION)
const char* ssid = "DAAF 2.4GHz";
const char* password = "kj23812382";

int ledR = 2;

struct DataPackage {
  uint8_t data1;
  uint8_t data2;
  uint8_t data3;
};

DataPackage receivedData;

JSONVar board;

AsyncWebServer server(80);
AsyncEventSource events("/events");

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) { 
  // Copies the sender mac address to a string
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&receivedData, incomingData, sizeof(receivedData));

  if (receivedData.data1 == 1) {
      digitalWrite(ledR, HIGH);
  } 
  else {
      digitalWrite(ledR, LOW);
  }
  
  board["signal"] = receivedData.data1;
  board["limit"] = receivedData.data2;
  board["distance"] = receivedData.data3;
  String jsonString = JSON.stringify(board);
  events.send(jsonString.c_str(), "new_readings", millis());
  
  Serial.printf("Signal %.2f: %u bytes\n", receivedData.data1, len);
  Serial.printf("Limit: %.2f \n", receivedData.data2);
  Serial.printf("Distance: %.2f \n", receivedData.data3);
  Serial.println();
}

const char index_html[] PROGMEM = R"rawliteral(

<!DOCTYPE HTML>
<html>
<head>
  <title>iTracker</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    p {font-size: 1.2rem;}
    body {
      margin: 0;
      background-color: #616161;
    }
    .topnav {overflow: hidden; background-color: #3c3c3c; color: white; font-size: 1.7rem;}
    .content {
      padding: 20px;
      margin-top: 20px;
      margin-bottom: 20px;
    }
    .reading {font-size: 2.8rem;}
    .card {
      background-color: #787878;
      border-radius: 10px;
      box-shadow: 0 0 10px 0 rgba(0, 0, 0, 0.1);
      padding: 20px;
      margin-bottom: 20px; /* Adiciona espaço na parte inferior de cada cartão */
      }
    .value {font-size: 2.0rem; font-weight: bold; color: #f9f9f9;}
    .label {font-size: 1.4rem; color: #f9f9f9;}
  </style>
</head>
<body>
  <div class="topnav">
    <h3>iTracker</h3>
  </div>
  <div class="content">
    <div class="card">
      <h4><i class="fas fa-chart-line"></i> LIMIT</h4>
      <p class="value" id="limit"></p>
      <p class="label">Limit Value</p>
    </div>
    <div class="card">
      <h4><i class="fas fa-ruler"></i> DISTANCE</h4>
      <p class="value" id="distance"></p>
      <p class="label">Distance Measurement</p>
    </div>
    <div class="card">
      <h4><i class="fas fa-wifi"></i> SIGNAL</h4>
      <p class="value" id="signal"></p>
      <p class="label">Signal Strength</p>
    </div>
  </div>
  <script>
    if (!!window.EventSource) {
      var source = new EventSource('/events');
 
      source.addEventListener('open', function(e) {
        console.log("Events Connected");
      }, false);
      source.addEventListener('error', function(e) {
        if (e.target.readyState != EventSource.OPEN) {
          console.log("Events Disconnected");
        }
      }, false);

      source.addEventListener('new_readings', function(e) {
        console.log("new_readings", e.data);
        var obj = JSON.parse(e.data);
        document.getElementById("limit").innerHTML = obj.limit.toFixed(2);
        document.getElementById("distance").innerHTML = obj.distance.toFixed(2);
        document.getElementById("signal").innerHTML = obj.signal.toFixed(2);
      }, false);
    }
  </script>
</body>
</html>
</html>)rawliteral";

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  pinMode(ledR, OUTPUT);

  // Set the device as a Station and Soft Access Point simultaneously
  WiFi.mode(WIFI_AP_STA);

  const char *SSID = "Slave.Dark";
  bool result = WiFi.softAP(SSID, "", CHANNEL, 0);
  if (!result) {
    Serial.println("AP Config failed.");
  } else {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
    Serial.print("AP CHANNEL "); Serial.println(WiFi.channel());
  }
  
  // Set device as a Wi-Fi Station
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Setting as a Wi-Fi Station..");
  }
  Serial.print("Station IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Wi-Fi Channel: ");
  Serial.println(WiFi.channel());

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });
   
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);
  server.begin();
}
 
void loop() {
  static unsigned long lastEventTime = millis();
  static const unsigned long EVENT_INTERVAL_MS = 5000;
  if ((millis() - lastEventTime) > EVENT_INTERVAL_MS) {
    events.send("ping",NULL,millis());
    lastEventTime = millis();
  }
}
