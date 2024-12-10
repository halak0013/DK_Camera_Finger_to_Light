#include <WiFi.h>
#include <WiFiUdp.h>
#include "esp_camera.h"

const char* ssid = "EYUP DENEYAP";
const char* password = "MTH#122016?!.";

// const char* udpAddress = "192.168.2.120";  // UDP istemcisinin IP adresi
const char* udpAddress = "192.168.1.38";  // UDP istemcisinin IP adresi, mavi a7 
const int udpPort = 23451;                 // UDP portu
int fingers[5] = { 0, 0, 0, 0, 0 };
int lights[] = {D9, D12, D13, D14, D15 }; // Deneyap Kart
//int lights[] = {D13, D14, D12, D10, A7 };  // Deneyap Kart 1A v2
int valueToSend =0;
WiFiUDP udp;


void setup() {
  Serial.begin(115200);
  Serial.println("Başladı");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  for (int i = 0; i < 5; i++) {
    pinMode(lights[i], OUTPUT);
    digitalWrite(lights[i], LOW);
  }

  // UDP'yi başlat
  udp.begin(udpPort);

  // UDP verilerini göndermek için FreeRTOS görevi oluşturun
  xTaskCreate(sendUdpDataTask, "SendUdpData", 4096, NULL, 1, NULL);
  xTaskCreate(receiveDataTask, "ReceiveUdpData", 8096, NULL, 1, NULL);

}

void loop() {
  // Main loop can be used for other tasks
  delay(1000);
}

void receiveDataTask(void* parameter) {
  char incomingPacket[255];  // Gelen veriyi saklamak için buffer
  while (true) {
    // Gelen UDP paketlerinin olup olmadığını kontrol et
    int packetSize = udp.parsePacket();
    if (packetSize) {
      // Paket boyutunu ve gönderici bilgilerini al
      Serial.printf("Paket boyutu: %d\n", packetSize);
      int len = udp.read(incomingPacket, 255);  // Gelen veriyi buffer'a oku
      if (len > 0) {
        incomingPacket[len] = 0;  // String sonu null karakteri ekle
      }

      Serial.printf("Gelen UDP verisi: %s\n", incomingPacket);  // Veriyi ekrana yazdır
      parseUDPData(incomingPacket, fingers);
      for (int i = 0; i < 5; i++) {
        Serial.print("Parmak ");
        Serial.print(i);
        Serial.print(": ");
        Serial.println(fingers[i]);
      }
      finger_to_light();

      vTaskDelay(10 / portTICK_PERIOD_MS);  // 30 fps için 33ms bekle
    }
  }
}

void sendUdpDataTask(void* parameter) {
  const int maxPacketSize = 1400;  // UDP paketi başına maksimum boyut
  while (true) {
    udp.beginPacket(udpAddress, udpPort);
    valueToSend+=1.0;
    udp.write((const uint8_t*)&valueToSend, sizeof(valueToSend));
    udp.endPacket();

    vTaskDelay(1000 / portTICK_PERIOD_MS);  // 30 fps için 33ms bekle
  }
}


void parseUDPData(const char* udpData, int fingers[5]) {
  // Veriyi virgülle ayır
  char dataCopy[50];                                 // Verinin kopyasını alıyoruz
  strncpy(dataCopy, udpData, sizeof(dataCopy) - 1);  // Gelen veriyi kopyala
  dataCopy[sizeof(dataCopy) - 1] = '\0';             // Sonuna null karakter ekle

  // Veriyi virgüllerle ayır
  char* token = strtok(dataCopy, ",");
  int index = 0;

  // Veriyi int'e çevirip angles dizisine at
  while (token != nullptr && index < 5) {
    fingers[index] = (int)round(atof(token));  // Float'ı yuvarlayarak int'e çevir
    token = strtok(nullptr, ",");              // Bir sonraki kısmı al
    index++;
  }
}

void finger_to_light() {
  for (int i = 0; i < 5; i++) {
    if (fingers[i] == 1) {
      digitalWrite(lights[i], HIGH);
    } else {
      digitalWrite(lights[i], LOW);
    }
  }
}
