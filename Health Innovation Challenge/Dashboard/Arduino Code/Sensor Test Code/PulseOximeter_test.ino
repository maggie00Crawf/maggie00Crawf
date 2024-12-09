#include <DFRobot_MAX30102.h>
DFRobot_MAX30102 particleSensor;


    // heart rate variables
    int32_t SPO2;           //SPO2
    int8_t SPO2Valid;       //Flag to display if SPO2 calculation is valid
    int32_t heartRate;      //Heart-rate
    int8_t heartRateValid;  //Flag to display if heart-rate calculation is valid


void setup() {
    Serial.begin(115200);

  if (!particleSensor.begin()) {
    Serial.println("MAX30102 was not found");
  }

  particleSensor.sensorConfiguration(/*ledBrightness=*/150, /*sampleAverage=*/SAMPLEAVG_8,
                                     /*ledMode=*/MODE_MULTILED, /*sampleRate=*/SAMPLERATE_200,
                                     /*pulseWidth=*/PULSEWIDTH_411, /*adcRange=*/ADCRANGE_16384);
}

void calculateHR() {
    particleSensor.heartrateAndOxygenSaturation(/**SPO2=*/&SPO2, /**SPO2Valid=*/&SPO2Valid, /**heartRate=*/&heartRate, /**heartRateValid=*/&heartRateValid);
}

void loop()
{
  Serial.print("HR detected: "); Serial.println(heartRateValid); // 1=True, 0=False
  Serial.print("HR: "); Serial.println(heartRate);
  Serial.print("SPO2 detected: "); Serial.println(SPO2Valid); // 1=True, 0=False
  Serial.print("SPO2: "); Serial.println(SPO2);
  delay(100);
}