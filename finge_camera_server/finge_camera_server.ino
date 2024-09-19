#include <WiFi.h>
#include <WiFiUdp.h>
#include "esp_camera.h"

const char* ssid = "xxxxxxx";
const char* password = "xxxxxxx";
const char* udpAddress = "192.168.x.x";  // UDP istemcisinin IP adresi
const int udpPort = 12345;                 // UDP portu
int fingers[5] = { 0, 0, 0, 0, 0 };
//int lights[] = {D9, D12, D13, D14, D15 }; // Deneyap Kart
int lights[] = { D9, D10, D12, D13, D14 };  // Deneyap Kart 1A v2

WiFiUDP udp;

void cameraInit(void);

void setup() {
  Serial.begin(115200);
  Serial.println("Başladı");
  cameraInit();  // Kamera konfigürasyonu yapıldı

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
    camera_fb_t* fb = esp_camera_fb_get();  // Çerçeve verisini al

    if (!fb) {
      Serial.println("Kamera verisi alınamadı");
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      continue;
    }

    uint32_t totalBytes = fb->len;
    uint32_t bytesSent = 0;

    while (bytesSent < totalBytes) {
      uint32_t bytesToSend = min((uint32_t)maxPacketSize, totalBytes - bytesSent);
      udp.beginPacket(udpAddress, udpPort);
      udp.write(fb->buf + bytesSent, bytesToSend);
      udp.endPacket();

      bytesSent += bytesToSend;
    }

    //Serial.printf("Sent %d bytes in multiple packets\n", totalBytes);

    esp_camera_fb_return(fb);             // Çerçeve belleğini serbest bırak
    vTaskDelay(33 / portTICK_PERIOD_MS);  // 30 fps için 33ms bekle
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

void cameraInit(void) {
  // Kamera ayarları (kodunuzdaki gibi)
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = CAMD2;
  config.pin_d1 = CAMD3;
  config.pin_d2 = CAMD4;
  config.pin_d3 = CAMD5;
  config.pin_d4 = CAMD6;
  config.pin_d5 = CAMD7;
  config.pin_d6 = CAMD8;
  config.pin_d7 = CAMD9;
  config.pin_xclk = CAMXC;
  config.pin_pclk = CAMPC;
  config.pin_vsync = CAMV;
  config.pin_href = CAMH;
  config.pin_sscb_sda = CAMSD;
  config.pin_sscb_scl = CAMSC;
  config.pin_pwdn = -1;
  config.pin_reset = -1;

  config.xclk_freq_hz = 15000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_LATEST;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.fb_count = 2;
  config.jpeg_quality = 30;

  if (!psramFound()) {
    config.fb_location = CAMERA_FB_IN_DRAM;
    config.fb_count = 1;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Kamera başlatma hatası 0x%x", err);
    return;
  }

  sensor_t* s = esp_camera_sensor_get();
}
