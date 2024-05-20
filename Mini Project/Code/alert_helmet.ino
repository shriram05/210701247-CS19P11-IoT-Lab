#include <TinyGPS++.h>
#include <SoftwareSerial.h>

const float VOLTAGE_AT_ZERO_PRESSURE = 0.5;  
const float VOLTAGE_PER_UNIT_PRESSURE = 0.1; 

const float FALL_THRESHOLD_PRESSURE = 42.0; 

TinyGPSPlus gps;

SoftwareSerial gsmModule(5, 4); 
SoftwareSerial gpsSerial(2, 3); 

float calculatePressure(int analogReading) {  
    float voltage = analogReading * (5.0 / 1023.0);
    Serial.print("Voltage reading: ");
    Serial.println(voltage);
    float pressure = (voltage - VOLTAGE_AT_ZERO_PRESSURE) / VOLTAGE_PER_UNIT_PRESSURE;
    return pressure;
}

void sendMessage(String phoneNumber, String message) {
    Serial.println("Sending a message to: " + phoneNumber);
    gsmModule.println("AT+CMGF=1"); 
    delay(1000);
    gsmModule.print("AT+CMGS=\"");
    gsmModule.print(phoneNumber); 
    gsmModule.println("\"");
    delay(1000);
    gsmModule.print(message); 
    delay(100);
    gsmModule.println((char)26); 
    delay(5000); 
    Serial.println("Message sent.");
}

void setup() {
    Serial.begin(9600);
    gsmModule.begin(9600);
    delay(1000);
    Serial.println("Initializing GSM module...");
    gsmModule.write("AT\r\n");
    delay(1000);
    gsmModule.write("ATE0\r\n");
    delay(1000);
    Serial.println("Initializing GPS module...");
    gpsSerial.begin(9600);
    Serial.println("GPS module initialized.");
    pinMode(A0, INPUT);
}

void loop() {
    int sensorValue = analogRead(A0);
    float pressure = calculatePressure(sensorValue);
    Serial.print("Pressure: ");
    Serial.println(pressure);
    while (gpsSerial.available() > 0) {
        gps.encode(gpsSerial.read());
    }
    if (pressure >= FALL_THRESHOLD_PRESSURE) {
        Serial.println("Helmet has fallen!");
        delay(1000);
        if (gps.location.isValid()) {
            double latitude = gps.location.lat();
            double longitude = gps.location.lng();
            String message = "Helmet has fallen! Location: Latitude " + String(latitude, 6) + ", Longitude " + String(longitude, 6);
            sendMessage("+919444569083", message);
        } else {
            sendMessage("+919444569083", "Helmet has fallen! Location data not available.");
        }
        delay(2000);
    }
    delay(1000); 
}

