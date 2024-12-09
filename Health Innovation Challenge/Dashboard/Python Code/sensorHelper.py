'''
Author: Hardit Sabharwal , Maggie Crawford
Date: 18th Oct , 2024
Project: Health Hub -Arduino Sensor Solution

This file is for creating and storing all the individual sensor helper functions.
1. IMU Sensor   2. Noise Sensor 3. Light Sensor 4. Pulse Oximeter
5. LEDs 6. Temp/Humiidity   7. Display
'''

import json
import ast
import time

# For Sleep State Calculation// Shift to Index
x, y, z = 0, 0, 0
z_angle_avg = []
State_of_sleep = []
State = None

def parse_heart_value(data):

  values = data.split(',')

  # Convert to integers
  flag_hr, hr_value, flag_o2, o2_value = int(values[0]), int(values[1]), int(values[2]), int(values[3])

  # Check if HR and O2 are valid based on their flags
  hr_result = hr_value if flag_hr == 1 else None
  o2_result = o2_value if flag_o2 == 1 else None

  return hr_result, o2_result

def parseArduinojson(string):

    if str(string).startswith("{"):
        output = json.loads(string)
        return output
    else:
        output = None

def outputStringToDisplay(string , arduino):

    outputString = f"<{string}>"
    try:
        arduino.write(outputString.encode())
    except:
        return

def outputColorToDisplay(colorHex , arduino):

    outputString = f"<+{colorHex}>"
    try:
        arduino.write(outputString.encode())
    except:
        return

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


def colorCalculation(color):
    red_goal_morning , green_goal_morning ,blue_goal_morning = 255 , 251 , 20
    red_goal_afternoon , green_goal_afternoon , blue_goal_afternoon = 202 , 239 , 252
    red_goal_night , green_goal_night , blue_goal_night = 217 , 4 ,4

    redVal = int(color[0:3])
    greenVal = int(color[3:6])
    blueVal = int(color[6:9])

    st = time.localtime()
    time_hour = st[3]

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

    else:
        red_goal = red_goal_night
        green_goal = green_goal_night
        blue_goal = blue_goal_night

    #if we have a color detected
    if redVal >= 0:

        red_show = red_goal - redVal
        green_show = green_goal - greenVal
        blue_show = blue_goal - blueVal

        red_show = constrain(red_show, 0, 255)
        green_show = constrain(green_show, 0, 255)
        blue_show = constrain(blue_show, 0, 255)

        ledInput = f"<C{color_3digit(red_show)}{color_3digit(green_show)}{color_3digit(blue_show)}>"
        return ledInput


    "<outputfrompython>"

# Sleep Functions
def read_serial():
	with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1) as ser:
		time.sleep(0.5)  # Wait for the connection to establish
		try:
			line = ser.readline().decode('utf-8').strip()
			if line:
				# Parse the JSON string
				data = json.loads(line)
				x = data.get("x")
				y = data.get("y")
				z = data.get("z")
				# print(f"x: {x}, y: {y}, z: {z}")
		except json.JSONDecodeError:
			print("Received non-JSON data:", line)
	return x, y, z

def calculate_angle():
	x=read_serial()[0]
	y=read_serial()[1]
	z=read_serial()[2]
	# print(x, "-->", x*x, " ", y, "-->", y*y)
	denominator = float(x*x + y*y)
	# print("Denominator: ", denominator)
	if denominator == 0:
		return 0  # Avoid division by zero
	angle_rad = math.atan(z / denominator)  # Calculate the angle in radians
	angle_calc = math.degrees(angle_rad)      # Convert to degrees
	# print("Angle RAD: ", angle_rad)
	return float(angle_calc)


def calculate_average(values):
	return sum(values) / len(values) if values else 0

def five_sec_average(): 
	values = []
	start_time = time.time()
	
	while True:
		# Read the angle value
		value = calculate_angle()
		values.append(value)
		# print(f"Current Average Angle={value:.2f}")
		
		# Allow new 5-sec average to be added
		if time.time() - start_time >= 5:
			average = calculate_average(values)
			z_angle_avg.append(average)
			print(f"Average angle over 5 seconds: {average:.2f}")
			break  # Exit the loop after calculating the average
		
		time.sleep(0.1)  # Adjust sampling rate as needed

##call function using Identify_sleep_state(z_angle_avg)##
def Identify_sleep_state(angle_input_list):
	five_sec_average()
	threshold_angle = 5
	threshold_time = 10
	last_angle = angle_input_list[-1]
	last_time_fluctuating = time.time()
	fluctuating = False
	threshold_print_statment = time.time()
	State = None

	while True:
		five_sec_average()
		current_angle = angle_input_list[-1]
		
		print(f"Change in angle: {current_angle - last_angle}")
		if abs(current_angle - last_angle) > threshold_angle:
			fluctuating = True
			last_time_fluctuating = time.time()
			State_of_sleep.append("awake")
			State = "awake"
		else:
			if fluctuating and (time.time() - last_time_fluctuating) >= threshold_time:
				fluctuating = False
				State_of_sleep.append("asleep")
				State = "asleep"
		
		last_angle = current_angle
		
		# Outputs Sleep State once per threshold time period
		if (time.time() - threshold_print_statment >= threshold_time):
			# outputString = f"<S{State}>"
			# print(outputString)
			threshold_print_statment = time.time()
			print()

		time.sleep(0.1)
		return State
