'''
Author: Hardit Sabharwal
Date: 18th Oct , 2024
Project: Health Hub -Arduino Sensor Solution
'''

import dash
from dash import html, dcc
from dash.dependencies import Input, Output, State
import plotly.graph_objs as go
import dash_daq as daq
import dash_bootstrap_components as dbc
import serial
import time
import sensorHelper as sh
import datetime


# Set up serial communication for both Arduinos
arduino_john_connected = False
arduino_jane_connected = False

try:
    ser_john = serial.Serial('/dev/cu.usbmodem101', 9600, timeout=1)  # John Doe's Arduino
    time.sleep(2)  # Give some time for the connection to be established
    arduino_john_connected = True
except serial.SerialException:
    arduino_john_connected = False

try:
    ser_jane = serial.Serial('/dev/cu.usbmodem1101', 9600, timeout=1)  # Jane Doe's Arduino
    time.sleep(2)
    arduino_jane_connected = True
except serial.SerialException:
    arduino_jane_connected = False

# External stylesheets and meta tags
external_stylesheets = [dbc.themes.BOOTSTRAP]

app = dash.Dash(__name__, external_stylesheets=external_stylesheets)
server = app.server

# Layout
app.layout = html.Div([
    dcc.Interval(id='update_value', interval=1 * 1000, n_intervals=0),

    # Patient selector
    html.Div([
        dcc.Dropdown(
            id='patient_selector',
            options=[
                {'label': 'John Doe', 'value': 'john'},
                {'label': 'Jane Doe', 'value': 'jane'}
            ],
            value='john',  # Default selected patient
            clearable=False,
            style={'width': '50%'}
        ),
    ], className='dropdown-container'),

    # Header and title
    html.Div([
        html.Div([
            html.Img(src=app.get_asset_url('sensor.png'), style={'height': '30px'}, className='title_image'),
            html.H1('Patient Dashboard', className='title_text')
        ], className='header-row'),

        html.H6('Room : E7-2112', className='location'),
    ], className='bg_title'),

    # Bento grid for displaying data
    html.Div(className='bento-grid', children=[
        html.Div(className='bento-widget', children=[
            html.H3('Heart Rate (bpm)', className='bento-title'),
            dcc.Graph(id='heart_rate_graph', config={'displayModeBar': False}),
        ], style={'gridColumn': 'span 3'}),  # Make this widget span two columns

        html.Div(className='bento-widget', children=[
            html.H3('Oxygen Level (%)', className='bento-title'),
            dcc.Graph(id='oxygen_graph', config={'displayModeBar': False}),
        ], style={'gridColumn': 'span 2'}),  # Make this widget span two columns

        html.Div(className='bento-widget', children=[
            html.H3('Room Humidity (%)', className='bento-title'),
            dcc.Graph(id='humidity_graph', config={'displayModeBar': False}),
        ], style={'gridColumn': 'span 2'}),  # Use graph for humidity

        html.Div(className='bento-widget', children=[
            html.H3('Room Temperature (°C)', className='bento-title'),
            daq.Thermometer(
                id='temp_thermometer',
                min=0,
                max=50,
                value=0,
                style={'width': '100%'}
            )
        ], style={'gridColumn': 'span 1'}),



        html.Div(className='bento-widget', children=[
            html.H3('Room Noise Level (dB)', className='bento-title'),
            daq.Gauge(
                id='noise_gauge',
                showCurrentValue=True,
                units='dB',
                value=0,
                min=0,
                max=1000,
                style={'width': '100%'}
            )
        ], style={'gridColumn': 'span 2'}),

        html.Div(className='bento-widget', children=[
            html.H3('Room Light Level (Lux)', className='bento-title'),
            dcc.Graph(id='light_graph', config={'displayModeBar': False}),
        ], style={'gridColumn': 'span 2'})  # Add light back
    ])
], className='container-fluid')


# Function to read data from Arduino
def read_arduino_data(serial_conn):
    hR, o2 = None, None
    soundVal = None
    tempVal , humidityVal = None , None
    lux = None
    if serial_conn.in_waiting > 0:
        try:
            arduino_data = serial_conn.readline().decode('utf-8').strip()
            data = sh.parseArduinojson(arduino_data)

            if data:
                print(data)
                soundVal = data["sound"]
                tempVal = data["temperature"]
                humidityVal = data["humidity"]
                lux = data["lux"]
                hR , o2 = sh.parse_heart_value(data["oximeter"])
                rgbCol = data["rgbSensor"]
                ledOutput = sh.colorCalculation(rgbCol)
                serial_conn.write(ledOutput.encode())

            output = [hR , o2 , tempVal ,humidityVal,soundVal , lux]
            return output
            # Expecting: heart_rate, oxygen, temperature, humidity, noise, light
        except:
            return None  # Handle any errors
    return None  # If no data is available


