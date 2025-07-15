#define BLYNK_TEMPLATE_ID "TMPL6Trt9WhP3"
#define BLYNK_TEMPLATE_NAME "MONITORING KELEMBAPAN"
#define BLYNK_AUTH_TOKEN "14nIP7TQRGDx1JM5HqG1iYdKwu6Xt9Zk"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "Buahar";
char pass[] = "12032004";

const int soilPins[4] = {32, 33, 34, 35};
int moisturePercent[4];

#define RELAY_PIN 26
#define MOISTURE_THRESHOLD 20

#define VPIN_SWITCH V4
#define VPIN_LED V5

bool manualMode = false;

BLYNK_WRITE(VPIN_SWITCH) {
  manualMode = param.asInt();
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);

  for (int i = 0; i < 4; i++) {
    pinMode(soilPins[i], INPUT);
  }
}

void loop() {
  Blynk.run();
  bacaSensorSoil();
  kontrolPompa();
  delay(500); 
}

void bacaSensorSoil() {
  for (int i = 0; i < 4; i++) {
    int raw = analogRead(soilPins[i]);
    moisturePercent[i] = map(raw, 4095, 0, 0, 100); 
    moisturePercent[i] = constrain(moisturePercent[i], 0, 100);

    Serial.print("Sensor ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(moisturePercent[i]);
    Serial.println(" %");

    // Kirim ke Blynk
    Blynk.virtualWrite(V0, moisturePercent[0]);
    Blynk.virtualWrite(V1, moisturePercent[1]);
    Blynk.virtualWrite(V2, moisturePercent[2]);
    Blynk.virtualWrite(V3, moisturePercent[3]);

  }
}

void kontrolPompa() {
  bool tanahKering = false;

  for (int i = 0; i < 4; i++) {
    if (moisturePercent[i] < MOISTURE_THRESHOLD) {
      tanahKering = true;
      break;
    }
  }

  if (manualMode || tanahKering) {
    digitalWrite(RELAY_PIN, HIGH);
    Blynk.virtualWrite(VPIN_LED, 1);
    Serial.println("Pompa ON");
  } else {
    digitalWrite(RELAY_PIN, LOW);
    Blynk.virtualWrite(VPIN_LED, 0);
    Serial.println("Pompa OFF");
  }
}
