#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Firebase.h>

const char *ssid = "Sabur";         // Enter your Wi-Fi name
const char *password = "12131311";  // Enter Wi-Fi password

const char *mqttBroker = "test.mosquitto.org";
const int mqttPort = 1883;
const char *mqttClientId = "ESP32CLIENT";
const char *topic = "mqttData";

#define firebaseHost "https://mq2-sensor-logs-default-rtdb.asia-southeast1.firebasedatabase.app/"

WiFiClient espClient;
PubSubClient mqttClient(espClient);

Firebase firebase(firebaseHost);

void setupWifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the Wi-Fi network");
}
void checkWifiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost. Reconnecting...");
    setupWifi();
  }
}
void setup() {
  Serial.begin(9600);
  setupWifi();
  mqttClient.setServer(mqttBroker, mqttPort);
}
void loop() {
  checkWifiConnection();

  if (Serial.available() > 0) {
    String sensorReading = Serial.readStringUntil('\n');
    sensorReading.trim();  // Remove leading/trailing whitespaces
    Serial.print("Received value: ");
    Serial.println(sensorReading);

    if (mqttClient.connected()) {
      mqttClient.publish(topic, sensorReading.c_str());
    }

    firebase.setString("MQ2 Reading", sensorReading);
  }

  // Maintain MQTT connection
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();
}


void reconnectMQTT() {
  while (!mqttClient.connected()) {
    if (mqttClient.connect(mqttClientId)) {
      Serial.println("Connected to MQTT Broker");
    } else {
      Serial.println("MQTT connection failed, retrying...");
      delay(2000);
    }
  }
}
