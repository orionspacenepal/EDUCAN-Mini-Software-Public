void setup() {

  // put your setup code here, to run once:
pinMode(A0, INPUT);
Serial.begin(9600);

}

void loop() {

int sensorValue=analogRead(A0);
float voltage= 2*sensorValue * (4.2 / 1023.0);
Serial.print("Voltage:");
Serial.println(voltage);
delay(1000);
}
