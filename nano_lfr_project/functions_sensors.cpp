#include <Arduino.h>
#include "config.h"
#include "functions.h"

dataSetting setting;
uint16_t reading[16] = {0};
bool previouslyCalibrated = false;
uint16_t sensorBits = 0;

uint16_t preDefinedThreshold[16] = {
  71, 71, 67, 67,
  27, 80, 80, 80,
  70, 70,70 , 400,
  100, 83, 654, 653
};

bool validSensor[16] = {
  true, true, true, true,
  false, true, true, true,
  true, true, true, true,
  true, true, true, true
};

static bool detectButtonPress(int pin) {
  return digitalRead(pin) == LOW;
}

static void success() {
}

static bool calibrationReadCommand(char target) {
  if (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (c == target) return true;
  }

  if (SoftSerial.available() > 0) {
    char c = (char)SoftSerial.read();
    if (c == target) return true;
  }

  return false;
}

static bool calibrationReadExit() {
  if (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (c == 'X' || c == 'x') return true;
  }

  if (SoftSerial.available() > 0) {
    char c = (char)SoftSerial.read();
    if (c == 'X' || c == 'x') return true;
  }

  return false;
}

static void sampleSurfaceFor10s(
  uint16_t outMin[16],
  uint16_t outMax[16],
  float outAvg[16]
) {
  uint32_t sum[16] = {0};
  uint32_t count = 0;

  for (int i = 0; i < 16; i++) {
    outMin[i] = 1023;
    outMax[i] = 0;
    outAvg[i] = 0.0f;
  }

  unsigned long startMs = millis();

  while ((millis() - startMs) < 10000UL) {
    readSensor();

    for (int i = 0; i < 16; i++) {
      if (reading[i] < outMin[i]) outMin[i] = reading[i];
      if (reading[i] > outMax[i]) outMax[i] = reading[i];
      sum[i] += reading[i];
    }

    count++;
    delay(5);
  }

  if (count == 0) count = 1;

  for (int i = 0; i < 16; i++) {
    outAvg[i] = (float)sum[i] / (float)count;
  }
}

static void printCalibrationTable(
  const uint16_t blackMin[16],
  const uint16_t blackMax[16],
  const float blackAvg[16],
  const uint16_t whiteMin[16],
  const uint16_t whiteMax[16],
  const float whiteAvg[16]
) {
  Serial.println(F("Idx\tBmin\tBmax\tBavg\tWmin\tWmax\tWavg\tDiff\tThr\tGap\tStatus"));
  SoftSerial.println(F("Idx\tBmin\tBmax\tBavg\tWmin\tWmax\tWavg\tDiff\tThr\tGap\tStatus"));

  for (int i = 0; i < 16; i++) {
    float diff = whiteAvg[i] - blackAvg[i];
    float thr = (whiteAvg[i] + blackAvg[i]) * 0.5f;
    int gap = (int)whiteMin[i] - (int)blackMax[i];

    const char *status = "BAD";
    if (!validSensor[i]) {
      status = "BAD";
    } else if (gap >= 40 && diff >= 40.0f) {
      status = "EXCELLENT";
    } else {
      status = "GOOD";
    }

    Serial.print(i + 1);         Serial.print('\t');
    Serial.print(blackMin[i]);   Serial.print('\t');
    Serial.print(blackMax[i]);   Serial.print('\t');
    Serial.print(blackAvg[i], 2);Serial.print('\t');
    Serial.print(whiteMin[i]);   Serial.print('\t');
    Serial.print(whiteMax[i]);   Serial.print('\t');
    Serial.print(whiteAvg[i], 2);Serial.print('\t');
    Serial.print(diff, 2);       Serial.print('\t');
    Serial.print(thr, 2);        Serial.print('\t');
    Serial.print(gap);           Serial.print('\t');
    Serial.println(status);

    SoftSerial.print(i + 1);         SoftSerial.print('\t');
    SoftSerial.print(blackMin[i]);   SoftSerial.print('\t');
    SoftSerial.print(blackMax[i]);   SoftSerial.print('\t');
    SoftSerial.print(blackAvg[i], 2);SoftSerial.print('\t');
    SoftSerial.print(whiteMin[i]);   SoftSerial.print('\t');
    SoftSerial.print(whiteMax[i]);   SoftSerial.print('\t');
    SoftSerial.print(whiteAvg[i], 2);SoftSerial.print('\t');
    SoftSerial.print(diff, 2);       SoftSerial.print('\t');
    SoftSerial.print(thr, 2);        SoftSerial.print('\t');
    SoftSerial.print(gap);           SoftSerial.print('\t');
    SoftSerial.println(status);
  }
}

