##Environmental Monitoring Prototype to reduce risk of hospital induced delerium
##detects noise levels and light color+intensity
##Created By: Maggie Crawford
##Created On: Sept 26 2024
##Last Edited On: Oct 15 2024

###make sure libratries are installed
import serial  # for Serial communication
import time  # for delay functions

###Edit baudrate and port to match your arduino settings

arduino = serial.Serial()
arduino.baudrate = 9600
arduino.port = "com10"
arduino.open()

###Initializing values	  
NotEnd = True
Color_List = [12]
redVal, greenVal, blueVal, red_goal, green_goal, blue_goal, red_chosen, green_chosen, blue_chosen, red_show, green_show, blue_show = (-1,)*12
redint, blueint, greenint = (0, )*3
##Declare color "goal" values for each section of the day
##Morning warm light (yellow: 255, 251, 20)
red_goal_morning = 255
green_goal_morning = 251
blue_goal_morning = 20
##Afternoon bright light (blue: 202, 239, 252)
red_goal_afternoon = 202
green_goal_afternoon = 239
blue_goal_afternoon = 252
##Night Low light (red: 217, 4, 4)
red_goal_night = 217
green_goal_night = 4
blue_goal_night = 4

##formats RGB values in 3 digits 
def color_3digit (color):
		color = int(color)
		if ((color < 100) and (color > 10)):
			return f"0{str(color)}"
		elif (color < 10):
			return f"00{str(color)}"
		else:
			return str(color)

def constrain(n, minn, maxn):
	if n < minn:
		return minn
	elif n > maxn:
		return maxn
	else:
		return n


#########################loop to read arduino serial monitor as it prints################################
while NotEnd:
	time.sleep(0.5)
	##Read time
	st = time.localtime()
	time_hour = st[3]
	# print(time_hour)

	print("before in wating")
	if arduino.in_waiting > 0:
		##Read arduino Serial Monitor and split line into individual accessible values
		SerialMonitor = arduino.readline()
		# Data_List = SerialMonitor.split(", ")
		# print(Data_List)
		print(SerialMonitor)

		##Assign 1st sensor value between commas to Color Sensor Value
		if str(SerialMonitor).startswith("="):
			# Color_List = Data_List[0]
			redVal = int(SerialMonitor[1:4])
			greenVal = int(SerialMonitor[4:7])
			blueVal = int(SerialMonitor[7:10])
		else:
			redVal , greenVal , blueVal = -1 , -1, -1


		# if redVal >= 0:
		print("Color_List value: ",redVal, greenVal, blueVal)
		if(redVal.isdigit()):
			redint = int(redVal.strip())
		else:
			redVal = 0

		if(greenVal.isdigit):
			greenint = int(greenVal.strip())
		else:
			greenint = 0

		if(blueVal.isdigit):	
			blueint = int(blueVal.strip())
		else:
			blueint = 0
		print(Color_List)
		print("RED: ", redVal)

	time.sleep(0.5)
	##Determine light color due to time of day
	if (7 <= time_hour) and (10 >= time_hour):
		red_goal = red_goal_morning
		green_goal = green_goal_morning
		blue_goal = blue_goal_morning
		# print("good morning: " + str(time_hour))
	elif ((11 <= time_hour) and (17 >= time_hour)):
		red_goal = red_goal_afternoon
		green_goal = green_goal_afternoon
		blue_goal = blue_goal_afternoon
		# print("good afternoon: "  + str(time_hour))

	elif (time_hour != 0):
		red_goal = red_goal_night
		green_goal = green_goal_night
		blue_goal = blue_goal_night
		# print("good night: " + str(time_hour))


	####take difference between goal color and read color
	##When read value of color is 0, shown value will be the same as goal
	##when read value of color is 255, shown value will be the opposite color as the goal (to cancel out hue)
	if (redVal >= 0):	
		# print("GOAL: ", red_goal," ", green_goal, " ", blue_goal)
		# print("READ: ", redVal," ", greenVal, " ", blueVal)

		red_show = red_goal - redVal
		green_show = green_goal - greenVal
		blue_show = blue_goal - blueVal

		# print("RED GOAL: ", color_3digit(red_show))

		red_show = constrain(red_show, 0, 255)
		green_show = constrain(green_show, 0, 255)
		blue_show = constrain(blue_show, 0, 255)

		# print("SHOW: ", red_show, green_show, blue_show)
		##Send color values being shown on LEDs to arduino
		# print("[C:" + str(color_3digit(red_show)) + str(color_3digit(green_show))+ str(color_3digit(blue_show)) + "]")
		inputchoice = "[C:" + str(color_3digit(red_show)) + str(color_3digit(green_show))+ str(color_3digit(blue_show)) + "]"
		# inputtext = f"<{inputchoice}>"
		arduino.write(inputchoice.encode())
		print(inputchoice)