# Callback to update gauges and charts based on the selected patient
@app.callback(
    [Output('heart_rate_graph', 'figure'),
     Output('oxygen_graph', 'figure'),
     Output('humidity_graph', 'figure'),
     Output('temp_thermometer', 'value'),
     Output('noise_gauge', 'value'),
     Output('light_graph', 'figure')],
    [Input('update_value', 'n_intervals'),
     Input('patient_selector', 'value')],
    [State('heart_rate_graph', 'figure'),
     State('oxygen_graph', 'figure'),
     State('humidity_graph', 'figure'),
     State('temp_thermometer', 'value'),
     State('noise_gauge', 'value'),  # State for noise gauge
     State('light_graph', 'figure')]
)
def update_patient_data(n_intervals, selected_patient, heart_rate_fig, oxygen_fig, humidity_fig,prev_temparture, prev_noise, light_fig):
    if selected_patient == 'john':
        if arduino_john_connected:
            print("JOHNY")
            data = read_arduino_data(ser_john)
        else:
            data = [None] * 6  # Return None for all values

    elif selected_patient == 'jane':
        if arduino_jane_connected:
            data = read_arduino_data(ser_jane)
        else:
            data = [None] * 6  # Return None for all values

    prev_heart_rate = heart_rate_fig['data'][0]['y'][-1] if heart_rate_fig and heart_rate_fig['data'][0]['y'] else 0
    prev_oxygen = oxygen_fig['data'][0]['value'] if oxygen_fig and 'value' in oxygen_fig['data'][0] else 0
    prev_humidity = humidity_fig['data'][0]['y'][-1] if humidity_fig and humidity_fig['data'][0]['y'] else 0
    prev_light = light_fig['data'][0]['y'][-1] if light_fig and light_fig['data'][0]['y'] else 0

    if data:
        heart_rate, oxygen, temperature, humidity, noise, light = data
    else:
        heart_rate, oxygen, temperature, humidity, noise, light = None, None, None, None, None, None

    heart_rate = heart_rate if heart_rate is not None else prev_heart_rate
    humidity = humidity if humidity is not None else prev_humidity
    light = light if light is not None else prev_light
    oxygen = oxygen if oxygen is not None else prev_oxygen
    noise = float(noise) if noise is not None else float(prev_noise)  # Use previous noise value if None
    temperature = float(temperature) if temperature is not None else float(prev_temparture)  # Use previous noise value if None



        # Handle previous data
    if heart_rate_fig is not None:
        x_existing_hr = heart_rate_fig['data'][0]['x']
        y_existing_hr = heart_rate_fig['data'][0]['y']
    else:
        x_existing_hr = []
        y_existing_hr = []

        # Append the new data
    x_updated_hr = x_existing_hr[-100:] + [n_intervals]
    y_updated_hr = y_existing_hr[-100:] + [heart_rate if heart_rate is not None else 0]

    heart_rate_fig = {
        'data': [go.Scatter(
            x=x_updated_hr,  # Append new intervals to x-values
            y=y_updated_hr,  # Append new heart rate values to y-values
            mode='lines+markers',
            name='Heart Rate',
            line = dict(color='red')
        )],
        'layout': go.Layout(
            title='HR Over Time',
            xaxis=dict(title='Time Interval'),
            yaxis=dict(title='HR (bpm)'),
            margin=dict(t=40, b=0, l=40, r=40),
            height=250
        )
    }

    oxygen_fig = {
        'data': [go.Indicator(
            mode="number",
            value=oxygen if oxygen is not None else 0,
            number={'suffix': " %"},
            title={'text': "Oxygen Level"},
            domain={'x': [0, 1], 'y': [0, 1]}
        )],
        'layout': go.Layout(
            margin=dict(t=0, b=0, l=0, r=0),
            height=250
        )
    }

     # Handle previous data
    if humidity_fig is not None:
        x_existing_hum = humidity_fig['data'][0]['x']
        y_existing_hum = humidity_fig['data'][0]['y']
    else:
        x_existing_hum = []
        y_existing_hum = []

        # Append the new data
    x_updated_hum = x_existing_hum[-100:] + [n_intervals]
    y_updated_hum = y_existing_hum[-100:] + [humidity if humidity is not None else 0]

    humidity_fig = {
        'data': [go.Scatter(
            x=x_updated_hum,  # Append new intervals to x-values
            y=y_updated_hum,  # Append new heart rate values to y-values
            mode='lines+markers',
            name='Heart Rate'
        )],
        'layout': go.Layout(
            title='Humidity Over Time',
            xaxis=dict(title='Time Interval'),
            yaxis=dict(title='%'),
            margin=dict(t=40, b=0, l=40, r=40),
            height=250
        )
    }

     # Handle previous data
    if light_fig is not None:
        x_existing_light = light_fig['data'][0]['x']
        y_existing_light = light_fig['data'][0]['y']
    else:
        x_existing_light = []
        y_existing_light = []

        # Append the new data
    x_updated_light = x_existing_light[-100:] + [n_intervals]
    y_updated_light = y_existing_light[-100:] + [light if light is not None else 0]

    light_fig = {
        'data': [go.Scatter(
            x=x_updated_light,  # Append new intervals to x-values
            y=y_updated_light,  # Append new heart rate values to y-values
            mode='lines+markers',
            name='Heart Rate',
            line = dict(color='yellow')

    )],
        'layout': go.Layout(
            title='LUX Level',
            xaxis=dict(title='Time Interval'),
            yaxis=dict(title='%'),
            margin=dict(t=40, b=0, l=40, r=40),
            height=250
        )
    }

    return heart_rate_fig, oxygen_fig, humidity_fig, temperature if temperature is not None else 0, noise, light_fig


if __name__ == '__main__':
    app.run_server(debug=True)
