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

  
  server.on("/vbat", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", getVbat().c_str());
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

  server.on("/ax", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", getAx().c_str());
  });
  server.on("/ay", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", getAy().c_str());
  });
  server.on("/az", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", getAz().c_str());
  });

  server.on("/gx", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", getGx().c_str());
  });
  server.on("/gy", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", getGy().c_str());
  });
  server.on("/gz", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", getGz().c_str());
  });

  server.begin();
  Serial.print("Server started at IP: ");
  Serial.println(WiFi.localIP());

}

void loop() {
}

void ReadSensorData() {
  // Replace with your actual sensor reading code
  double Ax, Ay, Az, T, Gx, Gy, Gz, vbat, sensor_temp, BmpTempr, BmpPress, BmpAlti;

  Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);

  // Divide each with their sensitivity scale factor
  Ax = (double)AccelX / AccelScaleFactor;
  Ay = (double)AccelY / AccelScaleFactor;
  Az = (double)AccelZ / AccelScaleFactor;
  T = (double)Temperature / 340 + 36.53; // Temperature formula
  Gx = (double)GyroX / GyroScaleFactor;
  Gy = (double)GyroY / GyroScaleFactor;
  Gz = (double)GyroZ / GyroScaleFactor;
  BmpTempr = bmp.readTemperature();
  BmpPress = bmp.readPressure();
  BmpAlti = bmp.readAltitude();

  sensors.requestTemperatures();
  sensor_temp = sensors.getTempCByIndex(0);

  vbat = analogRead(A0);
  double mapped_vbat = map(analogRead(A0), 0, 1023, 0, 4.2);
  
}

String getTemperature() {

  double BmpTempr;

  BmpTempr = bmp.readTemperature();

  return String(BmpTempr);
}

String getPressure() {

  double BmpPress;
  
  BmpPress = bmp.readPressure();

  return String(BmpPress);
}

String getAltitude() {

  double BmpAlti;
  
  BmpAlti = bmp.readAltitude();

  return String(BmpAlti);
}

String getVbat() {

  double vbat;

  vbat = analogRead(A0);
  vbat = map(analogRead(A0), 0, 1023, 0, 4.2);

  return String(vbat);
}

String getAx() {

  double Ax;
  Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);

  Ax = (double)AccelX / AccelScaleFactor;
  
  return String(Ax);
}

String getAy() {

  double Ay;
  Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);

  Ay = (double)AccelY / AccelScaleFactor;
  
  return String(Ay);
}

String getAz() {

  double Az;
  Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);

  Az = (double)AccelZ / AccelScaleFactor;
  
  return String(Az);
}

String getGx() {

  double Gx;
  Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);

  Gx = (double)GyroX / GyroScaleFactor;
  
  return String(Gx);
}

String getGy() {

  double Gy;
  Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);

  Gy = (double)GyroY / GyroScaleFactor;
  
  return String(Gy);
}

String getGz() {

  double Gz;
  Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);
  
  Gz = (double)GyroZ / GyroScaleFactor;
  
  return String(Gz);
}



