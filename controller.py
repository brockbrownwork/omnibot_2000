import pygame

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
                if event.axis in axis_to_direction:
                    button = axis_to_direction[event.axis]
                    if value == 1:
                        print(f"{button} pressed\n")
                    elif value == -1:
                        if not directional_button_pressed_before[button]:
                            # ignore the first "button released" signal, it's false
                            directional_button_pressed_before[button] = True
                        else:
                            print(f"{button} released\n")
                else:
                    pass
                    # print(f"Axis {event.axis} moved to {value}")
except KeyboardInterrupt:
    print("Stopping input listener")
finally:
    pygame.quit()