uint16_t readSensor() {
  uint16_t dataSensorBit = 0;

  const static uint8_t muxSequence[16][2] = {
    {0,  0b0000}, {1,  0b0001}, {3,  0b0011}, {2,  0b0010},
    {6,  0b0110}, {7,  0b0111}, {5,  0b0101}, {4,  0b0100},
    {12, 0b1100}, {13, 0b1101}, {15, 0b1111}, {14, 0b1110},
    {10, 0b1010}, {11, 0b1011}, {9,  0b1001}, {8,  0b1000}
  };

  for (byte i = 0; i < 16; i++) {
    digitalWrite(s3, (muxSequence[i][1] >> 3) & 1);
    digitalWrite(s2, (muxSequence[i][1] >> 2) & 1);
    digitalWrite(s1, (muxSequence[i][1] >> 1) & 1);
    digitalWrite(s0, muxSequence[i][1] & 1);

    delayMicroseconds(5);
    reading[muxSequence[i][0]] = analogRead(sig);
  }

  // for (int i = 0; i < 16; i++) {
  //   if (reading[i] < setting.LIMIT_VALUE_SENSOR[i]) {
  //     bitSet(dataSensorBit, 15 - i);
  //   }
  // }
  for (int i = 0; i < 16; i++) {
    if (!validSensor[i]) continue;
//                 <
    if (reading[i] > setting.LIMIT_VALUE_SENSOR[i]) {
      bitSet(dataSensorBit, 15 - i);
    }
  }


  if (setting.lineColor == whiteLine) {
    dataSensorBit = ~dataSensorBit & 0xFFFF;
  }

  return dataSensorBit;
}

void setSensorSensivity(byte sensitivity) {
  setting.sensor_sensitivity = sensitivity;
}

void calibrateSensor() {
  unsigned int minVal[16];
  unsigned int maxVal[16] = {0};

  for (int i = 0; i < 16; i++) minVal[i] = 1023;

  while (1) {
    readSensor();
    for (byte i = 0; i < 16; i++) {
      if (reading[i] > maxVal[i]) maxVal[i] = reading[i];
      if (reading[i] < minVal[i]) minVal[i] = reading[i];
    }

    if (detectButtonPress(SW0)) {
      success();
      break;
    }
  }

  for (int i = 0; i < 16; i++) {
    uint16_t range = maxVal[i] - minVal[i];
    float thresholdPercent = (float)setting.sensor_sensitivity / 100.0f;
    setting.LIMIT_VALUE_SENSOR[i] = minVal[i] + (uint16_t)(range * (1.0f - thresholdPercent));
  }
}


