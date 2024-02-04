#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
void setup()
{
   Serial.begin(115200);
    if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085/BMP180 sensor, check wiring!");
    while (1);
    }

}
void loop()
{
    double Altitude = bmp.readAltitude();
    double Pressure = bmp.readPressure();
    double Temperature = bmp.readTemperature();
    String packet = "BMP:";
    packet += Altitude;
    packet += " m in , ";
    packet += Pressure;
    packet += " hPa pressure &, ";
    packet += Temperature;
    packet += "°C";
    Serial.println(packet);
}