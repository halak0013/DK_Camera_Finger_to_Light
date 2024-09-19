# Parmakla led yakma   
Bu prozemizde mediapipe kütüpahanesini kullanarak parmaklarımızı tanıtıp her eklemimize indis verip parmaklarımız açıp kapatmamızla Deneyap Kart ile oluşturulmuş basit bir led devresindeki ledleri kontrol etmeyi öğreneceğiz.


<img src="doc/sheme.png" height = 300>

## Uygulamaya bşalamadan önce bazı hatırlatmalar!:
- Projeyi indireceğiniz yerin yolunda Türkçe karakter ve boşluk olmamamsı gerekmektedir. Yoksa kütüphane bulma işleminde sıkıntı çıkmaktadır. aşağıda olmaması gerekn birkaç örnek
  - /home/kullanici/TürkçeKarakter
  - /home/kullanici/yol/proje dosyası
- Ledlerin +,- bacaklarına dikkat etmek gerekmektedir(uzun(+) bacaklar kart pinlerine gelecektir)
- Sistemde python yüklü olması gerekmektedir
  - Pardus ve debian tabanlı sistemlerde otomatik gelmektedir. Gelmediyse `sudd apt install python3 python3-pip` uç birime yapıştırıp yüklebilirsiniz.
  - Windows için mağzadan aratıp kurabilirisiniz.


## İndirme
Projeyi indirmek için github sayfasındaki Yeşil **Code** yazan kısımdan zip olarak inder basmanız gerekmektedir.
<img src="doc/git_download.png" height=300>
> Git kullananlar için direk depoyu klonlayabilirler
```console
git clone https://github.com/halak0013/DK_Camera_Finger_to_Light.git
```

## Yükleme aşamaları
### Deneyap Kart yüklemesi
- Deneyap karta yükleme yapmak için ilk öncelikle Arduino üzerinden **finger_camera_server** projesini açmanız gerekmektedir. Sonrasında kod içinden ağ ayarlarının yapılması gerekmektedir
```c++
const char* ssid = "xxxxxxx";
const char* password = "xxxxxxx";
const char* udpAddress = "192.168.x.x";  // UDP istemcisinin IP adresi
const int udpPort = 12345;               // UDP portu
```
  - ssid -> Ağ adınız
  - password -> Ağ şifreniz
  - udpAddress -> Bilgisayarınızın yerel ip adresi (x.x kısmıları doldurulması gerekmektedir. bazen 192.168 ile başlamayabilir)
#### İp adresinizi bulmak için
- Pardusta Mağzadan **Sistem Gözlem Merkezi** uygulamasını kurup ağ kısmından bulabilirisiniz. Veya Pardus Hakkında uygulamasından bulabilirsiniz

<img src="doc/Pars_ip.png" height=300>
<img src="doc/Pars_ip2.png" height=300>
  
- Veya uç birimden `ip a` yazarak ip adresinizi öğrenebilirsiniz

----

- Windows için ise **Görev yönetcisinden** ağ kısmından bulabilirsiniz.
<img src="doc/win_ip.png" height=300>
  - Veya cmd içinde `ipconfig` yazarak öğrenebilirsiniz

#### Led pinlerini ayarlama
Karttan karta pin şeması değişebilir. Kullandığınız kartta göre aşağıdaki pin dizisini güncellemeniz gerekebilir.
```c++
//int lights[] = {D9, D12, D13, D14, D15 }; // Deneyap Kart
int lights[] = { D9, D10, D12, D13, D14 };  // Deneyap Kart 1A v2
```
Son olarak kartınızı seçip yükleme işlemini yapabilirsiniz.

### Python Kodu ayarları
İndirdiğinz projenin olduğu konumda istediğiniz kod editörünü kullanarak açmanız gerekmektedir. Bu projede vs code kullanacağız.

- İlk olarak eklentilerden Python ekentilerini kurmanız gerekmektedir.

<img src="doc/extension.png" height=150>

- Sonrasında **FingerCount.py** dosyasını açıp `shift+ctl+p` tuşlarına basıp `env` yazıdğınız zaman Python ortamı oluşturun veya Python: create environment basmanız gerekmektedir. Sonarsında sanal ortamı oluşturmanız gerekmektedir. 
- Oluşturma sırasında size bağımılıkları yüklemenizi isteyen bir seçenek çıkabilir. Seçip otomatik olarak kurulumu yapabilirisiniz

<img src="doc/requirements.png" height=70>

eğer otomatik çıkmazsa uç birim(terminale) yapıştırıp yükleyebilirsiniz.

`pip install -r requirements.txt`


## Çalıştırma

Deneyap Kart kodu attıktan sonra
Bilgisayardan **FingerCount.py** dosyasını çalıştırabilirsiniz.


Yararlandığım kaynaklar:
- https://github.com/murtazahassan
- https://chuoling.github.io/mediapipe/solutions/hands.html