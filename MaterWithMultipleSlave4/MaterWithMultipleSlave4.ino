#include <Adafruit_LiquidCrystal.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <LiquidCrystal_I2C.h>

#define CHANNEL 1

#define NUMSLAVES 3
esp_now_peer_info_t slaves[NUMSLAVES] = {};
int SlaveCnt = 0;
String SlaveName[NUMSLAVES];

#define PRINTSCANRESULTS 0

int buttonSwitchDev = 23;
int switcher = 0;
int buttonBlynk = 19;
int ledBlynk = 32;
int buttonForgotMode = 4;
int ledForgotmode = 26;

bool forgotMode = false;
int distance;
int distMax = 100;
int distDelay = 5000;

LiquidCrystal_I2C lcd(0x27,16,2);

// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    // Retry InitESPNow, add a counte and then restart?
    // InitESPNow();
    // or Simply Restart
    ESP.restart();
  }
}

// Scan for slaves in AP mode
void ScanForSlave() {
  int8_t scanResults = WiFi.scanNetworks(false, false, false, 300, CHANNEL);
  //reset slaves
  memset(slaves, 0, sizeof(slaves));
  SlaveCnt = 0;

  Serial.println("");
  if (scanResults == 0) {
    Serial.println("No WiFi devices in AP Mode found");
  } else {
    Serial.print("Found "); Serial.print(scanResults); Serial.println(" devices ");
    for (int i = 0; i < scanResults; ++i) {
      String SSID = WiFi.SSID(i);
      int32_t RSSI = WiFi.RSSI(i);
      String BSSIDstr = WiFi.BSSIDstr(i);

      Serial.println("");
      Serial.print("Name: "); Serial.print(SSID);

      if (PRINTSCANRESULTS) {
        Serial.print(i + 1); Serial.print(": "); Serial.print(SSID); Serial.print(" ["); Serial.print(BSSIDstr); Serial.print("]"); Serial.print(" ("); Serial.print(RSSI); Serial.print(")"); Serial.println("");
      }

      if(i == switcher){
          DistanceCalc();
        }
        Serial.println("\no index do escravo é: "); Serial.print(i);

      delay(10);

      if (SSID.indexOf("Slave") == 0) {
        Serial.print(i + 1); Serial.print(": "); Serial.print(SSID); Serial.print(" ["); Serial.print(BSSIDstr); Serial.print("]"); Serial.print(" ("); Serial.print(RSSI); Serial.print(")"); Serial.println("");
        // Get BSSID => Mac Address of the Slave
        int mac[6];
        if ( 6 == sscanf(BSSIDstr.c_str(), "%x:%x:%x:%x:%x:%x",  &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5] ) ) {
          for (int ii = 0; ii < 6; ++ii ) {
            slaves[SlaveCnt].peer_addr[ii] = (uint8_t) mac[ii];
          }
        }
        SlaveName[SlaveCnt] = WiFi.SSID(i);
        slaves[SlaveCnt].channel = CHANNEL; // pick a channel
        slaves[SlaveCnt].encrypt = 0; // no encryption

        SlaveCnt++;
      }
    }
  }

  if (SlaveCnt > 0) {
    Serial.print(SlaveCnt); Serial.println(" Slave(s) found, processing..");
  } else {
    Serial.println("No Slave Found, trying again.");
  }

  // clean up ram
  WiFi.scanDelete();
}

// Check if the slave is already paired with the master.
// If not, pair the slave with master
void manageSlave() {
  if (SlaveCnt > 0) {
    for (int i = 0; i < SlaveCnt; i++) {
      Serial.print("Processing: ");
      for (int ii = 0; ii < 6; ++ii ) {
        Serial.print((uint8_t) slaves[i].peer_addr[ii], HEX);
        if (ii != 5) Serial.print(":");
      }
      Serial.print(" Status: ");
      // check if the peer exists
      bool exists = esp_now_is_peer_exist(slaves[i].peer_addr);
      if (exists) {
        // Slave already paired.
        Serial.println("Already Paired");
      } else {
        // Slave not paired, attempt pair
        esp_err_t addStatus = esp_now_add_peer(&slaves[i]);

        delay(10);
      }
    }
  } else {
    // No slave found to process
    Serial.println("No Slave found to process");
  }
}


