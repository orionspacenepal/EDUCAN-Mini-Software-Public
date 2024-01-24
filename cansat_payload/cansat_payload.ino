#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>

// Data wire is connected to the Arduino digital pin 4
#define ONE_WIRE_BUS 14

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);

Adafruit_BMP085 bmp;
  
// MPU6050 Slave Device Address
const uint8_t MPU6050SlaveAddress = 0x68;

// Select SDA and SCL pins for I2C communication // change for wemos
const uint8_t scl = 5;
const uint8_t sda = 4;

// Sensitivity scale factor respective to full scale setting provided in datasheet
const uint16_t AccelScaleFactor = 16384;
const uint16_t GyroScaleFactor = 131;

// MPU6050 few configuration register addresses
const uint8_t MPU6050_REGISTER_SMPLRT_DIV   = 0x19;
const uint8_t MPU6050_REGISTER_USER_CTRL    = 0x6A;
const uint8_t MPU6050_REGISTER_PWR_MGMT_1   = 0x6B;
const uint8_t MPU6050_REGISTER_PWR_MGMT_2   = 0x6C;
const uint8_t MPU6050_REGISTER_CONFIG       = 0x1A;
const uint8_t MPU6050_REGISTER_GYRO_CONFIG  = 0x1B;
const uint8_t MPU6050_REGISTER_ACCEL_CONFIG = 0x1C;
const uint8_t MPU6050_REGISTER_FIFO_EN      = 0x23;
const uint8_t MPU6050_REGISTER_INT_ENABLE   = 0x38;
const uint8_t MPU6050_REGISTER_ACCEL_XOUT_H = 0x3B;
const uint8_t MPU6050_REGISTER_SIGNAL_PATH_RESET = 0x68;

int16_t AccelX, AccelY, AccelZ, Temperature, GyroX, GyroY, GyroZ;

double sensor_temp, BmpPress, BmpAlti, T;
//Ay, Az, T, Gx, Gy, Gz, vbat, sensor_temp, BmpTempr, BmpPress, BmpAlti;

const char* ssid = "Dot";  // Replace with your network SSID
const char* password = "1234567890";  // Replace with your network password

AsyncWebServer server(80);

void setup() {

  Serial.begin(9600);
  Wire.begin(sda, scl);
  MPU6050_Init();
  bmp.begin();
  sensors.begin();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  
  // Initialize server routes
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/html", get_html());
  });

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", getTemperature().c_str());
  });
  server.on("/pressure", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", getPressure().c_str());
  });
  server.on("/altitude", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", getAltitude().c_str());
  });

  server.begin();
  Serial.print("Server started at IP: ");
  Serial.println(WiFi.localIP());

}

void loop() {
}

void UpdateSensorData() {

  Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);

  // Divide each with their sensitivity scale factor
//  Ax = (double)AccelX / AccelScaleFactor;
//  Ay = (double)AccelY / AccelScaleFactor;
//  Az = (double)AccelZ / AccelScaleFactor;
  T = (double)Temperature / 340 + 36.53; // Temperature formula
//  Gx = (double)GyroX / GyroScaleFactor;
//  Gy = (double)GyroY / GyroScaleFactor;
//  Gz = (double)GyroZ / GyroScaleFactor;
//  BmpTempr = bmp.readTemperature();
  BmpPress = bmp.readPressure();
  BmpAlti = bmp.readAltitude();

  sensors.requestTemperatures();
  sensor_temp = sensors.getTempCByIndex(0);

//  vbat = analogRead(A0);
//  vbat = map(analogRead(A0), 0, 1023, 0, 4.2);
//  
}

String getTemperature() {

  UpdateSensorData();

  return String(sensor_temp);
}

String getSysTemp() {

  UpdateSensorData();

  return String(T);
}

String getPressure() {

  UpdateSensorData();

  return String(BmpPress);
}

String getAltitude() {

  UpdateSensorData();

  return String(BmpAlti);
}

void I2C_Write(uint8_t deviceAddress, uint8_t regAddress, uint8_t data) {
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.write(data);
  Wire.endTransmission();
}

// Configure MPU6050
void MPU6050_Init() {
  delay(150);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SMPLRT_DIV, 0x07);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_1, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_2, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_CONFIG, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_GYRO_CONFIG, 0x00); // Set +/-250 degree/second full scale
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_CONFIG, 0x00); // Set +/- 2g full scale
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_FIFO_EN, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_INT_ENABLE, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SIGNAL_PATH_RESET, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_USER_CTRL, 0x00);
}

// Read all 14 registers
void Read_RawValue(uint8_t deviceAddress, uint8_t regAddress) {
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.endTransmission();
  Wire.requestFrom(deviceAddress, (uint8_t)14);
  AccelX = (((int16_t)Wire.read() << 8) | Wire.read());
  AccelY = (((int16_t)Wire.read() << 8) | Wire.read());
  AccelZ = (((int16_t)Wire.read() << 8) | Wire.read());
  Temperature = (((int16_t)Wire.read() << 8) | Wire.read());
  GyroX = (((int16_t)Wire.read() << 8) | Wire.read());
  GyroY = (((int16_t)Wire.read() << 8) | Wire.read());
  GyroZ = (((int16_t)Wire.read() << 8) | Wire.read());
}

String get_html() {
  String url;

  url += "<html>";
  url += "<head>";
  url += "<title>CAN-SAT PAYLOAD</title>";
  url += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  url += "<link rel=\"icon\" href=\"data:,\">";
  url += "<style>";
  url += "html {";
  url += "font-family: Arial;";
  url += "display: inline-block;";
  url += "margin: 0px auto;";
  url += "text-align: center;";
  url += "}";
  url += "body {";
  url += "min-width: 310px;";
  url += "max-width: 800px;";
  url += "height: 400px;";
  url += "margin: 0 auto;";
  url += "}";
  url += "h1 {";
  url += "color: #0F3376;";
  url += "padding: 2vh;";
  url += "}";
  url += "p {";
  url += "font-size: 1.5rem;";
  url += "}";
  url += "h2 {";
  url += "font-family: Arial;";
  url += "font-size: 2.5rem;";
  url += "text-align: center;";
  url += "}";
  url += ".units {";
  url += "font-size: 1.2rem;";
  url += "}";
  url += ".sensor-labels {";
  url += "font-size: 1.5rem;";
  url += "vertical-align:middle;";
  url += "padding-bottom: 15px;";
  url += "}";
  url += "</style>";
  url += "</head>";
  url += "<body>";
  url += "<h1>CAN-SAT Payload</h1>";
  url += "<p>";
  url += "<span class=\"sensor-labels\">Temperature: </span>";
  url += "<span id=\"temperature\">%Temperature%</span>";
  url += "<span class=\"units\">&#8451;</span>";
  url += "</p>";
  url += "</body>";
  url += "<script>";
  url += "setInterval(function ( ) {";
  url += "var xhttp = new XMLHttpRequest();";
  url += "xhttp.open(\"GET\", \"/temperature\", true);";
  url += "xhttp.onreadystatechange = function() {";
  url += "if (this.readyState == 4 && this.status == 200) {";
  url += "document.getElementById(\"temperature\").innerHTML = this.responseText;";
  url += "}};";
  url += "xhttp.send();";
  url += "}, 10000 ) ;";
  url += "</script>";
  url += "</html>";

  return url;
}
