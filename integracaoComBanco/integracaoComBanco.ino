#include <WiFi.h>
#include <WiFiClient.h>

const char* ssid = "DAAF 2.4GHz";
const char* password = "kj23812382";
const char* server_ip = "192.168.15.6";
const int server_port = 12345; // Porta do servidor

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  
  Serial.println("Conectado ao WiFi");
}

void loop() {
  WiFiClient client;
  int data_to_send;
  
  if (client.connect(server_ip, server_port)) {
    Serial.println("Conectado ao servidor");
    
    data_to_send = 1;
    
    // Envie os dados para o servidor
    client.print(data_to_send);
    
    // Encerre a conexão com o servidor
    client.stop();
    
    Serial.println("Dados enviados com sucesso");
  } else {
    Serial.println("Falha na conexão com o servidor");
  }
  
  delay(1000);  // Envie dados periodicamente
}