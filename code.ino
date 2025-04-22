#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

// WiFi credentials
const char* ssid = "your_ssid_name";
const char* password = "your_ssid_password";

// API URLs
const char* insertRFIDURL = "https://yourdomain.in/college/insert_rfid_data.php";
const char* getRelayStatusURL = "https://yourdomain.in/college/get_relay_status.php";

// Safe GPIOs for relays (avoiding SPI pins 18, 19, etc.)
const int relayPins[] = {13, 12, 27, 26}; // Relay 1 → GPIO13, Relay 2 → GPIO12, etc.
bool relayStates[] = {HIGH, HIGH, HIGH, HIGH}; // Active LOW

// Buzzer
const int buzzer = 14;

// EEPROM Settings
#define EEPROM_SIZE 64
#define EEPROM_FLAG_ADDR 0
#define EEPROM_DATA_ADDR 1
#define MAX_RFID_LEN 32

// Timing
unsigned long lastRFIDTime = 0;
const unsigned long rfidCooldown = 2000;
unsigned long lastRelayCheck = 0;
const unsigned long relayCheckInterval = 3000;

String lastScannedTag = "";

void setup() {
  Serial.begin(115200);
  delay(1000); // Ensure serial is initialized

  // Initialize Serial2 for EM-18 (RX on GPIO 16)
  Serial2.begin(9600, SERIAL_8N1, 16, -1); // RX on GPIO 16, TX not used

  pinMode(buzzer, OUTPUT);  // Buzzer setup
  digitalWrite(buzzer, LOW);  // Initially off

  // Relay setup
  for (int i = 0; i < 4; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], HIGH); // Relay OFF (active LOW)
  }

  EEPROM.begin(EEPROM_SIZE); // Initialize EEPROM for ESP32
  connectWiFi();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  checkRFID();
  fetchRelayStatus();
}

void connectWiFi() {
  Serial.println("[INFO] Connecting to WiFi: " + String(ssid));
  WiFi.begin(ssid, password);
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 30) {
    delay(500);
    Serial.print(".");
    tries++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("[SUCCESS] WiFi connected! IP: " + WiFi.localIP().toString());
  } else {
    Serial.println("[ERROR] Failed to connect to WiFi.");
  }
}

void checkRFID() {
  if (millis() - lastRFIDTime < rfidCooldown) return;

  // Check if data is available from EM-18
  if (Serial2.available()) {
    String rfidTag = "";
    unsigned long startTime = millis();
    bool tagComplete = false;

    // Read until newline or timeout (100ms)
    while (millis() - startTime < 100) {
      if (Serial2.available()) {
        char c = Serial2.read();
        if (c == '\n' || c == '\r') {
          tagComplete = true;
          break;
        }
        rfidTag += c;
      }
    }

    // Clear any remaining buffer data
    while (Serial2.available()) {
      Serial2.read();
    }

    // Debug: Print raw tag and length
    Serial.println("[DEBUG] Raw RFID Tag: \"" + rfidTag + "\", Length: " + String(rfidTag.length()));

    // Validate tag: exactly 12 characters
    if (rfidTag.length() == 12) {
      Serial.println("[INFO] Valid RFID Tag Detected: " + rfidTag);
      lastRFIDTime = millis();
      lastScannedTag = rfidTag;

      bool sent = sendRFIDToServer(rfidTag);
      if (sent) {
        Serial.println("[SUCCESS] RFID logged. Beep success.");
        beepSuccess();
      } else {
        Serial.println("[WARN] Server unreachable. Storing tag to EEPROM...");
        storeUnsentRFID(rfidTag);
        beepError();
      }
    } else {
      Serial.println("[ERROR] Invalid RFID tag: \"" + rfidTag + "\" (Length: " + rfidTag.length() + ")");
    }
  }
}

bool sendRFIDToServer(String tag) {
  if (WiFi.status() != WL_CONNECTED) return false;

  WiFiClientSecure client;
  client.setInsecure();  // Disable SSL verification (for testing)

  HTTPClient http;
  http.begin(client, insertRFIDURL);  // Server URL
  http.addHeader("Content-Type", "application/json");

  String jsonPayload = "{\"rfid_tag\":\"" + tag + "\"}";
  int responseCode = http.POST(jsonPayload);
  String response = http.getString();
  http.end();

  Serial.println("[INFO] Server Response Code: " + String(responseCode));
  Serial.println("[INFO] Response: " + response);

  return (responseCode == 200 && response.indexOf("\"success\"") >= 0);
}

void fetchRelayStatus() {
  if (millis() - lastRelayCheck < relayCheckInterval) return;
  lastRelayCheck = millis();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WARN] WiFi not connected. Skipping relay check.");
    return;
  }

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  http.begin(client, getRelayStatusURL);
  int responseCode = http.GET();

  if (responseCode == 200) {
    String response = http.getString();
    Serial.println("[INFO] Relay API Response: " + response);

    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, response);

    if (error) {
      Serial.println("[ERROR] Failed to parse JSON.");
      return;
    }

    if (doc["status"] == "success") {
      JsonObject relays = doc["relays"];
      for (int i = 0; i < 4; i++) {
        String key = String(i + 1);
        if (relays.containsKey(key)) {
          int newState = relays[key] == 1 ? HIGH : LOW; // Active LOW logic
          if (relayStates[i] != newState) {
            relayStates[i] = newState;
            digitalWrite(relayPins[i], newState);
            Serial.println("[INFO] Relay " + String(i + 1) + " → " + (newState == LOW ? "ON" : "OFF"));
          }
        }
      }
    }
  } else {
    Serial.println("[ERROR] Relay GET failed. Code: " + String(responseCode));
  }

  http.end();
  Serial.println("[DEBUG] Free heap after relay check: " + String(ESP.getFreeHeap()));
}

void beepSuccess() {
  digitalWrite(buzzer, HIGH);
  delay(300);
  digitalWrite(buzzer, LOW);
}

void beepError() {
  for (int i = 0; i < 2; i++) {
    digitalWrite(buzzer, HIGH);
    delay(200);
    digitalWrite(buzzer, LOW);
    delay(100);
  }
}

void storeUnsentRFID(String tag) {
  EEPROM.write(EEPROM_FLAG_ADDR, 0x01);
  for (int i = 0; i < MAX_RFID_LEN; i++) {
    char c = (i < tag.length()) ? tag[i] : 0;
    EEPROM.write(EEPROM_DATA_ADDR + i, c);
  }
  EEPROM.commit();
}

String readUnsentRFID() {
  if (EEPROM.read(EEPROM_FLAG_ADDR) != 0x01) return "";

  String tag = "";
  for (int i = 0; i < MAX_RFID_LEN; i++) {
    char c = EEPROM.read(EEPROM_DATA_ADDR + i);
    if (c == 0) break;
    tag += c;
  }
  return tag;
}

void clearUnsentRFID() {
  EEPROM.write(EEPROM_FLAG_ADDR, 0x00);
  for (int i = 0; i < MAX_RFID_LEN; i++) {
    EEPROM.write(EEPROM_DATA_ADDR + i, 0);
  }
  EEPROM.commit();
}
