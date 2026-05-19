# 🛸 ESP32-S3 Tabanlı Özgün Drone Uçuş Kontrol Kartı (Flight Controller) & Donanım Tasarımı

Bu proje; ESP32-S3 mikrodenetleyicisi mimarisi etrafında şekillenen, çok katmanlı (multi-layer) ve yüksek kararlılık odaklı özgün bir drone uçuş kontrol kartı (PCB) ve gömülü yazılım geliştirme çalışmasıdır. Projenin ana odağı; harici uçuş kontrolörlerine bağımlılığı azaltarak, aviyonik sistem donanımını ve kontrol lojiğini tamamen özelleştirmektir.

---

## 🛠️ Donanım Mimarisi & Elektronik Tasarım (PCB)

Tasarım süreçleri **KiCad** EDA yazılımı kullanılarak, endüstriyel askeri donanım standartları (IPC kuralları) göz önünde bulundurularak gerçekleştirilmiştir.

- **Ana İşlemci:** ESP32-S3 (Dual-core Xtensa LX7, 240 MHz, entegre Wi-Fi & BLE). Donanımın ihtiyaç duyduğu yüksek hızlı sensör okuma ve kontrol döngüleri için optimize edilmiştir.
- **Güç Dağıtımı & Regülasyon:** LiPo pilden gelen yüksek akımı güvenli lojik seviyelere çekmek adına düşük gürültülü (Low-DropOut / LDO) regülatörler ve güç filtreleme hatları tasarlanmıştır. 
- **Sinyal ve Güç İzolasyonu:** Yüksek akım çeken ESC (Electronic Speed Controller) hatlarının ve fırçasız (BLDC) motorların oluşturduğu elektromanyetik parazitlerin (EMI) ve voltaj sıçramalarının (voltage spikes) hassas sensör hatlarını etkilememesi için şematik düzeyde izolasyon ve gürültü bastırma lojikleri uygulanmıştır.
- **Sensör Entegrasyonu:** Jiroskop ve ivmeölçer (IMU) birimleri, yüksek hızlı ve kararlı veri akışı sağlamak amacıyla **SPI** ve **I2C** veri yolları üzerinden mikrodenetleyiciye entegre edilmiştir.
- **Periferik Bağlantıları:** Telemetri modülleri, GPS ve harici alıcılar için ESP32-S3'ün çoklu donanımsal **UART** çıkışları optimize edilerek donanım arayüz yerleşimi yapılmıştır.

---

## 💻 Gömülü Yazılım & Kontrol Algoritmaları

- **Geliştirme Ortamı:** C/C++ (Arduino Core / ESP-IDF)
- **Haberleşme Protokolleri:** SPI, I2C, UART.
- **Motor Kontrol Lojiği:** Fırçasız motorların hassas devir kontrolü için ESC ünitelerine aktarılan PWM/DShot sinyal modülasyon optimizasyonları yapılmıştır.
- **Zamanlama & Kararlılık:** Sensör verilerinin gürültüden arındırılması ve kararlı bir uçuş algoritması çalıştırılması amacıyla Sinyaller ve Sistemler ile Kontrol Teorisi yaklaşımlarından yararlanılmıştır.

---

## 📂 Depo İçeriği (Repository Structure)

- `/hardware`: KiCad şematik çizimleri, PCB yerleşim şemaları (Gerber çıktıları) ve 3D render görüntüleri.
- `/firmware`: ESP32-S3 üzerinde koşan sensör okuma, ham veri işleme ve ESC sinyal modülasyon kodları.
- `/docs`: Projenin güç bütçesi hesaplamaları ve donanım blok diyagramları.

---

## 👤 Geliştirici & İletişim

- **Geliştirici:** Çağlar Özcine
- **LinkedIn:** [www.linkedin.com/in/çağlar-özçine-332b5239a](https://www.linkedin.com/in/%C3%A7a%C4%9Flar-%C3%B6z%C3%A7ine-332b5239a)
- **Telefon:** +90 534 783 47 20
