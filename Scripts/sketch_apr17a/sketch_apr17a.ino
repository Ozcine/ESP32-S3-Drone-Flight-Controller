//  Sadece donanımsal Motor Sürüşü!

//  U2(D0), U3(D1), U4(D2), U5(D3)
const int motorPins[4] = {D0, D1, D2, D3};

// Donanımsal PWM Fonksiyonumuz (Kütüphanesiz)
void escSinyalGonder(int motorIndex, int mikrosaniye) {
  int duty = mikrosaniye * 16384 / 20000; // 50Hz, 14-bit çözünürlük
  
  #if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
    ledcWrite(motorPins[motorIndex], duty);
  #else
    ledcWrite(motorIndex, duty);
  #endif
}

void setup() {
  Serial.begin(115200);
  delay(3000); 
  
  Serial.println("------------------------------------------");
  Serial.println("YALIN MOTOR TESTI BASLIYOR!");

  // 4 Motorun donanım kanallarını ayarla
  for(int i = 0; i < 4; i++) {
    #if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
      ledcAttach(motorPins[i], 50, 14); 
    #else
      ledcSetup(i, 50, 14);          
      ledcAttachPin(motorPins[i], i);   
    #endif
    
    // Güvenlik: Başlangıçta tüm motorlara SIFIR GAZ (1000us) yolla
    escSinyalGonder(i, 1000);
  }
  
  Serial.println("LUTFEN ANA LIPO PİLİ (veya 5V gücü) TAKIN!");
  Serial.println("ESC'lerin ARM (Kurulma) Bip sesleri gelecek...");
  
  // Sana pili takman ve ESC'lerin uyanması için tam 10 saniye veriyorum
  for(int i = 10; i > 0; i--) {
    Serial.print(i);
    Serial.println(" saniye sonra test baslayacak...");
    delay(1000);
  }
  
  Serial.println("DIKKAT: MOTORLAR DONMEYE BASLIYOR!");
}

void loop() {
  // ADIM 1: Rölantide (Çok yavaş) döndür
  Serial.println("Motorlar Rölantide (1050us)...");
  for(int i = 0; i < 4; i++) escSinyalGonder(i, 1050);
  delay(3000); // 3 saniye bu hızda kal
}
