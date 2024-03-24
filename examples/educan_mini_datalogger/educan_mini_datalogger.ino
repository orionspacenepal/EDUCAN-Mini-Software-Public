#include <FS.h>
#include <LittleFS.h>

#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
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
const uint8_t MPU6050_REGISTER_SMPLRT_DIV = 0x19;
const uint8_t MPU6050_REGISTER_USER_CTRL = 0x6A;
const uint8_t MPU6050_REGISTER_PWR_MGMT_1 = 0x6B;
const uint8_t MPU6050_REGISTER_PWR_MGMT_2 = 0x6C;
const uint8_t MPU6050_REGISTER_CONFIG = 0x1A;
const uint8_t MPU6050_REGISTER_GYRO_CONFIG = 0x1B;
const uint8_t MPU6050_REGISTER_ACCEL_CONFIG = 0x1C;
const uint8_t MPU6050_REGISTER_FIFO_EN = 0x23;
const uint8_t MPU6050_REGISTER_INT_ENABLE = 0x38;
const uint8_t MPU6050_REGISTER_ACCEL_XOUT_H = 0x3B;
const uint8_t MPU6050_REGISTER_SIGNAL_PATH_RESET = 0x68;

int16_t AccelX, AccelY, AccelZ, Temperature, GyroX, GyroY, GyroZ;

const char *ssid = "EDUCAN_MINI";      // Replace with your network SSID
const char *password = "1234567890"; // Replace with your network password

AsyncWebServer server(80);
//bool isDataReady = false;
unsigned long lastLogTime = 0;
const unsigned long logInterval = 500;
const unsigned int maxLogEntries = 1452;

File logFile;

bool extractLogFiles = false;

void MPU6050_Init()
{
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

String getTemperature()
{
  double BmpTempr;
  BmpTempr = bmp.readTemperature();
  return String(BmpTempr);
}

String getPressure()
{
  double BmpPress;
  BmpPress = bmp.readPressure();
  return String(BmpPress);
}

String getAltitude()
{
  double BmpAlti;
  BmpAlti = bmp.readAltitude();
  return String(BmpAlti);
}

String getVbat()
{
  double vbat, sensorValue;
  sensorValue = analogRead(A0);
  vbat = 2 * sensorValue * (3.15 / 1023.0);
  return String(vbat);
}

String getAx()
{
  double Ax;
  Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);
  Ax = (double)AccelX / AccelScaleFactor;
  return String(Ax);
}

String getAy()
{
  double Ay;
  Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);
  Ay = (double)AccelY / AccelScaleFactor;
  return String(Ay);
}

String getAz()
{
  double Az;
  Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);
  Az = (double)AccelZ / AccelScaleFactor;
  return String(Az);
}

String getGx()
{
  double Gx;
  Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);
  Gx = (double)GyroX / GyroScaleFactor;
  return String(Gx);
}

String getGy()
{
  double Gy;
  Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);
  Gy = (double)GyroY / GyroScaleFactor;
  return String(Gy);
}

String getGz()
{
  double Gz;
  Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);
  Gz = (double)GyroZ / GyroScaleFactor;
  return String(Gz);
}

void I2C_Write(uint8_t deviceAddress, uint8_t regAddress, uint8_t data)
{
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.write(data);
  Wire.endTransmission();
}



void Read_RawValue(uint8_t deviceAddress, uint8_t regAddress)
{
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


String logData()
{
  String logEntry;
  logFile= LittleFS.open("/log.txt", "a+");
  if (logFile)
  {
    
    logEntry = getTemperature() + "," + getPressure() + "," + getAltitude() + "," +
                      getVbat() + "," + getAx() + "," + getAy() + "," + getAz() + "," +
                      getGx() + "," + getGy() + "," + getGz();
    Serial.println("Log Entry: " + logEntry);

    logFile.println(logEntry);
    logFile.flush();
    checkAndTrimLogFile();    
  }
  return logEntry;
  logFile.close();
}

void checkAndTrimLogFile()
{
  if (LittleFS.exists("/log.txt"))
  {
    logFile = LittleFS.open("/log.txt", "r");
    if (logFile)
    {
      File tempFile = LittleFS.open("/temp.txt", "w");
      if (tempFile)
      {
        tempFile.println("Temperature,Pressure,Altitude,Battery Voltage,Ax,Ay,Az,Gx,Gy,Gz");
        tempFile.flush();

        int count = 0;
        // Skip the header line in the original file
        logFile.readStringUntil('\n');
        while (logFile.available() && count < maxLogEntries)
        {
          tempFile.write(logFile.read());
          count++;
        }
        tempFile.close();
      }
      logFile.close();
      LittleFS.remove("/log.txt");
      LittleFS.rename("/temp.txt", "/log.txt");
    }
  }
}



void handleDataLogging() {
    unsigned long currentTime = millis();
    if (currentTime - lastLogTime >= logInterval) {
        logData();
        Serial.println("inside handle data logging");
        lastLogTime = currentTime;
    }
}

void setup()
{

  Serial.begin(9600);
  Wire.begin(sda, scl);
  MPU6050_Init();
  bmp.begin();
  sensors.begin();

  if (!LittleFS.begin())
  {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
  logFile = LittleFS.open("/log.txt", "a"); // Open log file in append mode
  if (!logFile)
  {
    Serial.println("Error opening log file");
  }
  logFile.close();
  WiFi.softAP(ssid, password);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  // Initialize server routes
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.html"); });
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/style.css", "text/css"); });
  server.on("/highcharts.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/highcharts.js", "application/javascript"); });
  server.on("/vbat", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/plain", getVbat().c_str()); });
  Serial.println("till Vbat end point");
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/plain", getTemperature().c_str()); 
            handleDataLogging();
            });
  server.on("/pressure", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/plain", getPressure().c_str()); });
  server.on("/altitude", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/plain", getAltitude().c_str()); });
    Serial.println("till BMP end point");
  server.on("/ax", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/plain", getAx().c_str()); });
  server.on("/ay", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/plain", getAy().c_str()); });
  server.on("/az", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/plain", getAz().c_str()); });
  server.on("/gx", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/plain", getGx().c_str()); });
  server.on("/gy", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/plain", getGy().c_str()); });
  server.on("/gz", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/plain", getGz().c_str()); 
             });
    server.on("/triggerExtract", HTTP_GET, [](AsyncWebServerRequest *request)
    {
    extractLogFiles = true; // Set the trigger
   logFile = LittleFS.open("/log.txt", "r");
    if (logFile)
    {
        String logContent = logFile.readString();
        request->send(200, "text/plain", logContent);
        logFile.close();
    }
    else
    {
        request->send(500, "text/plain", "Error reading log file");
    }
    });

server.begin();
}

void loop()
{
}
