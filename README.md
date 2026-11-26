# ESP32-S3 Voice Recognition using Deepgram API

A real-time speech recognition project using the **ESP32-S3 Super Mini**, **INMP441 I2S microphone** and **Deepgram Nova-3 Speech-to-Text API**.

The project captures audio through an I2S microphone, sends the recorded audio to Deepgram over Wi-Fi and displays the recognized speech in the Arduino Serial Monitor.

The system is configured for **Indian English (`en-IN`)** recognition.

## Features

- ESP32-S3-based voice recognition.
- INMP441 digital I2S microphone.
- Cloud-based speech-to-text using Deepgram Nova-3.
- Indian English language configuration.
- 16 kHz, 16-bit, mono PCM audio.
- 5-second audio recording.
- Wi-Fi connectivity.
- HTTPS communication with Deepgram.
- PSRAM-based audio buffering.
- Audio signal diagnostics (Peak and RMS).
- JSON response parsing using ArduinoJson 7.
- Serial Monitor-controlled recording.

## Hardware Requirements

| Component | Description |
|---|---|
| Microcontroller | ESP32-S3 Super Mini |
| Microphone | INMP441 I2S MEMS microphone |
| Flash | 4 MB |
| PSRAM | 2 MB (board dependent) |
| Internet | Wi-Fi connection |
| API | Deepgram API key |

## Hardware Connections

### ESP32-S3 to INMP441

| INMP441 Pin | ESP32-S3 Pin |
|---|---|
| VDD | 3.3V |
| GND | GND |
| SCK / BCLK | GPIO 4 |
| WS / LRCLK | GPIO 5 |
| SD / DOUT | GPIO 6 |
| L/R | GND |

**Important:** The INMP441 microphone's acoustic port must remain unobstructed. Ensure that the microphone opening is exposed through the PCB mounting arrangement or enclosure.

A blocked microphone opening can significantly reduce the captured audio signal.

## Software Requirements

- Arduino IDE
- ESP32 Arduino Core 3.3.x
- ArduinoJson Library 7.x
- Deepgram API account

### Required Arduino Libraries

| Library | Purpose |
|---|---|
| WiFi.h | Wi-Fi connectivity |
| WiFiClientSecure.h | Secure HTTPS connection |
| HTTPClient.h | HTTP POST requests |
| ArduinoJson.h | JSON response parsing |
| ESP_I2S.h | I2S microphone interface |

The ESP32-specific libraries are included with the ESP32 Arduino Core.

## Deepgram API Configuration

This project uses the Deepgram Nova-3 speech recognition model.

Create an API key through:

https://console.deepgram.com/

Configure the following values in the Arduino sketch:

```cpp
const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

const char* DEEPGRAM_KEY = "YOUR_DEEPGRAM_API_KEY";
```

### Deepgram API Parameters

| Parameter | Configuration |
|---|---|
| Model | Nova-3 |
| Language | en-IN |
| Sample Rate | 16000 Hz |
| Encoding | Linear16 |
| Channels | 1 |
| Smart Formatting | Enabled |

API endpoint:

```text
https://api.deepgram.com/v1/listen
```

## Audio Configuration

| Setting | Value |
|---|---|
| Audio interface | I2S Standard |
| Sampling frequency | 16 kHz |
| Input format | 32-bit I2S microphone data |
| Processing format | 16-bit PCM |
| Audio channels | Mono |
| Recording duration | 5 seconds |
| Audio buffer | 160 KB |

The INMP441 provides digital audio through its I2S interface. The ESP32-S3 captures the microphone data and transforms it into 16-bit PCM samples for transmission.

## Working Principle

```text
       INMP441 MICROPHONE
               |
               | I2S Audio
               |
          ESP32-S3
               |
        Audio Processing
               |
        16-bit PCM Data
               |
          Wi-Fi Network
               |
        HTTPS POST Request
               |
        DEEPGRAM NOVA-3
               |
       Speech Recognition
               |
         JSON Response
               |
        ArduinoJson Parser
               |
       Recognized Text
               |
        SERIAL MONITOR
```

## Installation and Setup

1. Install Arduino IDE.
2. Install the ESP32 Arduino Board Package.
3. Select the ESP32S3 Dev Module.
4. Install ArduinoJson version 7.x.
5. Connect the INMP441 microphone according to the wiring table.
6. Open the Arduino sketch.
7. Enter your Wi-Fi credentials.
8. Enter your Deepgram API key.
9. Upload the program to the ESP32-S3.
10. Open Serial Monitor at 115200 baud.
11. Enter `R` to start recording.

The ESP32-S3 records audio for five seconds, uploads it to Deepgram and prints the recognized sentence.

## Example Serial Monitor Output

```text
ESP32-S3 INDIAN ENGLISH VOICE RECOGNITION

Connecting to WiFi...
WiFi connected!

Initializing INMP441...
INMP441 ready!

Ready!
Press R to record.

Speak now...
Peak: 16191 | RMS: 2679.63

What is the name now?

Ready. Press R to record again.
```

## Audio Diagnostics

The firmware calculates two audio measurements:

- **Peak Level:** Maximum absolute audio sample value.
- **RMS Level:** Root Mean Square amplitude of the recorded audio.

These measurements help identify microphone signal problems, incorrect wiring and insufficient audio input.

## Troubleshooting

| Problem | Possible Solution |
|---|---|
| I2S initialization failed | Check microphone wiring and GPIO configuration |
| Very low Peak and RMS | Check the microphone acoustic opening and connections |
| No speech recognized | Speak clearly and check microphone input |
| HTTP error | Check Wi-Fi connectivity and API key |
| JSON parsing failed | Check ArduinoJson version and nesting limit |
| Wi-Fi connection failed | Verify SSID and password |
| Audio buffer allocation failed | Check PSRAM configuration and available memory |

## Security Notes

- Never upload your personal Deepgram API key to a public GitHub repository.
- Keep Wi-Fi credentials out of publicly shared source code.
- Use a private configuration file or environment-specific settings for credentials.
- The current demonstration sketch uses `WiFiClientSecure::setInsecure()`, which disables TLS certificate verification. Use proper certificate validation for production deployments.

## Future Improvements

- OLED display for recognized text.
- Gemini API integration for AI-powered responses.
- Continuous audio streaming using WebSockets.
- Wake-word detection.
- Tamil and Hindi speech recognition.
- Voice activity detection.
- Text-to-speech output.
- Portable battery-powered voice assistant.

## Project Status

**Working prototype**

Successfully tested:

- ESP32-S3 I2S microphone capture.
- INMP441 audio acquisition.
- Wi-Fi connectivity.
- Deepgram Nova-3 API communication.
- Indian English transcription configuration.
- JSON response parsing.
- Recognized text output through Serial Monitor.

## License

This project is intended for educational, experimental and personal development purposes.

Choose an appropriate open-source license before publishing the repository, such as the MIT License.

---

**Developed using ESP32-S3, INMP441 and Deepgram Nova-3.**
