/*
  Abstraction
  ----------
  This program is getting from tactile sensor to controle leds.

  International NASA Space Apps Challenge 2019


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
  Monyu, Osaka, Japan
  

  Developer
  ----------
  Shunya TANAKA
  
    
*/

#define ID_TOUCH_SENSE 0x6d
#define PIN_LED_RED 25
#define PIN_LED_GREEN 32
#define PIN_LED_BLUE 33

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

  // PC to MCU
  Serial.begin(115200);

  // MCU to tactile sensor
  Serial2.begin(57600);

}

void loop() {

  // To handle brigtness led
  int _brightness_led[4] = { 0 };


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
    _brightness_led[0] = map(_tactile_distance[0], 0.0, 6.5, 0, 255);
    _brightness_led[1] = map(_tactile_distance[1], 0.0, 6.5, 0, 255);
    _brightness_led[2] = map(_tactile_distance[2], 0.0, 6.5, 0, 255);
    _brightness_led[3] = map(_tactile_distance[3], 0.0, 6.5, 0, 255);


  }

  // Control led
  ledcWrite(0, _brightness_led[0]);   // Blue
  ledcWrite(1, _brightness_led[1]);   // Green
  ledcWrite(2, _brightness_led[2]);   // Red

}
