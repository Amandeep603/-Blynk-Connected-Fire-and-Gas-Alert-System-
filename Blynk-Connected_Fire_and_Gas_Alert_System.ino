#define BLYNK_TEMPLATE_ID "TMPL3AggV-M74"
#define BLYNK_TEMPLATE_NAME "FireAlarm"
#define BLYNK_AUTH_TOKEN "blbKXRTa2aUxAolaYd-a4QhjCUgRgIIK"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

int TEMP_THRESHOLD = 30;
int GAS_THRESHOLD = 400;
const char* ssid = "Test";
const char* password = "Test123456";
char auth[] = BLYNK_AUTH_TOKEN;

#define DHTPIN 4 // D2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define MQ5_PIN A0
#define RED_LED_PIN 15 // D1 for overheat indication
#define GREEN_LED_PIN 14 // D3 for gas detection indication
#define BUZZER_PIN 2 // D4 for buzzer

float temperature = 0;
float humidity = 0;
int gasLevel = 0;

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, password);
  dht.begin();
  
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
}

void loop() {
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  gasLevel = analogRead(MQ5_PIN);

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);
  Blynk.virtualWrite(V2, gasLevel);

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C | Humidity: ");
  Serial.print(humidity);
  Serial.print(" % | Gas Level: ");
  Serial.println(gasLevel);

  bool alert = false; // Flag to indicate when to activate the buzzer

  if (temperature > TEMP_THRESHOLD) {
    Blynk.logEvent("hightemperature");
    Serial.println("Over-temperature detected!");
    digitalWrite(RED_LED_PIN, HIGH); // Turn on red LED
    alert = true; // Set alert flag
  } else {
    digitalWrite(RED_LED_PIN, LOW); // Turn off red LED if no overheat
  }

  if (gasLevel > GAS_THRESHOLD) {
    Blynk.logEvent("gasleakage");
    Serial.println("LPG leakage detected!");
    digitalWrite(GREEN_LED_PIN, HIGH); // Turn on green LED
    alert = true; // Set alert flag
  } else {
    digitalWrite(GREEN_LED_PIN, LOW); // Turn off green LED if no gas leak
  }

  if (alert) {
    digitalWrite(BUZZER_PIN, HIGH); // Turn on buzzer if there's an alert
  } else {
    digitalWrite(BUZZER_PIN, LOW); // Turn off buzzer if no alert
  }

  delay(500);
}
