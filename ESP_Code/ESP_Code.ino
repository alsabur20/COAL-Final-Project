#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Firebase.h>

const char *ssid = "Sabur";         // Enter your Wi-Fi name
const char *password = "12131313";  // Enter Wi-Fi password

const char *mqttBroker = "test.mosquitto.org";
const int mqttPort = 1883;
const char *mqttClientId = "ESP32CLIENT";
const char *topic = "mqttData";
const char *modeTopic = "operatingMode";

bool manualModeActive = false;

const int BUFFER_SIZE = 50;  // Adjust the size based on your requirements
uint8_t sensorReadings[BUFFER_SIZE];
int readingsCount = 0;

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

void callback(char *topic, byte *payload, unsigned int length) {
  String modePayload = "";
  for (int i = 0; i < length; i++) {
    modePayload += (char)payload[i];
  }
  
  // Convert modePayload to a char to compare with single quotes
  char modeReceived = modePayload.charAt(0);

  // Handling mode changes
  switch (modeReceived) {
    case '0':
      Serial.println("Mode: Automatic");
      manualModeActive = false; // Automatic mode, deactivate manual mode flag
      Serial2.write('0'); // Sending '0' for automatic mode
      break;
    case '1':
      Serial.println("Mode: Manual");
      manualModeActive = true; // Manual mode activated
      Serial2.write('1'); // Sending '1' for manual mode
      break;
    case '2':
      if (manualModeActive) {
        Serial.println("Alarm On in Manual Mode");
        Serial2.write('2'); // Sending '2' for alarm on in manual mode
      } else {
        Serial.println("Can't set alarm on without manual mode active");
      }
      break;
    case '3':
      if (manualModeActive) {
        Serial.println("Alarm Off in Manual Mode");
        Serial2.write('3'); // Sending '3' for alarm off in manual mode
      } else {
        Serial.println("Can't set alarm off without manual mode active");
      }
      break;
    default:
      Serial.println("Invalid Mode");
      break;
  }
}

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  setupWifi();
  mqttClient.setServer(mqttBroker, mqttPort);
  mqttClient.setCallback(callback);
  mqttClient.subscribe(modeTopic);
}

void loop() {
  checkWifiConnection();

  if (Serial2.available() > 0 && readingsCount < BUFFER_SIZE) {
    sensorReadings[readingsCount] = Serial2.read();
    readingsCount++;
    Serial.print("Received value: ");
    Serial.println(sensorReadings[readingsCount - 1]);
  }

  if (mqttClient.connected() && readingsCount > 0) {
    publishReading();
  }

  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();
}

void publishReading() {
  char readingString[8];  // Adjust the size as needed
  snprintf(readingString, sizeof(readingString), "%d", sensorReadings[0]);
  mqttClient.publish(topic, readingString);
  // firebase.setInt("MQ2 Reading", sensorReadings[0]);

  // Shift all readings in the buffer to the left to remove the published value
  for (int i = 1; i < readingsCount; i++) {
    sensorReadings[i - 1] = sensorReadings[i];
  }
  readingsCount--;
}

void reconnectMQTT() {
  while (!mqttClient.connected()) {
    if (mqttClient.connect(mqttClientId)) {
      Serial.println("Connected to MQTT Broker");
      mqttClient.subscribe(modeTopic);
    } else {
      Serial.println("MQTT connection failed, retrying...");
      delay(2000);
    }
  }
}
