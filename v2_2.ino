/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL6ZHzz3SP9"
#define BLYNK_TEMPLATE_NAME "nodemcu"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

#define DHTTYPE DHT11  // DHT 11
DHT dht(D2, DHTTYPE);

char auth[] = "j5nHrhG70_5kewiYg9O-SJHUIxlJgZ1H";  //Enter your Auth token
char ssid[] = "OPPO A16";                      //Enter your WIFI name
char pass[] = "Mika3210";                    //Enter your WIFI password

BlynkTimer timer;
bool Relay = false;

#define MOISTURE_THRESHOLD_LOW  50   // Moisture threshold for watering
#define MOISTURE_THRESHOLD_HIGH 70   // Moisture threshold for stopping watering
#define SENSOR_PIN A0
#define WATER_PUMP_PIN D3

void setup() {
  Serial.begin(9600);
  pinMode(WATER_PUMP_PIN, OUTPUT);
  digitalWrite(WATER_PUMP_PIN, HIGH);

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  timer.setInterval(1000L, checkSoilMoisture);
}

BLYNK_WRITE(V1) {
  Relay = param.asInt();

  if (Relay == 1) {
    digitalWrite(WATER_PUMP_PIN, LOW);
  } else {
    digitalWrite(WATER_PUMP_PIN, HIGH);
  }
}

void checkSoilMoisture() {
  int soilMoisture = analogRead(SENSOR_PIN);
  soilMoisture = map(soilMoisture, 0, 1024, 0, 100);
  soilMoisture = (soilMoisture - 100) * -1;

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  Blynk.virtualWrite(V0, soilMoisture);
  Blynk.virtualWrite(V3, humidity);
  Blynk.virtualWrite(V4, temperature);

  if (soilMoisture < MOISTURE_THRESHOLD_LOW) {
    digitalWrite(WATER_PUMP_PIN, LOW);
    Blynk.virtualWrite(V2, "Soil is dry. Watering...");
  } else if (soilMoisture > MOISTURE_THRESHOLD_HIGH) {
    digitalWrite(WATER_PUMP_PIN, HIGH);
    Blynk.virtualWrite(V2, "Soil is wet enough. Not watering.");
  } else {
    digitalWrite(WATER_PUMP_PIN, LOW);
    Blynk.virtualWrite(V2, "Soil moisture is optimal. Not watering.");
  }
}

void loop() {
  Blynk.run();
  timer.run();
}
