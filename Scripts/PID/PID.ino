#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <AlfredoCRSF.h>

HardwareSerial crsfSerial(1);
AlfredoCRSF crsf;
Adafruit_MPU6050 mpu;

// Motor Pinleri
const int motorPins[4] = {D0, D1, D2, D3}; 

// MPU6050 (Tekrar bakılacak)
float pitch_offset = 3.95;  
float roll_offset = -0.7; 

// --- PID KATSAYILARI  ---
float Kp_Roll = 1.2,  Ki_Roll = 0.04,  Kd_Roll = 0.8;
float Kp_Pitch = 1.2, Ki_Pitch = 0.04, Kd_Pitch = 0.8; 

// PID Hafıza Değişkenleri
float roll_integral = 0, roll_hata_onceki = 0;
float pitch_integral = 0, pitch_hata_onceki = 0;
unsigned long son_zaman;

// --- MOTOR SÜRÜŞ FONKSİYONU 
void escSinyalGonder(int motorIndex, int mikrosaniye) {
  // Güvenlik sınırları 
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
  Serial.begin(115200);
  delay(3000);

  // Alıcı Başlatma
  crsfSerial.begin(420000, SERIAL_8N1, D7, D6);
  crsf.begin(crsfSerial);

  // Sensör Başlatma
  Wire.begin(D4, D5);
  if (!mpu.begin()) {
    Serial.println("Sensör Bulunamadı!");
    while (1) yield();
  }
  
  // titreşim önleyici filtre!
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ); 

  // Motorları Başlat ve Sıfır Gaz Gönder
  for(int i = 0; i < 4; i++) {
    #if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
      ledcAttach(motorPins[i], 50, 14); 
    #else
      ledcSetup(i, 50, 14);          
      ledcAttachPin(motorPins[i], i);   
    #endif
    escSinyalGonder(i, 1000);
  }
  
  son_zaman = millis();
  Serial.println("PID SİSTEMİ AKTİF! GAZA BASILMASI BEKLENİYOR...");
}

void loop() {
  crsf.update();
  
  // --- 1. KUMANDAYI OKU (Ne İstiyoruz?) ---
  int ana_gaz = crsf.getChannel(3);      // CH3: Gaz (1000-2000)
  int kumanda_roll = crsf.getChannel(1); // CH1: Sağ/Sol (1000-2000)
  int kumanda_pitch = crsf.getChannel(2);// CH2: İleri/Geri (1000-2000)
  
  // Kumanda çubuklarını açıya (-30 ile +30 derece) çevir
  float hedef_roll = map(kumanda_roll, 1000, 2000, -30, 30);
  float hedef_pitch = map(kumanda_pitch, 1000, 2000, -30, 30); // İleri itince negatif değer verebilir, yönüne göre tersine çevirebiliriz.

  // --- 2. SENSÖRÜ OKU  ---
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  
  float raw_roll = atan2(a.acceleration.y, a.acceleration.z) * 180.0 / PI;
  float raw_pitch = atan2(-a.acceleration.x, a.acceleration.z) * 180.0 / PI; 
  
  float gercek_roll = raw_roll - roll_offset; 
  float gercek_pitch = raw_pitch - pitch_offset; 

  // --- 3. ZAMAN FARKI (dt) HESABI ---
  unsigned long simdiki_zaman = millis();
  float dt = (simdiki_zaman - son_zaman) / 1000.0;
  son_zaman = simdiki_zaman;
  if(dt <= 0.0) dt = 0.001; // Sıfıra bölme hatasını engelle

  // ==========================================
  // --- 4. ÇİFT EKSENLİ PID MATEMATİĞİ ---
  // ==========================================
  
  // ROLL (Sağ/Sol) PID
  float roll_hata = hedef_roll - gercek_roll;
  float roll_P = Kp_Roll * roll_hata;
  roll_integral += roll_hata * dt;
  float roll_I = Ki_Roll * roll_integral;
  float roll_D = Kd_Roll * ((roll_hata - roll_hata_onceki) / dt);
  roll_hata_onceki = roll_hata;
  float PID_Roll = roll_P + roll_I + roll_D;

  // PITCH (İleri/Geri) PID
  float pitch_hata = hedef_pitch - gercek_pitch;
  float pitch_P = Kp_Pitch * pitch_hata;
  pitch_integral += pitch_hata * dt;
  float pitch_I = Ki_Pitch * pitch_integral;
  float pitch_D = Kd_Pitch * ((pitch_hata - pitch_hata_onceki) / dt);
  pitch_hata_onceki = pitch_hata;
  float PID_Pitch = pitch_P + pitch_I + pitch_D;

  // ==========================================
  // --- 5. MOTOR DAĞITIMI (SİHİRLİ KARIŞIM) ---
  // ==========================================
  
  if (ana_gaz > 1050) { // Sadece gaz kolu itildiğinde çalışır
    
    /* 
     FİZİK KURALLARI:
     - Pitch pozitifse (Öne yat): Arka motorlar hızlanır (+), Ön motorlar yavaşlar (-)
     - Roll pozitifse (Sağa yat): Sol motorlar hızlanır (+), Sağ motorlar yavaşlar (-)
    */
    
    int motor_FL = ana_gaz - PID_Pitch + PID_Roll; // 0: Ön-Sol 
    int motor_FR = ana_gaz - PID_Pitch - PID_Roll; // 1: Ön-Sağ
    int motor_RL = ana_gaz + PID_Pitch + PID_Roll; // 2: Arka-Sol
    int motor_RR = ana_gaz + PID_Pitch - PID_Roll; // 3: Arka-Sağ

    escSinyalGonder(0, motor_FL);
    escSinyalGonder(1, motor_FR);
    escSinyalGonder(2, motor_RL);
    escSinyalGonder(3, motor_RR);
    
  } else {
    // Yerdeyken her şeyi sıfırla ki motorlar kendi kendine hızlanmasın
    for(int i=0; i<4; i++) escSinyalGonder(i, 1000);
    roll_integral = 0;
    pitch_integral = 0;
  }
}
