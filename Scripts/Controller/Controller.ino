//Bu kodun yazılmasının asıl sebebi şudur: Kontrolcüden alınan sinyaller bir makina diliyle yazılır. Crosssfire kullanımı ise bu dili çevirir.


#include<AlfredoCRSF.h>

HardwareSerial crsfSerial(1); //Sinyallerin ekstra bir hat üzerinden geçebilmesi için yeni bir ağ oluştırduk.

AlfredoCRSF crsf;

void setup(){
  Serial.begin(115200); //1.Kanal bilgisyaraın ekranına veri gönderme hızı

  crsfSerial.begin(420000, SERIAL_8N1, D7, D6); //RX den bilgi aktarılması için 2. bir kanal oluşturduk ve pinleri bunun içinde tanımladık
  crsf.begin(crsfSerial);  //crossfire kodunu başlattık
}

  void loop(){
    crsf.update(); //sürekli yeni sinyal geldi mi diyw kontrol eder ve her koda olmak zorundadır


    //CRSF kodu açtı ve şimdi ise kumandadan gelen 4 temel hareketi almak zorunda
    int ch1=crsf.getChannel(1);  // Roll(Sağ Sol)
    int ch2=crsf.getChannel(2);  // Pitch(İleri Geri)
    int ch3=crsf.getChannel(3);  // Throttle(gaz)
    int ch4=crsf.getChannel(4);  // Yaw(Kendi ekseninde)

    // Değerlerin okunması adına ekrana yansıtmak için şu kodlar kullanılır
    Serial.print("CH1 : "); Serial.print(ch1);
    Serial.print(" | CH2: "); Serial.print(ch2);
    Serial.print(" | CH3: "); Serial.print(ch3);
    Serial.print(" | CH4: "); Serial.println(ch4);

    delay(50); //veriler daha yavaş gelsin
  }