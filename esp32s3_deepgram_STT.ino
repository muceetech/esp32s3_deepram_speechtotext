
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <math.h>
#include "ESP_I2S.h"

// ==========================================
// WIFI AND DEEPGRAM CONFIGURATION
// ==========================================

const char* WIFI_SSID = "";
const char* WIFI_PASSWORD = "";

const char* DEEPGRAM_KEY = "";

// Indian English - Nova-3
const char* DEEPGRAM_URL =
  "https://api.deepgram.com/v1/listen"
  "?model=nova-3"
  "&language=en-IN"
  "&smart_format=true"
  "&encoding=linear16"
  "&sample_rate=16000"
  "&channels=1";

// ==========================================
// INMP441 PIN CONFIGURATION
// ==========================================

#define I2S_BCLK 4
#define I2S_WS   5
#define I2S_DIN  6

// ==========================================
// AUDIO CONFIGURATION
// ==========================================

#define SAMPLE_RATE 16000
#define RECORD_SECONDS 5

#define AUDIO_SAMPLES (SAMPLE_RATE * RECORD_SECONDS)
#define AUDIO_BYTES (AUDIO_SAMPLES * sizeof(int16_t))

// ==========================================
// GLOBAL OBJECTS
// ==========================================

I2SClass i2s;

int16_t* audioBuffer = nullptr;

// ==========================================
// WIFI CONNECTION
// ==========================================

void connectWiFi() {

  Serial.print("Connecting to WiFi");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
}

// ==========================================
// MICROPHONE INITIALIZATION
// ==========================================

bool initializeMicrophone() {

  Serial.println("Initializing INMP441...");

  i2s.setPins(
    I2S_BCLK,
    I2S_WS,
    -1,
    I2S_DIN,
    -1
  );

  bool result = i2s.begin(
    I2S_MODE_STD,
    SAMPLE_RATE,
    I2S_DATA_BIT_WIDTH_32BIT,
    I2S_SLOT_MODE_MONO,
    I2S_STD_SLOT_LEFT
  );

  if (!result) {
    Serial.println("I2S initialization failed!");
    return false;
  }

  i2s.configureRX(
    SAMPLE_RATE,
    I2S_DATA_BIT_WIDTH_32BIT,
    I2S_SLOT_MODE_MONO,
    I2S_RX_TRANSFORM_32_TO_16
  );

  Serial.println("INMP441 ready!");

  return true;
}

// ==========================================
// RECORD AUDIO
// ==========================================

bool recordAudio() {

  Serial.println("\nSpeak now...");

  memset(audioBuffer, 0, AUDIO_BYTES);

  size_t totalRead = 0;

  while (totalRead < AUDIO_BYTES) {

    size_t remaining = AUDIO_BYTES - totalRead;

    size_t chunk = i2s.readBytes(
      (char*)audioBuffer + totalRead,
      remaining
    );

    if (chunk == 0) {
      delay(1);
      continue;
    }

    totalRead += chunk;
  }

  // Audio diagnostics

  int32_t peak = 0;
  double sumSquares = 0;

  for (int i = 0; i < AUDIO_SAMPLES; i++) {

    int32_t sample = audioBuffer[i];

    int32_t magnitude = abs(sample);

    if (magnitude > peak) {
      peak = magnitude;
    }

    sumSquares += (double)sample * sample;
  }

  float rms = sqrt(sumSquares / AUDIO_SAMPLES);

  Serial.print("Peak: ");
  Serial.print(peak);

  Serial.print(" | RMS: ");
  Serial.println(rms);

  return true;
}

// ==========================================
// SEND AUDIO TO DEEPGRAM
// ==========================================

void sendToDeepgram() {

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected!");
    return;
  }

  WiFiClientSecure client;

  // Testing only. Validate the server certificate
  // before using this in a production device.
  client.setInsecure();

  HTTPClient https;

  https.setTimeout(30000);

  if (!https.begin(client, DEEPGRAM_URL)) {
    Serial.println("Connection failed!");
    return;
  }

  https.addHeader(
    "Authorization",
    String("Token ") + DEEPGRAM_KEY
  );

  https.addHeader(
    "Content-Type",
    "audio/l16"
  );

  int httpCode = https.POST(
    (uint8_t*)audioBuffer,
    AUDIO_BYTES
  );

  if (httpCode == 200) {

    String payload = https.getString();

    // ======================================
    // ARDUINOJSON 7 PARSING
    // ======================================

    JsonDocument doc;

    DeserializationError error = deserializeJson(
      doc,
      payload,
      DeserializationOption::NestingLimit(20)
    );

    if (error) {

      Serial.print("JSON error: ");
      Serial.println(error.c_str());

    } else {

      const char* transcript =
        doc["results"]["channels"][0]
           ["alternatives"][0]["transcript"] | "";

      // Print only recognized text

      if (strlen(transcript) > 0) {
        Serial.println();
        Serial.println(transcript);
        Serial.println();
      } else {
        Serial.println("No speech recognized.");
      }
    }

  } else {

    Serial.print("Deepgram HTTP error: ");
    Serial.println(httpCode);

    Serial.println(https.getString());
  }

  https.end();
}

// ==========================================
// SETUP
// ==========================================

void setup() {

  Serial.begin(115200);

  delay(1500);

  Serial.println("\nESP32-S3 INDIAN ENGLISH VOICE RECOGNITION");

  // Allocate audio buffer

  if (psramFound()) {
    audioBuffer = (int16_t*)ps_malloc(AUDIO_BYTES);
  } else {
    audioBuffer = (int16_t*)malloc(AUDIO_BYTES);
  }

  if (audioBuffer == nullptr) {

    Serial.println("Audio memory allocation failed!");

    while (true) {
      delay(1000);
    }
  }

  connectWiFi();

  if (!initializeMicrophone()) {

    Serial.println("Microphone initialization failed!");

    while (true) {
      delay(1000);
    }
  }

  Serial.println("\nReady!");
  Serial.println("Press R to record.");
}

// ==========================================
// MAIN LOOP
// ==========================================

void loop() {

  if (Serial.available()) {

    char command = Serial.read();

    if (command == 'R' || command == 'r') {

      if (recordAudio()) {
        sendToDeepgram();
      }

      Serial.println("Ready. Press R to record again.");
    }
  }

  delay(10);
}
