#define VOLTAGE_PIN A0
#define CURRENT_PIN A1
#define TEMP_PIN A2
#define CONTACTOR_PIN 13 // Our simulated safety relay

// Hardcoded Lithium-Ion Safety Limits
const float MAX_VOLTAGE = 4.2;
const float MIN_VOLTAGE = 3.0;

void setup() {
  Serial.begin(9600);
  pinMode(CONTACTOR_PIN, OUTPUT);
  
  // Start with the contactor CLOSED (LED ON) assuming the battery is safe
  digitalWrite(CONTACTOR_PIN, HIGH); 
  Serial.println("BMS Telemetry & Safety Initialized...");
}

void loop() {
  // 1. Read raw ADC data
  int rawVoltage = analogRead(VOLTAGE_PIN);
  
  // 2. Convert raw ADC (0-1023) to Actual Voltage (0.0V - 5.0V)
  float actualVoltage = (rawVoltage / 1023.0) * 5.0;

  Serial.print("Battery Voltage: "); 
  Serial.print(actualVoltage); 
  Serial.println(" V");

  // 3. THE SAFETY LOOP (The Bouncer)
  if (actualVoltage > MAX_VOLTAGE) {
    digitalWrite(CONTACTOR_PIN, LOW); // CUT THE POWER!
    Serial.println("CRITICAL FAULT: OVER-VOLTAGE DETECTED!");
  } 
  else if (actualVoltage < MIN_VOLTAGE) {
    digitalWrite(CONTACTOR_PIN, LOW); // CUT THE POWER!
    Serial.println("CRITICAL FAULT: UNDER-VOLTAGE DETECTED!");
  } 
  else {
    // Voltage is safe, keep contactor closed
    digitalWrite(CONTACTOR_PIN, HIGH); 
  }

  delay(500);
}
