#include <Arduino.h>

#define CO2_PIN 35


void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(CO2_PIN, INPUT);

  // Set the default voltage of the reference voltage
  //analogReadResolution(DEFAULT);

}

void loop()
{
  // Read voltage
  int sensor_value = analogRead(CO2_PIN);
  Serial.println(sensor_value);

  // The analog signal is converted to a voltage
  float voltage = sensor_value * (3300 / 4095.0);
  //   float voltage = sensor_value * (5000 / 1024.0);

  if (voltage == 0)
  {
    Serial.println("Fault");
  }
  else if (voltage < 400)
  {
    Serial.println("preheating");
  }
  else
  {
    int voltage_diference = voltage - 400;
    float concentration = (voltage_diference * 50.0) / 16.0;
    
    // Print Voltage and concentration
    Serial.println("[X] Voltage: " + String(voltage) + " mv  ||  Concentration: " + String(concentration) + " ppm.");
 
  }
  delay(15000);
}
