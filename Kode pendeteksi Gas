#define BLYNK_TEMPLATE_ID "TMPL6HAcENy5c"
#define BLYNK_TEMPLATE_NAME "Project Akhir"
#define BLYNK_AUTH_TOKEN "vfH0lSrj0RwPFYGoDST4TMMAp3Lx8DcM"  // Gantilah dengan Auth Token Anda

#define MQ135_PIN 34     // Pin analog untuk MQ-135
#define RED_PIN 25       // Pin LED RGB warna merah
#define GREEN_PIN 26     // Pin LED RGB warna hijau
#define BLUE_PIN 27      // Pin LED RGB warna biru
#define RELAY_PIN 18     // Pin relay
#define BUZZER_PIN 15    // Pin untuk buzzer pasif

// Logika kontrol relay
int relayon = HIGH;   // Relay aktif (ON) saat pin LOW
int relayoff = LOW;   // Relay mati (OFF) saat pin HIGH

// Threshold kualitas udara
const int goodThreshold = 800;       // Batas kualitas udara bagus (ppm)
const int moderateThreshold = 1000;  // Batas kualitas udara sedang (ppm)

// Variabel waktu
unsigned long previousMillisSensor = 0;  // Waktu pembacaan terakhir
const long sensorInterval = 3000;        // Interval pembacaan sensor (3 detik)

// Variabel buzzer
unsigned long previousMillisBuzzer = 0;
const long buzzerInterval = 500;   // Interval bunyi buzzer (0.5 detik)
bool buzzerState = false;          // Status buzzer
const int buzzerFrequency = 2000;  // Frekuensi buzzer dalam Hz

int previousAirQualityCategory = -1;  // Kategori kualitas udara sebelumnya

// Data Wi-Fi
char ssid[] = "Waw";         // Ganti dengan SSID Wi-Fi Anda
char pass[] = "11212121";         // Ganti dengan password Wi-Fi Anda

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <HTTPClient.h> // Library untuk HTTP request ke Telegram

// Virtual Pin untuk Blynk
#define VIRTUAL_PIN_AIR_QUALITY V1  // Virtual pin untuk menampilkan kualitas udara (ppm)
#define VIRTUAL_PIN_ALCOHOL V2      // Virtual pin untuk menampilkan kadar alkohol (ppm)
#define VIRTUAL_PIN_SMOKE V3        // Virtual pin untuk menampilkan kadar asap (ppm)
#define VIRTUAL_PIN_LED_RED V4      // Virtual pin untuk kontrol LED Merah

const String botToken = "8065084558:AAHa5MQVCWkkNyil0dbzxHWmXip37AWBhss"; // Bot token Telegram
const int chatId = 1493830472; // Chat ID Telegram

