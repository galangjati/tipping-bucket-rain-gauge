//tipping bucket
#define nilaitip 0.135

//sensor raindrop
#define sensor D3
#define sensorgerimis A0  
#define batasgerimis1 700   
#define batasgerimis2 500
#define batasgerimis3 400
int gerimis;

//blynk&esp
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

//setting wifi
char auth[] = "bhnSoTw56gyaKwUM1zc6sJaJbXPV4u9_";
char ssid[] = "Redmi";
char pass[] = "galanggalang";

BlynkTimer timer;
WidgetRTC rtc;

//variabel waktu
volatile unsigned long contactTime;   
unsigned long intervalrain1h=3600000; 
unsigned long previousMillisrain1h=0; 
int tipCount1h, tipCount24h;
float rain1h, rain24h;
int ijam, imenit, idetik, ihari, ibulan, itahun;

void clockDisplay()
{
  String jam = String(hour());
  String menit = String(minute());
  String detik = String(second());
  String hari = String(day());
  String bulan = String(month());
  String tahun = String(year());

 ijam = jam.toInt();
 imenit= menit.toInt();
 idetik = detik.toInt();
 ihari = hari.toInt();
 ibulan = bulan.toInt();
 itahun = tahun.toInt();

  String currentTime = jam + ":" + menit + ":" + detik;
  String currentDate = hari + "/" + bulan + "/" + tahun;
  Blynk.virtualWrite(V1, currentTime);
  Blynk.virtualWrite(V2, currentDate);
}
 
BLYNK_CONNECTED() {
  rtc.begin();
 }

void ICACHE_RAM_ATTR get_rain() {   
    if((millis() - contactTime) > 20 ){  
      tipCount1h += 1;
      tipCount24h += 1;
      contactTime = millis();  
    }
}
    
void setup() {
  Serial.begin(9600);
  
  //memulai blynk
  Blynk.begin(auth, ssid, pass);

  //setting sensor photointerrupter
  pinMode(sensor, INPUT_PULLUP); 
  attachInterrupt(digitalPinToInterrupt(sensor), get_rain, FALLING);
  
  setSyncInterval(10 * 60);
  
 timer.setInterval(1000L, clockDisplay);
 timer.setInterval(1000L, MyFunction);
 }

void MyFunction() {
  if ((rain1h < 0.5) && (rain24h < 0.5)){
    gerimis = analogRead(sensorgerimis);
    if ((gerimis <= batasgerimis3)){
      rain1h = 0.5;
      rain24h = 0.5;
    }
    else if ((gerimis <= batasgerimis2)){
      rain1h = 0.3;
      rain24h = 0.3;
    }
    else if ((gerimis <= batasgerimis1)){
      rain1h = 0.15;
      rain24h = 0.15;
    }
  }

  else {
    rain1h = tipCount1h * nilaitip/2;    
    rain24h = tipCount24h * nilaitip/2;  
  }
 
  //restart tiap hujan selama 1 jam
  if ((unsigned long)(millis() - previousMillisrain1h) >= intervalrain1h){
    previousMillisrain1h = millis();
    tipCount1h = 0;
  }

  //restart harian   
  if ((ijam == 23) &&(imenit == 59)){
    tipCount1h = 0;
    tipCount24h = 0;
  }
  
  Blynk.virtualWrite(V0, String(rain1h,3));
  Blynk.virtualWrite(V3, String(rain24h,3));
  Blynk.virtualWrite(V4, gerimis);
}
  
void loop() {
 Blynk.run();
 timer.run();
}
