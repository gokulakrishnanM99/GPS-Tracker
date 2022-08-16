#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>

/* ##############  Blynk Connection Setup  ############## */
char auth[] = ""; // Blynk authentication key
char ssid[] = ""; // Name of your WiFi SSID
char pass[] = ""; // WiFi Password
long a;

/*  virtual pins:
  # V0: MAP
  # V1: LCD screen in Advanced
*/
// Blynk LCD and map widgets
WidgetLCD lcd(V1);
WidgetMap myMap(V2);
String GPSLabel = "BLYNK"; //Labeling location on MAP

/* ##############  Timer  ############## */
SimpleTimer timer;

/* ##############  GPS PIN Setup  ############## */
static const int RXPin = 4, TXPin = 5;   // GPIO 4=D2(conneect Tx of GPS) and GPIO 5=D1(Connect Rx of GPS)
static const uint32_t GPSBaud = 9600; //if Baud rate 9600 didn't work in your case then use 4800

TinyGPSPlus gps;                             // The TinyGPS++ object
SoftwareSerial ss(RXPin, TXPin);     // Serial connection to the GPS module

/* ############################
   Starting Program
   ############################ */

void setup() {
  Serial.begin(9600);       // serial connection for debugging
  ss.begin(GPSBaud);
  pinMode(A0,INPUT);
  //Connect Blynk
  Blynk.begin(auth, ssid, pass);
  Serial.println("Activating GPS");
  timer.setInterval(1000L, periodicUpdate);
  timer.setInterval(60*1000, reconnectBlynk);
}

//Show GPS lat and lng on LCD
void periodicUpdate() {
  String line1, line2;
  //LCD
  lcd.clear();
  a=analogRead(A0);
  Serial.println(a);
  delay(2000);
   Blynk.virtualWrite(V8,a);
   if(a>1000){
  if (gps.location.isValid() && (gps.location.age() < 3000)) {
    //position current
    line1 = String("lat: ") + String(gps.location.lat(), 6);
    line2 = String("lng: ") + String(gps.location.lng(), 6);
    lcd.print(0, 0, line1);
    lcd.print(0, 1, line2);
    //update location on map
    myMap.location(2, gps.location.lat(), gps.location.lng(), GPSLabel);
  } else {
    //position is lost
    lcd.print(0, 0, "GPS lost");
  }
   }
}

void updateGPS() {
  //read data from GPS module
  while (ss.available() > 0) {
    gps.encode(ss.read());
  }
}

void reconnectBlynk() {
  if (!Blynk.connected()) {
    Serial.println("Lost connection");
    if(Blynk.connect()) Serial.println("Reconnected");
    else Serial.println("Not reconnected");
  }
}

void loop() {
  timer.run();
  if(Blynk.connected()) { Blynk.run(); }
  updateGPS();
}
