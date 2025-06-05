import pygame
import serial
import time
import threading
import requests

arduino_port = 'COM9'  # **CHANGE THIS to your Arduino's serial port**
baud_rate = 115200
ip_address = "192.168.0.114"

def send_command(command):
    """Sends a command to the ip address by going to ip_address/command using threading so we don't block the main thread"""
    threading.Thread(target=send_command_thread, args=(command,)).start()

def send_command_thread(command):
    """Sends a command to the ip address by going to ip_address/command"""
    url = f"http://{ip_address}/{command}"
    response = requests.get(url)
    print(f"Sent command: {command}")
    print(f"Response: {response.text}")

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
        for event in pygame.event.get():
            if event.type == pygame.JOYBUTTONDOWN:
                if event.button in button_number_to_name:
                    button = button_number_to_name[event.button]
                    print(f"{button} pressed\n")
                    if button == "B":
                        # beep
                        send_command("beep on")
                else:
                    print(f"Button {event.button} pressed\n")
            elif event.type == pygame.JOYBUTTONUP:
                if event.button in button_number_to_name:
                    button = button_number_to_name[event.button]
                    print(f"{button} released\n")
                    if button == "B":
                        # beep off
                        send_command("beep off")
                else:
                    print(f"Button {event.button} released\n")
            elif event.type == pygame.JOYAXISMOTION:
                value = round(event.value)
                if event.axis in axis_to_direction: # if the button is on the d-pad...
                    button = axis_to_direction[event.axis]
                    if value == 1: # if the button is pressed...
                        print(f"{button} pressed\n")
                        # send_command("on")
                        if button == "UP":
                            send_command("moveForward")
                            # pass
                        elif button == "DOWN":
                            send_command("moveBackward")
                        elif button == "LEFT":
                            send_command("turnLeft")
                        elif button == "RIGHT":
                            send_command("turnRight")
                    elif value == -1: # if the button was released...
                        if not directional_button_pressed_before.get(button, False):
                            # ignore the first "button released" signal, it's false
                            directional_button_pressed_before[button] = True
                        else:
                            if button in ["UP", "DOWN", "LEFT", "RIGHT"]:
                                send_command("stopMotors") # Sending "off" for any directional button release
                                # send_command("off")
                            print(f"{button} released\n")
                else:
                    pass
                    # print(f"Axis {event.axis} moved to {value}")
except KeyboardInterrupt:
    print("Stopping input listener")
finally:
    pygame.quit()
