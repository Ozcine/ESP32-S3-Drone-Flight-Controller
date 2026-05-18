#include <AlfredoCRSF.h>

HardwareSerial crsfSerial(1);
AlfredoCRSF crsf;

const int motorPins[4] = {D0, D1, D2, D3}; //Motor çıkışları

void escSinyalGonder(int motorIndex, int mikrosaniye) {
  // GÜVENLİK KİLİDİ: Alıcıdan saçma bir veri gelirse motorların fırlamasını engeller
  if(mikrosaniye < 1000) mikrosaniye = 1000;
  if(mikrosaniye > 2000) mikrosaniye = 2000;
  
  int duty = mikrosaniye * 16384 / 20000;
  
  #if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
    ledcWrite(motorPins[motorIndex], duty);
  #else
    ledcWrite(motorIndex, duty); 
  #endif
}

void setup() {
  // 1. KRİTİK DÜZELTME: İşlemci uyanır uyanmaz hiç vakit kaybetmeden ESC'leri kur!
  // Bunu delay'in üstüne aldık ki pili taktığın an ESC'ler sinyali görsün.
  for(int i = 0; i < 4; i++) {
    #if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
      ledcAttach(motorPins[i], 50, 14); 
    #else
      ledcSetup(i, 50, 14);          
      ledcAttachPin(motorPins[i], i);   
    #endif
    escSinyalGonder(i, 1000); // 4 motora birden sıfır gaz bas
  }

  // Şimdi ekranı ve diğer şeyleri yavaştan başlatabiliriz
  Serial.begin(115200);
  delay(2000); 
  
  Serial.println("------------------------------------------");
  Serial.println("MOTORLAR 1000us GUVENLIK SINYALINE ALINDI");

  crsfSerial.begin(420000, SERIAL_8N1, D7, D6);
  crsf.begin(crsfSerial);
  
  Serial.println("LUTFEN ANA LIPO PİLİ TAKIN!");
  Serial.println("------------------------------------------");
  
  for (int i = 5; i > 0; i--) {
    Serial.print(i);
    Serial.println(" saniye icinde ESC'lerden kuruldum BİİİİİP sesi gelmeli...");
    delay(1000);
  }
  
  Serial.println("Sistem Hazir! Kumandayi atesleyin.");
}

void loop() {
  // 2. KRİTİK DÜZELTME: Bu fonksiyon asla delay ile yavaşlatılmamalıdır!
  crsf.update();
  
  // Alıcı henüz gücünü almadıysa (Mac'ten bağlıyken), getChannel(3) sıfır döndürebilir.
  // Biz yukarıdaki escSinyalGonder içine koyduğumuz kilit ile bu 0'ı 1000'e sabitledik. (Failsafe)
  int throttle = crsf.getChannel(3);

  // Kumandadan gelen tek gaz kolu (CH3) emrini, 4 motora birden yolla!
  for(int i = 0; i < 4; i++) {
    escSinyalGonder(i, throttle);
  }

  // İşlemciyi dondurmadan (delay kullanmadan) ekrana saniyede sadece 10 kere yazdırma taktiği
  static unsigned long son_yazdirma = 0;
  if(millis() - son_yazdirma > 100) {
    Serial.print("4 Motora Giden Sinyal (us): ");
    Serial.println(throttle);
    son_yazdirma = millis();
  }
}