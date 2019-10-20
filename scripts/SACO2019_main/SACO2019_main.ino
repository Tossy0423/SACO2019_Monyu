/*
  Abstraction
  ----------
  This is the operating system of the ship life activity "Monyu"(Monyu-OS).
  Monyu-OS receives plasma temperature sent from NOAA API.
  A system that changes the color of Monyu depending on the degree of danger.
  Monyu changes color, it informs staff working onboard and offboard about the degree of danger.

  Development enviroment
  ----------
  OS
    macOS Mojave 10.14.6

  IDE
    ArduinoIDE 1.8.8

  MCU
    ESP32s

  Tactile sensor
    Shokkakupod, Touchence


  Team
  ----------
  ESP4589, Osaka, Japan


  Developer
  ----------
  Shunya TANAKA
  Takuro MIKAMI

  @International NASA Space Apps Challenge 2019

*/

/* ========== Include files ========== */
#include <iostream>
#include <string>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>


/* ========== Define address pinassign ========== */
#define ID_TOUCH_SENSE 0x6d
#define PIN_LED_RED 25
#define PIN_LED_GREEN 32
#define PIN_LED_BLUE 33

/* ========== Define address NOAA ========== */
const String endpoint = "https://services.swpc.noaa.gov/products/solar-wind/plasma-5-minute.json";

/* ========== Define string value for json ========== */
StaticJsonDocument<200> doc;

/* ========== Setting Wifi========== */
WiFiClient httpsClient;
PubSubClient mqttClient(httpsClient);
// SSID & Pass
const char* ssid = "SACO2019_Monyu";
const char* password = "asdf1234";




void setup() {

  // Set Timer to use led for pwm contorl
  ledcSetup(0, 12800, 8);
  ledcAttachPin(PIN_LED_BLUE, 0);

  ledcSetup(1, 12800, 8);
  ledcAttachPin(PIN_LED_GREEN, 1);

  ledcSetup(2, 12800, 8);
  ledcAttachPin(PIN_LED_RED, 2);


  // On board LED
  pinMode(2, OUTPUT);

  // MCU to tactile sensor
  Serial2.begin(57600);

  // MCU to PC 
  Serial.begin(115200);

  //WiFiの設定
  WiFi.begin(ssid, password);  //  Wi-Fi APに接続 ----A
  while (WiFi.status() != WL_CONNECTED) {  //  Wi-Fi AP接続待ち
    delay(500);
    Serial.print(".");
  }
  Serial.print("WiFi connected\r\nIP address: ");
  Serial.println(WiFi.localIP());

}

void loop() {

  // To handle brigtness led
  int _brightness_led[4] = { 0 };

  // To use plasma temprature
  int temp = 0;


  if ((WiFi.status() == WL_CONNECTED)) {

    digitalWrite(2, HIGH);

    HTTPClient http;

    http.begin(endpoint); //URLを指定
    int httpCode = http.GET();  //GETリクエストを送信

    if (httpCode > 0) { //返答がある場合

      String payload = http.getString();  //返答（JSON形式）を取得
      Serial.print(payload);
      Serial.printf("\n");


      // pick up data
      // 不要な部分を削除, これを行わないときちんとデータが取り出せない(ESPのバッファエラー？)
      payload.remove(0, 186);
      Serial.print(payload);

      // 10文字目から5文字分を取り出す.
      String sent = payload.substring(10, 5);


      temp = sent.toInt();
      Serial.printf("temprature=%d", temp);
      Serial.print("\n");




    }

    else {
      Serial.println("Error on HTTP request");
    }

    http.end(); //Free the resources
  } else {
    digitalWrite(2, LOW);
  }

  // delay(500);   //30秒おきに更新


  /* ========== Serial communication from MCU to tactile sensor ========== */

  // Write to tactile sensor
  Serial2.write(ID_TOUCH_SENSE);

  // Wait To Sampling
  delayMicroseconds(1260);

  // Receive data from tactile sensor
  if (0 < Serial2.available())
  {
    // Define receive data (uint8_t)
    uint8_t _r_data_char[10] = { 0 };

    // Receive data
    for (int i = 0; i < 8; i++)_r_data_char[i] = Serial2.read();

    // DEBUG
    //Serial.printf("------------------------------\n");
    //Serial.printf("[DEBUG]: Receive Data(char)\n");
    //for(int i = 0; i < 8; i++)Serial.printf("[%d] = %d, ", i, _r_data_char[i]);
    //Serial.print("\n");

    // Define receive data(int)
    unsigned int _r_data_int[4] = {0};

    // Convert "char" to "int"
    _r_data_int[0] = _r_data_char[0] << 8 | _r_data_char[1];
    _r_data_int[1] = _r_data_char[2] << 8 | _r_data_char[3];
    _r_data_int[2] = _r_data_char[4] << 8 | _r_data_char[5];
    _r_data_int[3] = _r_data_char[6] << 8 | _r_data_char[7];

    // DEBUG
    // Serial.printf("------------------------------\n");
    // Serial.printf("[DEBUG]: Converted Data(int)\n");
    // for (int i = 0; i < 4; i++)Serial.printf("[%d] = %d, ", i, _r_data_int[i]);
    // Serial.print("\n");

    // Convert to tactile distance
    double _tactile_distance[4] = { 0 };
    _tactile_distance[0] = _r_data_int[0] / 100.0 * 6.0;
    _tactile_distance[1] = _r_data_int[1] / 100.0 * 6.0;
    _tactile_distance[2] = _r_data_int[2] / 100.0 * 6.0;
    _tactile_distance[3] = _r_data_int[3] / 100.0 * 6.0;

    // DEBUG
    //Serial.printf("------------------------------\n");
    //Serial.printf("[DEBUG]: Tactile distance(float)\n");
    //for (int i = 0; i < 4; i++)Serial.printf("[%d] = %0.2f[mm], ", i, _tactile_distance[i]);
    //Serial.print("\n");

    // Normalization
    _brightness_led[0] = map(_tactile_distance[0], 0, 6.0, 0, 255);
    _brightness_led[1] = map(_tactile_distance[1], 0, 6.0, 0, 255);
    _brightness_led[2] = map(_tactile_distance[2], 0, 6.0, 0, 255);
    _brightness_led[3] = map(_tactile_distance[3], 0, 6.0, 0, 255);

    //_brightness_led[0] = map(temp, 50000, 85000, 0, 255);
    //_brightness_led[1] = map(temp, 50000, 85000, 0, 255);
    //_brightness_led[2] = map(temp, 50000, 85000, 0, 255);
    //_brightness_led[3] = map(temp, 50000, 85000, 0, 255);


  }



  if (60000 <= temp) {
    // Control led
    ledcWrite(0, 0);   // Blue
    ledcWrite(1, 20 + _brightness_led[0]);   // Green
    ledcWrite(2, 255 + _brightness_led[1]);   // Red

  } else if (50000 <= temp && temp <= 59999) {

    // Control led
    ledcWrite(0, 255 + _brightness_led[2]);
    ledcWrite(1, 30 + _brightness_led[3]);
    ledcWrite(2, 0);

  } else {
    ledcWrite(0, _brightness_led[2]);
    ledcWrite(1, 30);
    ledcWrite(2,  0);
  }


}
