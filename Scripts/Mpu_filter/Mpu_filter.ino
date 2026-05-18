#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <math.h>

#define I2C_SDA 5  // Karttaki D4 pini
#define I2C_SCL 6  // Karttaki D5 pini

Adafruit_MPU6050 mpu;

float pitch_offset = 3.95;  
float roll_offset = -0.7;  

// Değişkenler
float pitch = 0;  // Drone eğim açısı
float roll = 0;
float previousTime, currentTime, elapsedTime;  // iki dögü arasında geçen zaman

void setup(void) {
  Serial.begin(115200);
  
  Wire.begin(I2C_SDA, I2C_SCL);

  if (!mpu.begin()) {
    Serial.println("Sensör Yok! Kabloları (SDA->D4, SCL->D5) kontrol et.");
    while (1) yield();
  }

  // Drone Ayarları
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ); // Yüksek frekanslı titreşimleri alçaltır (alçak geçiren filtre)

  // Zaman sayacını başlat
  previousTime = millis(); // ilk referans zamanı (milisaniye)
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Zaman hesapla
  currentTime = millis();
  elapsedTime = (currentTime - previousTime) / 1000.0; // Saniyeye çevir
  previousTime = currentTime;    

  // İvme ölçerin açısı hesaplandı
  float acc_roll = atan2(a.acceleration.y, a.acceleration.z) * 180.0 / PI;   
  float acc_pitch = atan2(-a.acceleration.x, a.acceleration.z) * 180.0 / PI;

  // Kalibrasyon
  acc_roll -= roll_offset;
  acc_pitch -= pitch_offset;

  //  GYRO Verisini Al 
  float gyro_roll_rate = g.gyro.x * 180.0 / PI; //roll eksenin dönüş hızı
  float gyro_pitch_rate = g.gyro.y * 180.0 / PI; //pitch ekseninin dönüş hızı

  // SENSÖR FÜZYONU 
  //  (Eski Açı + Gyro Değişimi) * %96 + (İvmeölçer Açısı) * %4
  
  roll = 0.96 * (roll + gyro_roll_rate * elapsedTime) + 0.04 * acc_roll;
  pitch = 0.96 * (pitch + gyro_pitch_rate * elapsedTime) + 0.04 * acc_pitch;

  // Mavi çizgi (Filtreli) ve Kırmızı çizgi (Ham) gösterit
  Serial.print(acc_pitch); // Titreyen (Ham Veri)
  Serial.print(","); 
  Serial.println(pitch);   // Kaymak Gibi (Filtreli Veri)
}