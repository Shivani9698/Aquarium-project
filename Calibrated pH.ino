#define SensorPin 27           // pH meter Analog output to Arduino Analog Input
#define LED 13
#define samplingInterval 20
#define printInterval 800
#define ArrayLenth  10         // Reduced times of collection for faster response

int pHArray[ArrayLenth];       // Store the average value of the sensor feedback
int pHArrayIndex = 0;
float referenceVoltage = 3.3;  // Set to 3.3V for your board

// Adjusted intercept to correct for the 0.2 pH error
float slope = 3.6;             // Slope of the line (adjust this to fine-tune)
float intercept = -4.70;       // Increased by 0.2 to correct the pH error

void setup(void)
{
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  Serial.println("pH meter experiment!");    // Test the serial monitor
}

void loop(void)
{
  static unsigned long samplingTime = millis();
  static unsigned long printTime = millis();
  static float pHValue, voltage;

  if (millis() - samplingTime > samplingInterval)
  {
    pHArray[pHArrayIndex++] = analogRead(SensorPin);
    if (pHArrayIndex == ArrayLenth) pHArrayIndex = 0;

    voltage = averagearray(pHArray, ArrayLenth) * referenceVoltage / 1024.0; // Adjust for 3.3V reference voltage
    pHValue = slope * voltage + intercept; // Corrected formula
    samplingTime = millis();
  }

  if (millis() - printTime > printInterval)   // Every 800 milliseconds, print a numerical value
  {
    Serial.print("Voltage: ");
    Serial.print(voltage, 2);
    Serial.print("    pH value: ");
    Serial.println(pHValue, 2);
    digitalWrite(LED, digitalRead(LED) ^ 1);
    printTime = millis();
  }
}

double averagearray(int* arr, int number)
{
  long sum = 0;
  for (int i = 0; i < number; i++) {
    sum += arr[i];
  }
  return (double)sum / number;
}
