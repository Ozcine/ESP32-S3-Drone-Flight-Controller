#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <math.h>


#define I2C_SDA 5  // Karttaki D4 pini
#define I2C_SCL 6  // Karttaki D5 pini

float pitch_offset = 3.95;  
float roll_offset = -0.7;  

Adafruit_MPU6050 mpu;

void setup(void) {
  Serial.begin(115200);
  
  // ESP için I2C pinlerini başlatıyoruz
  Wire.begin(I2C_SDA, I2C_SCL);

  if (!mpu.begin()) {
    Serial.println("Sensör yok! Kabloları (D4 ve D5) kontrol et.");
    while (1) yield();
  }

  // Standart Drone Ayarları
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);  // Drone standartları
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);       // Saniyede 500 derece kadar dönüşleri ölçülebilir
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);    // Gürültü önleyici sadece ana hareketlere odaklanır
}

void loop() {
  // Gyro sensörü hızı denetleyecek
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Ham Açıyı Hesapla
  float raw_roll = atan2(a.acceleration.y, a.acceleration.z) * 180.0 / PI;
  float raw_pitch = atan2(-a.acceleration.x, a.acceleration.z) * 180.0 / PI; 

  // Kalibrasyon
  float calib_roll = raw_roll - roll_offset;
  float calib_pitch = raw_pitch - pitch_offset; 

  Serial.print("Pitch (One-Arkaya): ");
  Serial.print(calib_pitch);
  Serial.print("\t | Roll (Saga-Sola): "); 
  Serial.println(calib_roll);

  delay(100);
}