void calibrationModeLoop() {
  uint16_t blackMin[16], blackMax[16];
  uint16_t whiteMin[16], whiteMax[16];
  float blackAvg[16], whiteAvg[16];

  setMotor(0, 0);
  g_L = 0;
  g_R = 0;

  Serial.println(F("=== CALIBRATION MODE M=5 ==="));
  SoftSerial.println(F("=== CALIBRATION MODE M=5 ==="));

  Serial.println(F("Send B and place on BLACK surface"));
  SoftSerial.println(F("Send B and place on BLACK surface"));

  while (!calibrationReadCommand('B') && !calibrationReadCommand('b')) {
    if (calibrationReadExit()) {
      Serial.println(F("Calibration aborted"));
      SoftSerial.println(F("Calibration aborted"));
      g_M = 1;
      return;
    }
    delay(10);
  }

  Serial.println(F("Sampling BLACK for 10 seconds..."));
  SoftSerial.println(F("Sampling BLACK for 10 seconds..."));
  sampleSurfaceFor10s(blackMin, blackMax, blackAvg);

  Serial.println(F("BLACK capture done"));
  SoftSerial.println(F("BLACK capture done"));

  Serial.println(F("Send W and place on WHITE surface"));
  SoftSerial.println(F("Send W and place on WHITE surface"));

  while (!calibrationReadCommand('W') && !calibrationReadCommand('w')) {
    if (calibrationReadExit()) {
      Serial.println(F("Calibration aborted"));
      SoftSerial.println(F("Calibration aborted"));
      g_M = 1;
      return;
    }
    delay(10);
  }

  Serial.println(F("Sampling WHITE for 10 seconds..."));
  SoftSerial.println(F("Sampling WHITE for 10 seconds..."));
  sampleSurfaceFor10s(whiteMin, whiteMax, whiteAvg);

  Serial.println(F("WHITE capture done"));
  SoftSerial.println(F("WHITE capture done"));

  Serial.println(F("\n=== BLACK SURFACE STATS ==="));
SoftSerial.println(F("\n=== BLACK SURFACE STATS ==="));

for (int i = 0; i < 16; i++) {
  Serial.print(F("S")); Serial.print(i+1);
  Serial.print(F(" -> Min: ")); Serial.print(blackMin[i]);
  Serial.print(F(" Max: ")); Serial.print(blackMax[i]);
  Serial.print(F(" Avg: ")); Serial.println(blackAvg[i], 2);

  SoftSerial.print(F("S")); SoftSerial.print(i+1);
  SoftSerial.print(F(" -> Min: ")); SoftSerial.print(blackMin[i]);
  SoftSerial.print(F(" Max: ")); SoftSerial.print(blackMax[i]);
  SoftSerial.print(F(" Avg: ")); SoftSerial.println(blackAvg[i], 2);
}

Serial.println(F("\n=== WHITE SURFACE STATS ==="));
SoftSerial.println(F("\n=== WHITE SURFACE STATS ==="));

for (int i = 0; i < 16; i++) {
  Serial.print(F("S")); Serial.print(i+1);
  Serial.print(F(" -> Min: ")); Serial.print(whiteMin[i]);
  Serial.print(F(" Max: ")); Serial.print(whiteMax[i]);
  Serial.print(F(" Avg: ")); Serial.println(whiteAvg[i], 2);

  SoftSerial.print(F("S")); SoftSerial.print(i+1);
  SoftSerial.print(F(" -> Min: ")); SoftSerial.print(whiteMin[i]);
  SoftSerial.print(F(" Max: ")); SoftSerial.print(whiteMax[i]);
  SoftSerial.print(F(" Avg: ")); SoftSerial.println(whiteAvg[i], 2);
}

  for (int i = 0; i < 16; i++) {
    float diff = whiteAvg[i] - blackAvg[i];
    float thr = (whiteAvg[i] + blackAvg[i]) * 0.5f;
    int gap = (int)whiteMin[i] - (int)blackMax[i];
    bool overlap = (whiteMin[i] <= blackMax[i]);

    bool isGood = false;
    bool isExcellent = false;

    if (!overlap && diff >= 15.0f && gap >= 10) {
      isGood = true;
    }

    if (!overlap && diff >= 40.0f && gap >= 40) {
      isExcellent = true;
    }

    validSensor[i] = (isGood || isExcellent);

    if (thr < 0.0f) thr = 0.0f;
    if (thr > 1023.0f) thr = 1023.0f;

    preDefinedThreshold[i] = (uint16_t)(thr + 0.5f);
    setting.LIMIT_VALUE_SENSOR[i] = preDefinedThreshold[i];
  }

  printCalibrationTable(blackMin, blackMax, blackAvg, whiteMin, whiteMax, whiteAvg);

  Serial.println(F("Updated validSensor[] = {"));
  SoftSerial.println(F("Updated validSensor[] = {"));
  for (int i = 0; i < 16; i++) {
    Serial.print(validSensor[i] ? F("true") : F("false"));
    SoftSerial.print(validSensor[i] ? F("true") : F("false"));

    if (i < 15) {
      Serial.print(F(", "));
      SoftSerial.print(F(", "));
    }
  }
  Serial.println(F("};"));
  SoftSerial.println(F("};"));

  Serial.println(F("Updated preDefinedThreshold[] = {"));
  SoftSerial.println(F("Updated preDefinedThreshold[] = {"));
  for (int i = 0; i < 16; i++) {
    Serial.print(preDefinedThreshold[i]);
    SoftSerial.print(preDefinedThreshold[i]);

    if (i < 15) {
      Serial.print(F(", "));
      SoftSerial.print(F(", "));
    }
  }
  Serial.println(F("};"));
  SoftSerial.println(F("};"));

  Serial.println(F("Calibration done. Send X to exit from this loop."));
  SoftSerial.println(F("Calibration done. Send X to exit from this loop."));

  while (1) {
    setMotor(0, 0);
    g_L = 0;
    g_R = 0;

    if (calibrationReadExit()) {
      Serial.println(F("Exit calibration mode"));
      SoftSerial.println(F("Exit calibration mode"));
      g_M = 1;
      return;
    }

    delay(20);
  }
}