struct DataPackage {
  uint8_t data1;
  uint8_t data2;
  uint8_t data3;
};
// send data
void sendData() {

  DataPackage package;
  
  if(digitalRead(buttonBlynk) == HIGH)
  {
    package.data1 = 1;
    digitalWrite(ledBlynk, HIGH);
  }
  else
  {
    package.data1 = 0;
    digitalWrite(ledBlynk, LOW);
  }

  if(distance>distMax && forgotMode){

    package.data2 = 1;

    //delay(distDelay);
  }
  else{
    package.data2 = 0;
  }

  if(distance<255){
    package.data3 = distance;
  }
  else{
    package.data3 = 0;
  }
  
  const uint8_t *peer_addr = slaves[switcher].peer_addr;

  Serial.print("Sending: "); Serial.println(package.data1);

  esp_err_t result = esp_now_send(peer_addr, (uint8_t *)&package, sizeof(package));
  Serial.print("Send Status: ");

  if (result == ESP_OK) {
    Serial.println("Success");
  } else if (result == ESP_ERR_ESPNOW_NOT_INIT) {
    // How did we get so far!!
    Serial.println("ESPNOW not Init.");
  } else if (result == ESP_ERR_ESPNOW_ARG) {
    Serial.println("Invalid Argument");
  } else if (result == ESP_ERR_ESPNOW_INTERNAL) {
    Serial.println("Internal Error");
  } else if (result == ESP_ERR_ESPNOW_NO_MEM) {
    Serial.println("ESP_ERR_ESPNOW_NO_MEM");
  } else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
    Serial.println("Peer not found.");
  } else {
    Serial.println("Not sure what happened");
  }
  delay(10);
}

// callback when data is sent from Master to Slave
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Sent to: "); Serial.println(macStr);
  Serial.print("Last Packet Send Status: "); Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(115200);

  WiFi.setTxPower(WIFI_POWER_17dBm); //2db foi legal

  pinMode(buttonSwitchDev, INPUT);
  pinMode(buttonBlynk, INPUT);
  pinMode(ledBlynk, OUTPUT);
  pinMode(buttonForgotMode, INPUT);
  pinMode(ledForgotmode, OUTPUT);

  lcd.init();  
  lcd.clear();
  lcd.backlight();

  lcd.setCursor(0,0);   
  lcd.print("Disp :");
  lcd.setCursor(0,1);   
  lcd.print("Dist:");

  WiFi.mode(WIFI_STA);
  Serial.println("ESPNow/Multi-Slave/Master Example");
  Serial.print("STA MAC: "); Serial.println(WiFi.macAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
}

void loop() {

  ScanForSlave();

  ManageSwitch();

  if (SlaveCnt > 0) { // check if slave channel is defined
    // Add slave as peer if it has not been added already
    manageSlave();

    sendData();
  } else {
    // No slave found to process
  }

  ManageForgotMode();

  // wait for 3seconds to run the logic again
  delay(100);
}

void ManageSwitch() {
  
  int prev = 0;
  int current = digitalRead(buttonSwitchDev);

  if(current != prev && current == HIGH){
    if(switcher>SlaveCnt){
      switcher=0;
    }
    else{
      switcher++;
    }

  }
  prev = current;

  if (SlaveCnt > 0 && SlaveName[switcher]!=0){
    lcd.setCursor(6,0); 
    lcd.print("           ");
    delay(10);

    lcd.setCursor(6,0); 
    lcd.print(SlaveName[switcher].substring(SlaveName[switcher].indexOf('.') + 1));
  }
  else{
    lcd.setCursor(6,0); 
    lcd.print("Not Found");
    forgotMode = false;
    lcd.setCursor(5,1);
    lcd.print("Connect it");
  }
  lcd.setCursor(4,0);  
  lcd.print(switcher+1);
}

void ManageForgotMode() {

  int prev = 0;
  int current = digitalRead(buttonForgotMode);

  if(current != prev && current == HIGH){
    forgotMode = !forgotMode;
  }
  prev = current;

  if(forgotMode)
  {
    digitalWrite(ledForgotmode, HIGH);
  }
  else
  {
    digitalWrite(ledForgotmode, LOW);
  }
}

void DistanceCalc() {

  Serial.println("O nome do escravo é: "); Serial.print(WiFi.SSID(switcher));
  
  int buffer = 100;
  int j = 0;
  float media;
  int32_t tempRSSI;

  while(j<buffer){
        tempRSSI = WiFi.RSSI(switcher);
        media+=tempRSSI;
        delay(10);
        j++;
  }
  j = 0;
  media/=buffer;
  distance = pow(10.0, ((-40 - media) / (10.0 * 1)));
  media = 0;

  lcd.setCursor(5,1);  
  lcd.print("           ");
  delay(10);

  lcd.setCursor(5,1);
  lcd.print(distance);
  lcd.print("Units");

  Serial.println("\nA distância é: "); Serial.print(distance);
}