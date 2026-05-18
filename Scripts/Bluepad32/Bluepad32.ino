#include <Bluepad32.h>

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

// Bağlandığı zaman
void onConnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            Serial.println(">>> SAHANE! XBOX KOLU BAGLANDI! <<<");
            myControllers[i] = ctl;
            break;
        }
    }
}

// Eğer kol koparsa
void onDisconnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            Serial.println("kol yok");
            myControllers[i] = nullptr;
            break;
        }
    }
}

void setup() {
    Serial.begin(115200);
    
    // Bluetooth sistemine, kol bağlanırsa veya koparsa hangi işlemleri yapacağını söylüyoruz
    BP32.setup(&onConnectedController, &onDisconnectedController);
    
    // Güvenlik için
    BP32.forgetBluetoothKeys();
    
    Serial.println("Drone Hazir! Kolu eşleştirin");
}

void loop() {
    BP32.update();

    // kolu kontrol et
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        ControllerPtr myController = myControllers[i];

        // Eğer kol bağlıysa verileri oku
        if (myController && myController->isConnected()) {
            
            // Joystick verilerini okuyoruz (-511 ile 511 arası değer verir)
            int sol_y = myController->axisY();  // Sol çubuk (İleri-Geri / Pitch)
            int sol_x = myController->axisX();  // Sol çubuk (Sağa-Sola / Roll)
            
            int sag_y = myController->axisRY(); // Sağ çubuk (Gaz)
            int sag_x = myController->axisRX(); // Sağ çubuk (Kendi etrafında dön)

            // Okunan değerleri ekrana yazdır 
            Serial.printf("Gaz: %4d | Ileri/Geri: %4d | Saga/Sola: %4d | Donme: %4d\n", sag_y, sol_y, sol_x, sag_x);
        }
    }
    
    // Yazılar ekranda çok hızlı akmasın diye çok kısa bekleme
    delay(100); 
}