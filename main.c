#define VOLTAGE_PIN A0
#define CURRENT_PIN A1
#define TEMP_PIN A2
#define CONTACTOR_PIN 13

const float MAX_VOLTAGE = 4.2;
const float MIN_VOLTAGE = 3.0;

// --- NEW BMS CAPACITY VARIABLES ---
const float TOTAL_CAPACITY_AH = 100.0; // A 100 Amp-hour battery pack
float currentCapacityAh = 100.0;       // Start fully charged
unsigned long previousTime = 0;        // To track delta time

void setup() {
  Serial.begin(9600);
  pinMode(CONTACTOR_PIN, OUTPUT);
  digitalWrite(CONTACTOR_PIN, HIGH); 
  
  // Initialize our timer
  previousTime = millis();
  Serial.println("BMS Boot Sequence Complete. Contactor CLOSED.");
}

void loop() {
  // 1. Read Raw Telemetry
  int rawVoltage = analogRead(VOLTAGE_PIN);
  int rawCurrent = analogRead(CURRENT_PIN);
  
  float actualVoltage = (rawVoltage / 1023.0) * 5.0;
  
  // Map raw 0-1023 to -50 Amps to +50 Amps
  float actualCurrent = ((rawCurrent / 1023.0) * 100.0) - 50.0; 

  // 2. THE MATH: Coulomb Counting (Integration)
  unsigned long currentTime = millis();
  // Calculate hours passed since last loop (millis -> seconds -> hours)
  float deltaTimeHours = (currentTime - previousTime) / 3600000.0; 
  previousTime = currentTime;

  // Add (or subtract) the Amp-hours used in this fraction of a second
  currentCapacityAh = currentCapacityAh + (actualCurrent * deltaTimeHours);
  
  // Calculate Percentage
  float stateOfCharge = (currentCapacityAh / TOTAL_CAPACITY_AH) * 100.0;

  // Prevent SoC from jumping over 100% or below 0%
  if (stateOfCharge > 100.0) stateOfCharge = 100.0;
  if (stateOfCharge < 0.0) stateOfCharge = 0.0;

  // 3. Print the Dashboard
  Serial.print("V: "); Serial.print(actualVoltage); 
  Serial.print("V | I: "); Serial.print(actualCurrent); 
  Serial.print("A | SoC: "); Serial.print(stateOfCharge);
  Serial.println("%");

  // 4. THE BOUNCER (Safety)
  if (actualVoltage > MAX_VOLTAGE || actualVoltage < MIN_VOLTAGE) {
    digitalWrite(CONTACTOR_PIN, LOW); 
    Serial.println("FAULT: VOLTAGE LIMIT EXCEEDED. CONTACTOR OPEN.");
  } else {
    digitalWrite(CONTACTOR_PIN, HIGH); 
  }

  delay(500); 
}
