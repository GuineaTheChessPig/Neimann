#include <Wire.h>
#include <SoftwareSerial.h>

const int buttonA = 2;
const int vibA = 5;
const int slaveAddress = 8;

SoftwareSerial bluetooth(3, 4); // RX, TX

int countA = 0;
bool lastStateA = LOW;
unsigned long lastPressTimeA = 0;
const unsigned long debounceDelay = 50;
const unsigned long pauseDelay = 1000;

String inputA = "";
bool inputCompleteA = false;

void setup() {
  pinMode(buttonA, INPUT);
  pinMode(vibA, OUTPUT);
  Wire.begin(); // 主機模式
  bluetooth.begin(9600);
  Serial.begin(9600);
}

void loop() {
  bool currentStateA = digitalRead(buttonA);
  if (currentStateA != lastStateA) {
    if (currentStateA == HIGH) {
      countA++;
      lastPressTimeA = millis();
    }
    lastStateA = currentStateA;
  }
  if (millis() - lastPressTimeA > pauseDelay && countA > 0) {
    inputA += String(countA) + " ";
    countA = 0;
    inputCompleteA = true;
  }

  if (inputCompleteA) {
    Wire.beginTransmission(slaveAddress);
    Wire.write(inputA.c_str());
    Wire.endTransmission();
    inputA = "";
    inputCompleteA = false;
  }

  if (bluetooth.available()) {
    String command = bluetooth.readStringUntil('\n');
    executeMove(command);
  }
}

void executeMove(String move) {
  char startFile = move.charAt(0);
  char startRank = move.charAt(1);

  int startFileCount = startFile - 'a' + 1;
  int startRankCount = startRank - '1' + 1;

  vibrate(vibA, startFileCount);
  delay(1000);
  vibrate(vibA, startRankCount);
}

void vibrate(int pin, int count) {
  for (int i = 0; i < count; i++) {
    digitalWrite(pin, HIGH);
    delay(200);
    digitalWrite(pin, LOW);
    delay(200);
  }
}
