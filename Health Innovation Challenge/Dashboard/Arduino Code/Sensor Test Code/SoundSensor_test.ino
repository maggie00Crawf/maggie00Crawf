const int pinSound = A0; // Analog pin of Sound Sensor 

void setup(){ 
  Serial.begin(9600); 
  delay(100); 
} 

int SoundVal = 0;


void loop()
{ 
SoundVal = analogRead(pinSound); 
Serial.println(SoundVal); 
delay(100); 
} 