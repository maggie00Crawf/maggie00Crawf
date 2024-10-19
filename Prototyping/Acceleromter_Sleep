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


char data_char[10];


void loop(){
  sBmx160SensorData_t Omagn, Ogyro, Oaccel;

  bmx160.getAllData(&Omagn, &Ogyro, &Oaccel);

  Serial.print("#");
  Serial.print(Oaccel.x); Serial.print(" "); Serial.print(Oaccel.y    ); Serial.print(" "); Serial.print(Oaccel.z    ); 
  Serial.println("#");
  delay(50);

  Serial.println(data_format(Oaccel.x));

  delay(500);
}


//formats data into values with sign(+/-), then 000.00
String data_format(float data) {
  data_str = string(data);
  if (data_str.startsWith("-")) {
    data_char = data_str;
  } else {
      data_char = string("+" + string(data_str));
  }

  Serial.print("Sign changed: "); Serial.print(data_str);
  
  if ((data < 100) && (data > 10)) {
    data_str = "00" + String(data);
  } else if (data < 10) {
    data_str = "00" + String(data);
  } else {
    data_str = String(data);
  }


}