int16_t errorD(int dataSensor) {
  int16_t out = 0;

  switch (dataSensor) {
    case 0b0000000110000000: out = 0; break;
    case 0b0000001110000000: out = 1; break;
    case 0b0000000100000000: out = 1; break;
    case 0b0000001100000000: out = 2; break;
    case 0b0000011100000000: out = 3; break;
    case 0b0000001000000000: out = 3; break;
    case 0b0000011000000000: out = 4; break;
    case 0b0000111000000000: out = 5; break;
    case 0b0000010000000000: out = 5; break;
    case 0b0000110000000000: out = 6; break;
    case 0b0001110000000000: out = 7; break;
    case 0b0000100000000000: out = 7; break;
    case 0b0001100000000000: out = 8; break;
    case 0b0011100000000000: out = 9; break;
    case 0b0001000000000000: out = 9; break;
    case 0b0011000000000000: out = 10; break;
    case 0b0111000000000000: out = 12; break;
    case 0b0010000000000000: out = 12; break;
    case 0b0110000000000000: out = 14; break;
    case 0b1110000000000000: out = 15; break;
    case 0b0100000000000000: out = 15; break;
    case 0b1100000000000000: out = 16; break;
    case 0b1000000000000000: out = 17; break;

    case 0b0000000111000000: out = -1; break;
    case 0b0000000010000000: out = -1; break;
    case 0b0000000011000000: out = -2; break;
    case 0b0000000011100000: out = -3; break;
    case 0b0000000001000000: out = -3; break;
    case 0b0000000001100000: out = -4; break;
    case 0b0000000001110000: out = -5; break;
    case 0b0000000000100000: out = -5; break;
    case 0b0000000000110000: out = -6; break;
    case 0b0000000000111000: out = -7; break;
    case 0b0000000000010000: out = -7; break;
    case 0b0000000000011000: out = -8; break;
    case 0b0000000000011100: out = -9; break;
    case 0b0000000000001000: out = -9; break;
    case 0b0000000000001100: out = -10; break;
    case 0b0000000000001110: out = -12; break;
    case 0b0000000000000100: out = -12; break;
    case 0b0000000000000110: out = -14; break;
    case 0b0000000000000111: out = -15; break;
    case 0b0000000000000010: out = -15; break;
    case 0b0000000000000011: out = -16; break;
    case 0b0000000000000001: out = -17; break;
    default: out = 0; break;
  }

  return out;
}




int16_t errorA(uint16_t bits) {
  const int16_t w[16] = {
    -8, -7, -6,  -5,
    -4, -3, -2,  -1,
     1,  2,  3,   4,
     5,  6,  7,   8
  };

  int32_t sum = 0;
  int16_t cnt = 0;

  for (int i = 0; i < 16; i++) {
    if (!validSensor[i]) continue;

    if (bits & (1u << (15 - i))) {
      sum += w[i];
      cnt++;
    }
  }

  if (cnt == 0) {
    return lastError;
  }

  return (int16_t)((sum / cnt)*100/8);
}

// int16_t errorA(uint16_t bits) {
//   const int16_t w[16] = {-150, -130, -110, -90, -70, -50, -30, -10, 10, 30, 50, 70, 90, 110, 130, 150};
//   int32_t sum = 0;
//   int16_t cnt = 0;

//   for (int i = 0; i < 16; i++) {
//     if (bits & (1u << i)) {
//       sum += w[i];
//       cnt++;
//     }
//   }

//   if (cnt == 0) {
//     return 0;
//   }

//   return (int16_t)(sum / cnt);
// }
