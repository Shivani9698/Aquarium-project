#define TdsSensorPin 27

void setup() {
    Serial.begin(115200);
    pinMode(TdsSensorPin, INPUT);
}
void loop() {
    int analogValue = analogRead(TdsSensorPin);  // Read the analog value from the TDS sensor
    
    if (analogValue > 10) {  // Ignore very low readings
        float tdsValue = 0.3101 * analogValue + 15.12;
        Serial.print("Analog Value: ");
        Serial.println(analogValue);
        Serial.print("TDS: ");
        Serial.println(tdsValue);
    } else {
        Serial.println("Sensor is dry or not connected.");
    }

    delay(1000);  // Delay 1 second before reading the next value
}