void setup() {
  // Konfigurasi pin
  pinMode(MQ135_PIN, INPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Hubungkan ke Wi-Fi
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Pastikan perangkat dimulai dalam kondisi OFF
  digitalWrite(RELAY_PIN, relayoff); // Matikan relay di awal
  noTone(BUZZER_PIN);               // Matikan buzzer
  digitalWrite(RED_PIN, LOW);       // Matikan LED merah
  digitalWrite(GREEN_PIN, LOW);     // Matikan LED hijau
  digitalWrite(BLUE_PIN, LOW);      // Matikan LED biru

  Serial.begin(115200);
  Serial.println("System Initialized. Relay set to OFF.");
}

void loop() {
  unsigned long currentMillis = millis();

  // Baca kualitas udara setiap sensorInterval
  if (currentMillis - previousMillisSensor >= sensorInterval) {
    previousMillisSensor = currentMillis;

    int airQuality = analogRead(MQ135_PIN);
    float ppmAlcohol = map(airQuality, 0, 1023, 0, 500);  // Konversi nilai ADC ke ppm alkohol
    float ppmSmoke = map(airQuality, 0, 1023, 0, 1000);   // Konversi nilai ADC ke ppm asap
    float ppmAirQuality = (ppmAlcohol + ppmSmoke) / 2;    // Rata-rata konsentrasi
    int airQualityCategory = -1;
    String airQualityStatus = "";

    // Tentukan kategori kualitas udara
    if (ppmAirQuality < goodThreshold) {
      airQualityCategory = 0; // Udara bagus
      airQualityStatus = "Baik";
    } else if (ppmAirQuality >= goodThreshold && ppmAirQuality < moderateThreshold) {
      airQualityCategory = 1; // Udara sedang
      airQualityStatus = "Waspada";
    } else {
      airQualityCategory = 2; // Udara buruk
      airQualityStatus = "Bahaya";
      
      // Kirim notifikasi ke Telegram jika kualitas udara buruk
      sendTelegramNotification("Peringatan: Bahaya! Kualitas udara buruk!");
    }

    // Tampilkan data di Serial Monitor
    Serial.print("Konsentrasi Alkohol: ");
    Serial.print(ppmAlcohol);
    Serial.println(" ppm");
    Serial.print("Konsentrasi Asap: ");
    Serial.print(ppmSmoke);
    Serial.println(" ppm");
    Serial.print("Kualitas Udara: ");
    Serial.print(ppmAirQuality);
    Serial.println(" ppm");
    Serial.print("Status: ");
    Serial.println(airQualityStatus);
    Serial.println("-------------------------------");

    // Kirimkan kadar alkohol dan kadar asap ke aplikasi Blynk
    Blynk.virtualWrite(VIRTUAL_PIN_ALCOHOL, ppmAlcohol);  // Kirimkan kadar alkohol
    Blynk.virtualWrite(VIRTUAL_PIN_SMOKE, ppmSmoke);      // Kirimkan kadar asap
    Blynk.virtualWrite(VIRTUAL_PIN_AIR_QUALITY, ppmAirQuality);  // Kirimkan kualitas udara

    // Tentukan LED berdasarkan status kualitas udara
    if (airQualityCategory != previousAirQualityCategory) {
      previousAirQualityCategory = airQualityCategory;

      if (airQualityCategory == 0) {
        // Udara bagus
        digitalWrite(GREEN_PIN, HIGH);
        digitalWrite(RED_PIN, LOW);
        digitalWrite(BLUE_PIN, LOW);
        digitalWrite(RELAY_PIN, relayoff); // Matikan relay
        noTone(BUZZER_PIN);               // Matikan buzzer
        Serial.println("Relay OFF. Udara Baik.");
        Blynk.virtualWrite(VIRTUAL_PIN_LED_RED, LOW);  // LED Merah OFF
      } else if (airQualityCategory == 1) {
        // Udara sedang
        digitalWrite(GREEN_PIN, HIGH);
        digitalWrite(RED_PIN, HIGH);
        digitalWrite(BLUE_PIN, LOW);
        digitalWrite(RELAY_PIN, relayoff); // Matikan relay
        noTone(BUZZER_PIN);               // Matikan buzzer
        Serial.println("Relay OFF. Udara Sedang.");
        Blynk.virtualWrite(VIRTUAL_PIN_LED_RED, LOW);  // LED Merah OFF
      } else {
        // Udara buruk
        digitalWrite(GREEN_PIN, LOW);
        digitalWrite(RED_PIN, HIGH);
        digitalWrite(BLUE_PIN, LOW);
        digitalWrite(RELAY_PIN, relayon); // Aktifkan relay
        Serial.println("Relay ON. Udara Buruk.");
        Blynk.virtualWrite(VIRTUAL_PIN_LED_RED, HIGH);  // LED Merah ON
      }
    }
  }

  // Kontrol buzzer jika kualitas udara buruk
  if (previousAirQualityCategory == 2) {
    if (currentMillis - previousMillisBuzzer >= buzzerInterval) {
      previousMillisBuzzer = currentMillis;
      buzzerState = !buzzerState;

      if (buzzerState) {
        tone(BUZZER_PIN, buzzerFrequency); // Nyalakan buzzer
        Serial.println("Buzzer ON.");
      } else {
        noTone(BUZZER_PIN);               // Matikan buzzer
        Serial.println("Buzzer OFF.");
      }
    }
  }

  // Fungsi Blynk
  Blynk.run();
}

// Fungsi untuk mengirim pesan ke bot Telegram
void sendTelegramNotification(String message) {
  HTTPClient http;
  String telegramUrl = "https://api.telegram.org/bot" + botToken + "/sendMessage?chat_id=" + String(chatId) + "&text=" + message;

  http.begin(telegramUrl); // Mulai koneksi HTTP
  int httpCode = http.GET(); // Mengirim GET request

  if (httpCode > 0) {
    Serial.println("Telegram Notification Sent Successfully!");
  } else {
    Serial.println("Error in sending Telegram notification.");
  }
  
  http.end(); // Menutup koneksi HTTP
}
