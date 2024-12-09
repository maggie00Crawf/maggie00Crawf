!pip install pyserial
import serial  # for Serial communication 
 

arduino = serial.Serial('/dev/cu.usbmodem1101', 9600, timeout=1)  # Update with your port 

time.sleep(2)  # wait for 2 seconds for the communication to get established 

print("Connected") 
 

while (1): 

    if (arduino.inWaiting() > 0): 
        arduinoOut = arduino.readline().decode('utf-8').strip() 
         # if you are transmitting data from Arduino to Python via the format "<message>"
        if not str(arduinoOut).startswith("<"): 
            print(arduinoOut) 

 