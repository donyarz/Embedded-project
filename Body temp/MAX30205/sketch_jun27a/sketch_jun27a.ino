#include <Wire.h>

#define MAX30205_ADDRESS 0x49 // Update to the correct I2C address found by the scanner
#define TEMPERATURE_REGISTER 0x00

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // Initialize I2C with SDA on GPIO 21 and SCL on GPIO 22
}

void loop() {
  float temperature = readTemperature();
  
  // Check if temperature is -273.15 °C (absolute zero)
  if (temperature != -273.15) {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");
  }

  delay(1000); // Wait for 1 second before reading again
}

float readTemperature() {
  Wire.beginTransmission(MAX30205_ADDRESS);
  Wire.write(TEMPERATURE_REGISTER);
  if (Wire.endTransmission() != 0) {
    return -273.15; // Return absolute zero if error
  }

  Wire.requestFrom(MAX30205_ADDRESS, 2);
  if (Wire.available() == 2) {
    byte msb = Wire.read();
    byte lsb = Wire.read();

    int16_t rawTemperature = (msb << 8) | lsb;
    return rawTemperature * 0.00390625; // Conversion factor for 16-bit to Celsius
  } else {
    return -273.15; // Return absolute zero if error
  }
}
