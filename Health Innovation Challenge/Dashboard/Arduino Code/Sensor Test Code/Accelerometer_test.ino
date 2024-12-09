#include <DFRobot_BMX160.h>
DFRobot_BMX160 bmx160;

void setup(){
  Serial.begin(115200);
  delay(100);
  
  //init the hardware bmx160  
  if (bmx160.begin() != true){
    Serial.println("init false");
    while(1);
  }
  delay(100);
}

void loop(){
  //Gets all the sensor data
  sBmx160SensorData_t Omagn, Ogyro, Oaccel;
  bmx160.getAllData(&Omagn, &Ogyro, &Oaccel);

  //Print out the accelerometer raw x, y, and z values to Serial Monitor
  Serial.print("RAW: ");
  Serial.print(Oaccel.x); Serial.print(" "); Serial.print(Oaccel.y    ); Serial.print(" "); Serial.print(Oaccel.z    ); Serial.println(" m/s^2"); 
}
