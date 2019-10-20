//#include <Arduino_JSON.h>

#include <iostream>
#include <string>

#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
//#define server "soldier.cloudmqtt.com"
//#define PORT 14645
#include <ArduinoJson.h>

//温度センサのライブラリ BME280
//#include <Wire.h>
//#include <Adafruit_Sensor.h>
//#include <Adafruit_BME280.h>
//#define SEALEVELPRESSURE_HPA (1013.25)
//Adafruit_BME280 bme; // I2C

//DIP PIN
#define DIP1 32
#define DIP2 33
#define DIP3 34
#define DIP4 35

//StaticJsonBuffer<200> jsonBuffer;

const String endpoint = "https://services.swpc.noaa.gov/products/solar-wind/plasma-5-minute.json";
//const String key = "yourAPIkey";

//wifi設定//////////////////////////////
WiFiClient httpsClient;
PubSubClient mqttClient(httpsClient);

//wifi

// const char* ssid = "takudon";
// const char* password = "utyuyear1444";

const char* ssid = "SpaceAppsWifi-C";
const char* password = "2019osaka";


///////////////////////////////////////

//温度データの変数
float temp;
//温度データをchar型して格納するための配列
char Tempstring[10];
char idBuf[20];
char subscribe_url_char[12];
//グローバル変数
String subscribe_url;
String publish_url;
String deviceID_str;

StaticJsonDocument<200> doc;

void setup() {

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



  if ((WiFi.status() == WL_CONNECTED)) {

    HTTPClient http;

    http.begin(endpoint); //URLを指定
    int httpCode = http.GET();  //GETリクエストを送信

    if (httpCode > 0) { //返答がある場合

      String payload = http.getString();  //返答（JSON形式）を取得
      // Serial.printf("httoCode=%d\n", httpCode);
      // Serial.printf("payload=");
      Serial.print(payload);
      Serial.printf("\n");


      // pick up data
      // mysubstr(&_pick_temperature, payload, 5);
      // String.remove()
      payload.remove(0, 186);
      Serial.print(payload);

      String sent = payload.substring(10, 5);
      Serial.println("-----------------");
      Serial.print(sent);
      Serial.print("\n");

      int temp = 0;
      temp = sent.toInt();
      Serial.printf("hehe=%d", temp);
      Serial.print("\n");




    }

    else {
      Serial.println("Error on HTTP request");
    }

    http.end(); //Free the resources
  }

  delay(500);   //30秒おきに更新

}