// next section:
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
  url += "<script src=\"https://code.highcharts.com/highcharts.js\"></script>";
  url += "<style>";
  url += "html {";
  url += "font-family: Arial;";
  url += "display: inline-block;";
  url += "margin: 0px auto;";
  url += "text-align: center;";
  url += "}";
  url += "body {";
  url += "min-width: 310px;";
  url += "max-width: 100%;";
  url += "height: 400px;";
  url += "margin: 0 auto;";
  url += "}";
  url += ".column {";
  url += "float: left;";
  url += "width: 31.85%;";
  url += "padding: 10px;";
  url += "}";
  url += ".row:after {";
  url += "content: "";";
  url += "display: table;";
  url += "clear: both;";
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
  url += "<span class=\"sensor-labels\">Battery Voltage: </span>";
  url += "<span id=\"vbat\">%vbat%</span>";
  url += "<span class=\"units\"> V</span>";
  url += "</p>";
  url += "<div class=\"row\">";
  url += "<div class=\"column\" id=\"chart-temperature\"></div>";
  url += "<div class=\"column\" id=\"chart-pressure\"></div>";
  url += "<div class=\"column\" id=\"chart-altitude\"></div>  ";
  url += "</div>";
  url += "<div class=\"row\">";
  url += "<div class=\"column\" id=\"chart-ax\"></div>";
  url += "<div class=\"column\" id=\"chart-ay\"></div>";
  url += "<div class=\"column\" id=\"chart-az\"></div>  ";
  url += "</div>";
  url += "<div class=\"row\">";
  url += "<div class=\"column\" id=\"chart-gx\"></div>";
  url += "<div class=\"column\" id=\"chart-gy\"></div>";
  url += "<div class=\"column\" id=\"chart-gz\"></div>  ";
  url += "</div>";
  url += "</body>";
  url += "<script>";
  url += "setInterval(function ( ) {";
  url += "var xhttp = new XMLHttpRequest();";
  url += "xhttp.open(\"GET\", \"/vbat\", true);";
  url += "xhttp.onreadystatechange = function() {";
  url += "if (this.readyState == 4 && this.status == 200) {";
  url += "document.getElementById(\"vbat\").innerHTML = this.responseText;";
  url += "}};";
  url += "xhttp.send();";
  url += "}, 10000 ) ;";
  url += "var chartT = new Highcharts.Chart({";
  url += "chart:{ renderTo : 'chart-temperature' },";
  url += "title: { text: 'Current Temperature' },";
  url += "series: [{";
  url += "showInLegend: false,";
  url += "data: []";
  url += "}],";
  url += "plotOptions: {";
  url += "line: { animation: false,";
  url += "dataLabels: { enabled: true }";
  url += "},";
  url += "series: { color: '#059e8a' }";
  url += "},";
  url += "xAxis: { type: 'datetime',";
  url += "dateTimeLabelFormats: { second: '%H:%M:%S' }";
  url += "},";
  url += "yAxis: {";
  url += "title: { text: 'Temperature (&#8451;)' }";
  url += "},";
  url += "credits: { enabled: false }";
  url += "});";
  url += "setInterval(function ( ) {";
  url += "var xhttp = new XMLHttpRequest();";
  url += "xhttp.open(\"GET\", \"/temperature\", true);";
  url += "xhttp.onreadystatechange = function() {";
  url += "if (this.readyState == 4 && this.status == 200) {";
  url += "var myVar = this.responseText;";
  url += "var x = (new Date()).getTime() + (5*3600 + 45*60)*1000,";
  url += "y = parseFloat(myVar);";
  url += "if(chartT.series[0].data.length > 10) {";
  url += "chartT.series[0].addPoint([x, y], true, true, true);";
  url += "} else {";
  url += "chartT.series[0].addPoint([x, y], true, false, true);";
  url += "}}};";
  url += "xhttp.send();";
  url += "}, 10000 );";
  url += "var chartP = new Highcharts.Chart({";
  url += "chart:{ renderTo : 'chart-pressure' },";
  url += "title: { text: 'Current Pressure' },";
  url += "series: [{";
  url += "showInLegend: false,";
  url += "data: []";
  url += "}],";
  url += "plotOptions: {";
  url += "line: { animation: false,";
  url += "dataLabels: { enabled: true }";
  url += "},";
  url += "series: { color: '#059e8a' }";
  url += "},";
  url += "xAxis: { type: 'datetime',";
  url += "dateTimeLabelFormats: { second: '%H:%M:%S' }";
  url += "},";
  url += "yAxis: {";
  url += "title: { text: 'Pressure (mmHg)' }";
  url += "},";
  url += "credits: { enabled: false }";
  url += "});";
  url += "setInterval(function ( ) {";
  url += "var xhttp = new XMLHttpRequest();";
  url += "xhttp.open(\"GET\", \"/pressure\", true);";
  url += "xhttp.onreadystatechange = function() {";
  url += "if (this.readyState == 4 && this.status == 200) {";
  url += "var myVar = this.responseText;";
  url += "var x = (new Date()).getTime() + (5*3600 + 45*60)*1000,";
  url += "y = parseFloat(myVar);";
  url += "if(chartP.series[0].data.length > 10) {";
  url += "chartP.series[0].addPoint([x, y], true, true, true);";
  url += "} else {";
  url += "chartP.series[0].addPoint([x, y], true, false, true);";
  url += "}}};";
  url += "xhttp.send();";
  url += "}, 10000 );";
  url += "var chartA = new Highcharts.Chart({";
  url += "chart:{ renderTo : 'chart-altitude' },";
  url += "title: { text: 'Current Altitude' },";
  url += "series: [{";
  url += "showInLegend: false,";
  url += "data: []";
  url += "}],";
  url += "plotOptions: {";
  url += "line: { animation: false,";
  url += "dataLabels: { enabled: true }";
  url += "},";
  url += "series: { color: '#059e8a' }";
  url += "},";
  url += "xAxis: { type: 'datetime',";
  url += "dateTimeLabelFormats: { second: '%H:%M:%S' }";
  url += "},";
  url += "yAxis: {";
  url += "title: { text: 'Altitude (m)' }";
  url += "},";
  url += "credits: { enabled: false }";
  url += "});";
  url += "setInterval(function ( ) {";
  url += "var xhttp = new XMLHttpRequest();";
  url += "xhttp.open(\"GET\", \"/altitude\", true);";
  url += "xhttp.onreadystatechange = function() {";
  url += "if (this.readyState == 4 && this.status == 200) {";
  url += "var myVar = this.responseText;";
  url += "var x = (new Date()).getTime() + (5*3600 + 45*60)*1000,";
  url += "y = parseFloat(myVar);";
  url += "if(chartA.series[0].data.length > 10) {";
  url += "chartA.series[0].addPoint([x, y], true, true, true);";
  url += "} else {";
  url += "chartA.series[0].addPoint([x, y], true, false, true);";
  url += "}}};";
  url += "xhttp.send();";
  url += "}, 10000 );";
  url += "var chartAx = new Highcharts.Chart({";
  url += "chart:{ renderTo : 'chart-ax' },";
  url += "title: { text: 'Ax' },";
  url += "series: [{";
  url += "showInLegend: false,";
  url += "data: []";
  url += "}],";
  url += "plotOptions: {";
  url += "line: { animation: false,";
  url += "dataLabels: { enabled: true }";
  url += "},";
  url += "series: { color: '#059e8a' }";
  url += "},";
  url += "xAxis: { type: 'datetime',";
  url += "dateTimeLabelFormats: { second: '%H:%M:%S' }";
  url += "},";
  url += "yAxis: {";
  url += "title: { text: 'Ax (rad/s2)' }";
  url += "},";
  url += "credits: { enabled: false }";
  url += "});";
  url += "setInterval(function ( ) {";
  url += "var xhttp = new XMLHttpRequest();";
  url += "xhttp.open(\"GET\", \"/ax\", true);";
  url += "xhttp.onreadystatechange = function() {";
  url += "if (this.readyState == 4 && this.status == 200) {";
  url += "var myVar = this.responseText;";
  url += "var x = (new Date()).getTime() + (5*3600 + 45*60)*1000,";
  url += "y = parseFloat(myVar);";
  url += "if(chartAx.series[0].data.length > 10) {";
  url += "chartAx.series[0].addPoint([x, y], true, true, true);";
  url += "} else {";
  url += "chartAx.series[0].addPoint([x, y], true, false, true);";
  url += "}}};";
  url += "xhttp.send();";
  url += "}, 10000 );";
  url += "var chartAy = new Highcharts.Chart({";
  url += "chart:{ renderTo : 'chart-ay' },";
  url += "title: { text: 'Ay' },";
  url += "series: [{";
  url += "showInLegend: false,";
  url += "data: []";
  url += "}],";
  url += "plotOptions: {";
  url += "line: { animation: false,";
  url += "dataLabels: { enabled: true }";
  url += "},";
  url += "series: { color: '#059e8a' }";
  url += "},";
  url += "xAxis: { type: 'datetime',";
  url += "dateTimeLabelFormats: { second: '%H:%M:%S' }";
  url += "},";
  url += "yAxis: {";
  url += "title: { text: 'Ay (rad/s2)' }";
  url += "},";
  url += "credits: { enabled: false }";
  url += "});";
  url += "setInterval(function ( ) {";
  url += "var xhttp = new XMLHttpRequest();";
  url += "xhttp.open(\"GET\", \"/ay\", true);";
  url += "xhttp.onreadystatechange = function() {";
  url += "if (this.readyState == 4 && this.status == 200) {";
  url += "var myVar = this.responseText;";
  url += "var x = (new Date()).getTime() + (5*3600 + 45*60)*1000,";
  url += "y = parseFloat(myVar);";
  url += "if(chartAy.series[0].data.length > 10) {";
  url += "chartAy.series[0].addPoint([x, y], true, true, true);";
  url += "} else {";
  url += "chartAy.series[0].addPoint([x, y], true, false, true);";
  url += "}}};";
  url += "xhttp.send();";
  url += "}, 10000 );";
  url += "var chartAz = new Highcharts.Chart({";
  url += "chart:{ renderTo : 'chart-az' },";
  url += "title: { text: 'Az' },";
  url += "series: [{";
  url += "showInLegend: false,";
  url += "data: []";
  url += "}],";
  url += "plotOptions: {";
  url += "line: { animation: false,";
  url += "dataLabels: { enabled: true }";
  url += "},";
  url += "series: { color: '#059e8a' }";
  url += "},";
  url += "xAxis: { type: 'datetime',";
  url += "dateTimeLabelFormats: { second: '%H:%M:%S' }";
  url += "},";
  url += "yAxis: {";
  url += "title: { text: 'Az (rad/s2)' }";
  url += "},";
  url += "credits: { enabled: false }";
  url += "});";
  url += "setInterval(function ( ) {";
  url += "var xhttp = new XMLHttpRequest();";
  url += "xhttp.open(\"GET\", \"/az\", true);";
  url += "xhttp.onreadystatechange = function() {";
  url += "if (this.readyState == 4 && this.status == 200) {";
  url += "var myVar = this.responseText;";
  url += "var x = (new Date()).getTime() + (5*3600 + 45*60)*1000,";
  url += "y = parseFloat(myVar);";
  url += "if(chartAz.series[0].data.length > 10) {";
  url += "chartAz.series[0].addPoint([x, y], true, true, true);";
  url += "} else {";
  url += "chartAz.series[0].addPoint([x, y], true, false, true);";
  url += "}}};";
  url += "xhttp.send();";
  url += "}, 10000 );";
  url += "var chartGx = new Highcharts.Chart({";
  url += "chart:{ renderTo : 'chart-gx' },";
  url += "title: { text: 'Gx' },";
  url += "series: [{";
  url += "showInLegend: false,";
  url += "data: []";
  url += "}],";
  url += "plotOptions: {";
  url += "line: { animation: false,";
  url += "dataLabels: { enabled: true }";
  url += "},";
  url += "series: { color: '#059e8a' }";
  url += "},";
  url += "xAxis: { type: 'datetime',";
  url += "dateTimeLabelFormats: { second: '%H:%M:%S' }";
  url += "},";
  url += "yAxis: {";
  url += "title: { text: 'Gx (rad)' }";
  url += "},";
  url += "credits: { enabled: false }";
  url += "});";
  url += "setInterval(function ( ) {";
  url += "var xhttp = new XMLHttpRequest();";
  url += "xhttp.open(\"GET\", \"/gx\", true);";
  url += "xhttp.onreadystatechange = function() {";
  url += "if (this.readyState == 4 && this.status == 200) {";
  url += "var myVar = this.responseText;";
  url += "var x = (new Date()).getTime() + (5*3600 + 45*60)*1000,";
  url += "y = parseFloat(myVar);";
  url += "if(chartGx.series[0].data.length > 10) {";
  url += "chartGx.series[0].addPoint([x, y], true, true, true);";
  url += "} else {";
  url += "chartGx.series[0].addPoint([x, y], true, false, true);";
  url += "}}};";
  url += "xhttp.send();";
  url += "}, 10000 );";
  url += "var chartGy = new Highcharts.Chart({";
  url += "chart:{ renderTo : 'chart-gy' },";
  url += "title: { text: 'Gy' },";
  url += "series: [{";
  url += "showInLegend: false,";
  url += "data: []";
  url += "}],";
  url += "plotOptions: {";
  url += "line: { animation: false,";
  url += "dataLabels: { enabled: true }";
  url += "},";
  url += "series: { color: '#059e8a' }";
  url += "},";
  url += "xAxis: { type: 'datetime',";
  url += "dateTimeLabelFormats: { second: '%H:%M:%S' }";
  url += "},";
  url += "yAxis: {";
  url += "title: { text: 'Gy (rad)' }";
  url += "},";
  url += "credits: { enabled: false }";
  url += "});";
  url += "setInterval(function ( ) {";
  url += "var xhttp = new XMLHttpRequest();";
  url += "xhttp.open(\"GET\", \"/gy\", true);";
  url += "xhttp.onreadystatechange = function() {";
  url += "if (this.readyState == 4 && this.status == 200) {";
  url += "var myVar = this.responseText;";
  url += "var x = (new Date()).getTime() + (5*3600 + 45*60)*1000,";
  url += "y = parseFloat(myVar);";
  url += "if(chartGy.series[0].data.length > 10) {";
  url += "chartGy.series[0].addPoint([x, y], true, true, true);";
  url += "} else {";
  url += "chartGy.series[0].addPoint([x, y], true, false, true);";
  url += "}}};";
  url += "xhttp.send();";
  url += "}, 10000 );";
  url += "var chartGz = new Highcharts.Chart({";
  url += "chart:{ renderTo : 'chart-gz' },";
  url += "title: { text: 'Gz' },";
  url += "series: [{";
  url += "showInLegend: false,";
  url += "data: []";
  url += "}],";
  url += "plotOptions: {";
  url += "line: { animation: false,";
  url += "dataLabels: { enabled: true }";
  url += "},";
  url += "series: { color: '#059e8a' }";
  url += "},";
  url += "xAxis: { type: 'datetime',";
  url += "dateTimeLabelFormats: { second: '%H:%M:%S' }";
  url += "},";
  url += "yAxis: {";
  url += "title: { text: 'Gz (rad)' }";
  url += "},";
  url += "credits: { enabled: false }";
  url += "});";
  url += "setInterval(function ( ) {";
  url += "var xhttp = new XMLHttpRequest();";
  url += "xhttp.open(\"GET\", \"/gz\", true);";
  url += "xhttp.onreadystatechange = function() {";
  url += "if (this.readyState == 4 && this.status == 200) {";
  url += "var myVar = this.responseText;";
  url += "var x = (new Date()).getTime() + (5*3600 + 45*60)*1000,";
  url += "y = parseFloat(myVar);";
  url += "if(chartGz.series[0].data.length > 10) {";
  url += "chartGz.series[0].addPoint([x, y], true, true, true);";
  url += "} else {";
  url += "chartGz.series[0].addPoint([x, y], true, false, true);";
  url += "}}};";
  url += "xhttp.send();";
  url += "}, 10000 );";
  url += "</script>";
  url += "</html>";

  return url;
}
