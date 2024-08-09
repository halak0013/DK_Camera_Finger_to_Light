/*
 *  KameraYuzTanima örneği,
 *  Bu örnek WiFi ağı ve şifresini girdikten sonra ağa bağlanacaktır. 
 *  WiFi ağına bağlandıktan sonra seri port ekranından görüntünün yayınlanacağı IP adresi yazılacaktır.
 * 
 *  Bu örnek kamera konnektörü dahili olan Deneyap Geliştirme Kartlarını desteklemektedir.  
*/
// ---------->>>>>>>>>> YUKLEME YAPILAMDAN DIKKAT EDILMESI GEREKEN HUSUS <<<<<<<<<<----------
// "Araclar->Partition Scheme->Huge APP" secilmeli //
// "Tools->Partition Scheme->Huge APP" secilmeli //

// Görüntü işlem ile haberleşme yapmak için arka planda pytohn kodlarını çalıştırmanız gerekmektedir.




#include "WiFi.h"
#include "esp_camera.h"
#include <AsyncTCP.h>
//Kullanılan karta göre buradaki pinleri değiştirilmelidir.
int lights[] = {D9, D12, D13, D14, D15 };


// WiFi ağ bilgilerinizi buraya ekleyin
const char* ssid = "*********";
const char* password = "*********";

void cameraInit(void);
void startCameraServer();

// Asenkron TCP sunucusu nesnesi
AsyncServer* server;

// Verileri tamponlayacağımız bir String değişkeni
String dataBuffer = "";

void onClientConnect(void* arg, AsyncClient* client) {
  Serial.println("Yeni bağlantı kabul edildi");

  // Gelen verileri işleyin
  client->onData([](void* arg, AsyncClient* client, void* data, size_t len) {
    String receivedData((char*)data, len);
    dataBuffer += receivedData;

    // Veriler '\n' karakterine ayrılıyor
    int index;
    while ((index = dataBuffer.indexOf('\n')) != -1) {
      String line = dataBuffer.substring(0, index);
      dataBuffer = dataBuffer.substring(index + 1);
      finger_to_light(line);
      // Gelen doğru veriyi işleyin
      Serial.println("Gelen veri: " + line);
    }

    // İstemciye yanıt gönderin
    // client->write("Veri alındı\n");
  });

  // Bağlantı kapatıldığında yapılacak işlemler
  client->onDisconnect([](void* arg, AsyncClient* client) {
    Serial.println("İstemci bağlantıyı kapattı");
    delete client;
  });
}



void setup() {
  Serial.begin(115200);  // Hata ayiklamak icin seri port ekran baslatildi
  Serial.setDebugOutput(true);
  Serial.println();

  for (int i = 0; i < 5; i++) {
    pinMode(lights[i], OUTPUT);
  }

  cameraInit();  // Kamera konfigurasyonu yapildi

  WiFi.begin(ssid, password);  // Wi-Fi agina baglaniliyor

  while (WiFi.status() != WL_CONNECTED) {  // Baglanti saglanan kadar bekleniyor
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Wi-Fi agina baglanildi ");

  startCameraServer();  // Kamera server baslatildi

  Serial.print("Kamera hazir! Baglanmak icin 'http://");  // Baglanti saglandi
  Serial.print(WiFi.localIP());                           // Goruntunun yayinlanacagi IP adresi seri port ekranına yaziliyor
  Serial.println("' adresini kullaniniz");


  // Asenkron TCP sunucusunu oluştur
  server = new AsyncServer(65000);             // 65000 portu örnek bir port
  server->onClient(onClientConnect, nullptr);  // İstemci bağlantısını yönet
  server->begin();
}

void loop() {
  delay(1000);
}

void cameraInit(void) {
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
  //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  //init with high specs to pre-allocate larger buffers                     for larger pre-allocated frame buffer.
  if (config.pixel_format == PIXFORMAT_JPEG) {
    if (psramFound()) {
      config.jpeg_quality = 10;
      config.fb_count = 2;
      config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
      // Limit the frame size when PSRAM is not available
      config.frame_size = FRAMESIZE_SVGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  } else {
    // Best option for face detection/recognition
    config.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
    config.fb_count = 2;
#endif
  }

  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t* s = esp_camera_sensor_get();
  // Drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);
}

void finger_to_light(String msg) {
  for (int i = 0; i < 5; i++) {
    Serial.println(msg.charAt(i * 3));
    if (msg.charAt(i * 3) == '1') {
      digitalWrite(lights[i], HIGH);
    } else {
      digitalWrite(lights[i], LOW);
    }
  }
}
