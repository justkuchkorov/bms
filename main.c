#define VOLTAGE_PIN A0
#define CURRENT_PIN A1
#define TEMP_PIN A2
#define CONTACTOR_PIN 13

// Hardcoded Safety Limits
const float MAX_VOLTAGE = 4.2;
const float MIN_VOLTAGE = 3.0;

// BMS Logic Variables
const float TOTAL_CAPACITY_AH = 100.0; 
float currentCapacityAh = 100;       
unsigned long previousTime = 0;        

// --- CAN BUS SIMULATION SETTINGS ---
#define BMS_CAN_ID 0x620  // Standard ID for Battery Data

void setup() {
  Serial.begin(9600);
  pinMode(CONTACTOR_PIN, OUTPUT);
  digitalWrite(CONTACTOR_PIN, HIGH); 
  
  previousTime = millis();
  Serial.println("BMS Boot Sequence Complete. Contactor CLOSED.");
}

// Function to simulate broadcasting data over a vehicle network
void sendCANFrame(float v, float soc) {
  // Scale 0-5V to 0-255 (1 byte)
  uint8_t v_byte = (uint8_t)((v / 5.0) * 255);
  // Scale 0-100% to 0-100 (1 byte)
  uint8_t soc_byte = (uint8_t)soc;

  Serial.print("  [CAN BUS OUT] ID: 0x");
  Serial.print(BMS_CAN_ID, HEX);
  Serial.print(" DATA: ");
  
  // Print Hexadecimal representation of the battery state
  if(v_byte < 16) Serial.print("0"); Serial.print(v_byte, HEX); Serial.print(" ");
  if(soc_byte < 16) Serial.print("0"); Serial.print(soc_byte, HEX); 
  Serial.println(" 00 00 00 00 00 00");
}

void loop() {
  // 1. Telemetry Reading
  int rawVoltage = analogRead(VOLTAGE_PIN);
  int rawCurrent = analogRead(CURRENT_PIN);
  float actualVoltage = (rawVoltage / 1023.0) * 5.0;
  float actualCurrent = ((rawCurrent / 1023.0) * 100.0) - 50.0; 

  // 2. Integration Math (Coulomb Counting)
  unsigned long currentTime = millis();
  float deltaTimeHours = (currentTime - previousTime) / 3600000.0; 
  previousTime = currentTime;
  currentCapacityAh += (actualCurrent * deltaTimeHours);
  
  float stateOfCharge = (currentCapacityAh / TOTAL_CAPACITY_AH) * 100.0;
  if (stateOfCharge > 100.0) stateOfCharge = 100.0;
  if (stateOfCharge < 0.0) stateOfCharge = 0.0;

  // 3. Print Local Dashboard
  Serial.print("V: "); Serial.print(actualVoltage); 
  Serial.print("V | I: "); Serial.print(actualCurrent); 
  Serial.print("A | SoC: "); Serial.print(stateOfCharge);
  Serial.print("%");

  // 4. Network Broadcast (The "Voice")
  sendCANFrame(actualVoltage, stateOfCharge);

  // 5. Safety Bouncer
  if (actualVoltage > MAX_VOLTAGE || actualVoltage < MIN_VOLTAGE) {
    digitalWrite(CONTACTOR_PIN, LOW); 
    Serial.println("CRITICAL FAULT: VOLTAGE LIMIT EXCEEDED!");
  } else {
    digitalWrite(CONTACTOR_PIN, HIGH); 
  }

  delay(1000); // 1Hz Broadcast rate is standard for BMS
}
