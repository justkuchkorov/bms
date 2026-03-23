// Define the hardware pins our sensors are plugged into
#define VOLTAGE_PIN A0
#define CURRENT_PIN A1
#define TEMP_PIN A2

void setup() {
  // Initialize the serial monitor so we can see the data output
  Serial.begin(9600);
  Serial.println("BMS Telemetry Initialized...");
}

void loop() {
  // 1. Read the raw ADC values from the hardware (0 to 1023)
  int rawVoltage = analogRead(VOLTAGE_PIN);
  int rawCurrent = analogRead(CURRENT_PIN);
  int rawTemp = analogRead(TEMP_PIN);

  // 2. Print the raw data to the Serial Monitor
  Serial.print("V_Raw: "); Serial.print(rawVoltage);
  Serial.print(" | I_Raw: "); Serial.print(rawCurrent);
  Serial.print(" | T_Raw: "); Serial.println(rawTemp);

  // Run this loop every 500 milliseconds
  delay(500);
}
