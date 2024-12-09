int temp = 0;
int humidity = 0;

void setup() 
{

    Serial.begin(115200);

    // Initialize temp/humidity sensor
    Environment.setPin(4);
    Environment.begin();
}

void loop()
{
    temp = Environment.readTemperature();
    humidity = Environment.readHumidity();

    Serial.print("Temperature="); Serial.print(temp); Serial.println(" degrees Celcius")
    Serial.print("Humidity="); Serial.println(humidity);
    Serial.println();
}