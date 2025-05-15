import pygame
import serial
import time

arduino_port = 'COM9'  # **CHANGE THIS to your Arduino's serial port**
baud_rate = 115200

def send_command(ser, command):
    """Sends a string command to the Arduino followed by a newline."""
    ser.write((command + '\n').encode('utf-8'))
    print(f"Sent command: {command}")

def read_response(ser):
    """Reads and returns a line (until newline) from the Arduino."""
    # Wait a short time for data to arrive
    time.sleep(0.1)
    if ser.in_waiting > 0:
        response = ser.readline().decode('utf-8').rstrip()
        return response
    return None


try:
    ser = serial.Serial(arduino_port, baud_rate, timeout=1)
    time.sleep(2) # Wait for the Arduino to reset and for the serial port to open

    # Wait for the Arduino to signal readiness
    print("Waiting for Arduino...")
    while True:
        response = read_response(ser)
        if response == "Arduino Ready":
            print("Arduino is ready.")
            break
        elif response is not None:
            print(f"Unexpected response during startup: {response}")
        time.sleep(0.1)
except serial.SerialException as e:
    print(f"Serial error: {e}")
except Exception as e:
    print(f"An error occurred: {e}")

# Initialize pygame
pygame.init()
pygame.joystick.init()

# Check if a joystick is connected
if pygame.joystick.get_count() > 0:
    joystick = pygame.joystick.Joystick(0)
    joystick.init()
    print(f"Gamepad detected: {joystick.get_name()}")

button_number_to_name = {
    0:"A",
    1:"B",
    2:"X",
    3:"Y",
    4:"SELECT",
    6:"START",
    9:"L",
    10:"R"
}

axis_to_direction = {
    12:"UP",
    13:"DOWN",
    14:"LEFT",
    15:"RIGHT"
}

# this is a workaround for the fact that for some reason, the first time you press a directional button it sends "released" then "pressed"... it's fine
directional_button_pressed_before = {
    "UP": False,
    "DOWN": False,
    "LEFT": False,
    "RIGHT": False
}

# Main loop to detect input
try:
    while True:
        response = read_response(ser)
        if response and response != "None":
            print("Response:")
            print(response)
        for event in pygame.event.get():
            if event.type == pygame.JOYBUTTONDOWN:
                if event.button in button_number_to_name:
                    button = button_number_to_name[event.button]
                    print(f"{button} pressed\n")
                else:
                    print(f"Button {event.button} pressed\n")
            elif event.type == pygame.JOYBUTTONUP:
                if event.button in button_number_to_name:
                    button = button_number_to_name[event.button]
                    print(f"{button} released\n")
                else:
                    print(f"Button {event.button} released\n")
            elif event.type == pygame.JOYAXISMOTION:
                value = round(event.value)
                if event.axis in axis_to_direction: # if the button is on the d-pad...
                    button = axis_to_direction[event.axis]
                    if value == 1: # if the button is pressed...
                        print(f"{button} pressed\n")
                        if button == "UP":
                            # send_command(ser, "FOO")
                            send_command(ser, "on")
                    elif value == -1: # if the button was released...
                        if not directional_button_pressed_before[button]:
                            # ignore the first "button released" signal, it's false
                            directional_button_pressed_before[button] = True
                        else:
                            if button == "UP":
                                send_command(ser, "off")
                            print(f"{button} released\n")
                else:
                    pass
                    # print(f"Axis {event.axis} moved to {value}")
except KeyboardInterrupt:
    print("Stopping input listener")
finally:
    pygame.quit()
