def decode_can_frame(raw_log_line):
    """
    Takes a raw string from the BMS serial monitor and extracts the physical data.
    Expected format: "... [CAN BUS OUT] ID: 0x620 DATA: 50 64 00 00 00 00 00 00"
    """
    if "DATA:" not in raw_log_line:
        return None, None
    
    # Spliting the string and grab the hex bytes
    payload = raw_log_line.split("DATA:")[1].strip().split(" ")
    
    # Byte 0 is our Voltage, Byte 1 is our SoC
    v_hex = payload[0]
    soc_hex = payload[1]
    
    # 2. Converting the Hex strings back to base-10 Decimal integers
    v_dec = int(v_hex, 16)
    soc_dec = int(soc_hex, 16)
    
    # 3. REVERSING THE C MATH
    # C Code: v_byte = (actualVoltage / 5.0) * 255
    # Python: actualVoltage = (v_dec / 255.0) * 5.0
    actual_voltage = (v_dec / 255.0) * 5.0
    
    # SoC was a 1:1 mapping, so it's just the decimal number
    actual_soc = soc_dec
    
    return actual_voltage, actual_soc

# ---------------------------------------------------------
# THE SIMULATION TEST
# ---------------------------------------------------------

test_logs = [
    "[CAN BUS OUT] ID: 0x620 DATA: 50 64 00 00 00 00 00 00", # 1.57V, 100%
    "[CAN BUS OUT] ID: 0x620 DATA: 4A 64 00 00 00 00 00 00", # 1.47V, 100%
    "[CAN BUS OUT] ID: 0x620 DATA: 65 64 00 00 00 00 00 00", # 1.99V, 100%
    "[CAN BUS OUT] ID: 0x620 DATA: 88 64 00 00 00 00 00 00"  # 2.68V, 100%
]

print("="*50)
print(" 🔋 BMS CAN BUS TELEMETRY DECODER INITIALIZED")
print("="*50)

for log in test_logs:
    voltage, soc = decode_can_frame(log)
    
    # Format the output to look like a clean terminal dashboard
    hex_string = log.split("DATA:")[1].strip()
    print(f"📥 RX Hex: [{hex_string}]  -->  📊 V: {voltage:.2f}V | SoC: {soc}%")

print("="*50)
