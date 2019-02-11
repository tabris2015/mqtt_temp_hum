#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include "param.h"

WiFiClient espClient;
PubSubClient client(espClient);

DHT dht(dht_dpin, DHTTYPE); 

long lastMsg = 0;
float temp = 0.0;
float hum = 0.0;
float diff = 1.0;

// prototypes

void init_wifi();
void reconnect();
bool checkBound(float newValue, float prevValue, float maxDiff);
void callback(char* topic, byte* payload, unsigned int length);
//

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(OUT_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);


  digitalWrite(LED_BUILTIN, 1);
  digitalWrite(STATUS_LED_PIN, 1);
  digitalWrite(OUT_PIN, 0);

  Serial.begin(9600);
  delay(10);
  Serial.println('\n');

  init_wifi();
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float newTemp = dht.readTemperature();
  float newHum = dht.readHumidity();

  if (checkBound(newTemp, temp, diff))
  {
    temp = newTemp;
    Serial.print("New temperature:");
    Serial.println(String(temp).c_str());
    client.publish(TEMP_TOPIC, String(temp).c_str(), true);
  }
  if (checkBound(newHum, hum, diff)) {
    hum = newHum;
    Serial.print("New humidity:");
    Serial.println(String(hum).c_str());
    client.publish(HUM_TOPIC, String(hum).c_str(), true);
  }
  delay(1000);
  
}


// function bodies
void init_wifi()
{
  WiFi.begin(SSID, PASS);
  Serial.print("Conectando a ");
  Serial.print(SSID); 
  Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer

  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);
  client.subscribe(LED_TOPIC);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    digitalWrite(STATUS_LED_PIN, 1);

    dht.begin();
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // If you do not want to use a username and password, change next line to
    // if (client.connect("ESP8266Client")) {
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      client.subscribe(LED_TOPIC);
      digitalWrite(STATUS_LED_PIN, 0);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  
}

bool checkBound(float newValue, float prevValue, float maxDiff) {
  return !isnan(newValue) &&
         (newValue < prevValue - maxDiff || newValue > prevValue + maxDiff);
}

// message callback

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(OUT_PIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(OUT_PIN, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}