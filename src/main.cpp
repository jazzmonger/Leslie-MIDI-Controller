#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

static constexpr uint8_t kMidiRxPin = D1;           // GPIO5
static constexpr uint8_t kTriggerPin = D4;          // GPIO2
static constexpr uint8_t kSpeedIndicatorPin = D5;   // GPIO14 — high=fast, low=slow
static constexpr uint8_t kTriggerCc = 87;
static constexpr uint8_t kSpeedCc = 108;
static constexpr uint32_t kPulseMs = 250;

static SoftwareSerial MidiSerial(kMidiRxPin, -1, false);

static uint8_t runningStatus = 0;
static uint8_t dataBytes[2];
static uint8_t dataIndex = 0;
static uint32_t pulseEndMs = 0;

static uint8_t expectedDataBytes(uint8_t status) {
  switch (status & 0xF0) {
    case 0xC0:
    case 0xD0:
      return 1;
    case 0x80:
    case 0x90:
    case 0xA0:
    case 0xB0:
    case 0xE0:
      return 2;
    default:
      return 0;
  }
}

static void releaseTriggerPin() {
  pinMode(kTriggerPin, INPUT_PULLUP);
}

static void startPulse() {
  pinMode(kTriggerPin, OUTPUT);
  digitalWrite(kTriggerPin, LOW);
  pulseEndMs = millis() + kPulseMs;
}

static void updatePulse() {
  if (pulseEndMs != 0 && millis() >= pulseEndMs) {
    pulseEndMs = 0;
    releaseTriggerPin();
  }
}

static bool shouldPulseForCc87() {
  return digitalRead(kSpeedIndicatorPin) == LOW;
}

static bool isCc108Fast(uint8_t value) {
  return value == 85 || value == 127;
}

static bool isCc108Slow(uint8_t value) {
  return value == 0 || value == 43;
}

static bool shouldPulseForCc108(uint8_t cc108Value) {
  const bool isFast = digitalRead(kSpeedIndicatorPin) == HIGH;

  if (isCc108Fast(cc108Value)) {
    return !isFast;
  }
  if (isCc108Slow(cc108Value)) {
    return isFast;
  }
  return false;
}

static void handleControlChange(uint8_t status, uint8_t cc, uint8_t value) {
  if (status != 0xB0) {
    return;
  }

  if (cc == kTriggerCc) {
    if (value != 127 || !shouldPulseForCc87()) {
      return;
    }
    startPulse();
    return;
  }

  if (cc == kSpeedCc && shouldPulseForCc108(value)) {
    startPulse();
  }
}

static void handleMidiByte(uint8_t byte) {
  if (byte >= 0xF8) {
    return;
  }

  if (byte & 0x80) {
    runningStatus = byte;
    dataIndex = 0;
    if ((byte & 0xF0) == 0xF0) {
      runningStatus = 0;
    }
    return;
  }

  if (runningStatus == 0) {
    return;
  }

  const uint8_t needed = expectedDataBytes(runningStatus);
  if (needed == 0) {
    return;
  }

  dataBytes[dataIndex++] = byte;
  if (dataIndex < needed) {
    return;
  }

  handleControlChange(runningStatus, dataBytes[0], dataBytes[1]);
  dataIndex = 0;
}

void setup() {
  WiFi.mode(WIFI_OFF);

  pinMode(kSpeedIndicatorPin, INPUT);
  releaseTriggerPin();

  MidiSerial.begin(31250, SWSERIAL_8N1, kMidiRxPin, -1, false, 256, 1024);
}

void loop() {
  while (MidiSerial.available()) {
    handleMidiByte(MidiSerial.read());
  }
  updatePulse();
}
