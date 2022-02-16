'''
 TELEPHONE RINGER BY MakeItHackin
 FOR MORE INFORMATION, VISIT: https://github.com/MakeItHackin/TelephoneRinger
 Version: 1.1 with Optional Display
'''
# DISPLAY CODE PROVIDED BY ADAFRUIT
# SPDX-FileCopyrightText: 2021 ladyada for Adafruit Industries
# SPDX-License-Identifier: MIT

useOLEDDisplay = True  #set true for using a SSD1306 OLED Display

import board
import time
import digitalio

if (useOLEDDisplay == True):
    import displayio
    import terminalio
    from adafruit_display_text import label
    import adafruit_displayio_ssd1306

print("Starting...")


#SETTING UP PIN ASSIGNMENTS AND DIRECTIONS.  TESTED ON ADAFRUIT FEATHER RP2040.  YOU MAY NEED TO CHANGE THESE PINS.

#MOTOR CONTROLLER
in1 = digitalio.DigitalInOut(board.D13)
in1.direction = digitalio.Direction.OUTPUT
in2 = digitalio.DigitalInOut(board.D12)
in2.direction = digitalio.Direction.OUTPUT

#LEDS
led1 = digitalio.DigitalInOut(board.D11)
led1.direction = digitalio.Direction.OUTPUT
led2 = digitalio.DigitalInOut(board.D10)
led2.direction = digitalio.Direction.OUTPUT

#BUTTONS
button_A = digitalio.DigitalInOut(board.D9)
button_A.direction = digitalio.Direction.INPUT
button_A.pull = digitalio.Pull.UP

button_B = digitalio.DigitalInOut(board.D6)
button_B.direction = digitalio.Direction.INPUT
button_B.pull = digitalio.Pull.UP

button_C = digitalio.DigitalInOut(board.D5)
button_C.direction = digitalio.Direction.INPUT
button_C.pull = digitalio.Pull.UP

# LOOP COUNT FOR AMERICAN RINGER
# HIGHER VALUE INCREASES RING DURATION
americanLoopCount = 30

# LOOP DELAY FOR AMERICAN RINGER
# A VALUE OF 25 WILL PRODUCE 20 HERTZ.  YOU PROBABLY DON'T NEED TO CHANGE THIS VALUE. SOME PHONES WILL NOT RING WITH VALUES BEYOND A COUPLE HERTZ.
americanDelay = .026

# LOOP COUNT FOR BRITISH RINGER
# HIGHER VALUE INCREASES RING DURATION
britishLoopCount = 10

# LOOP DELAY FOR BRITISH RINGER
# THE DEFAULT VALUE OF .019 IS PRETTY LOW AND MAY NOT WORK ON ALL PHONES 
britishDelay = .019

# DELAY BETWEEN RINGS FOR BRITISH RINGER
# FEEL FREE TO EXPERIMENT WITH THIS VALUE
britishDelayBetweenRings = .2

# WHEN A USER PRESSES BUTTONS THESE SCREENS WILL APPEAR
def displayButtonPress(button):  
    if (button == 'a' or button == 'A'):
        print("BUTTON A PRESSED!")
    elif (button == 'b' or button == 'B'):
        print("BUTTON B PRESSED!")
    elif (button == 'c' or button == 'C'):
        print("BUTTON C PRESSED!")

    if (useOLEDDisplay == True):
        bg_sprite = displayio.TileGrid(color_bitmap, pixel_shader=color_palette, x=0, y=0)
        splash.append(bg_sprite)
        if (button == 'a' or button == 'A'):
            textA = "BUTTON A PRESSED\nAMERICAN RING!"
        elif (button == 'b' or button == 'B'):
            textA = "BUTTON B PRESSED\nBRITISH RING!"
        elif (button == 'c' or button == 'C'):
            textA = "BUTTON C PRESSED\nSINGLE RING!"
        text_areaA = label.Label(terminalio.FONT, text=textA, color=0xFFFF00, background_color = 0x000000, x=0, y=5, line_spacing = 1, scale = 1)
        splash.append(text_areaA)
        display.show(splash)
    return

'''
    THIS FUNCTION IS USED TO RING THE TELEPHONE USING GIVEN PARAMETERS
    ringCount: This is how many times you want the phone to ring
    ringDelay: This is the amount of milliseconds you want between each ring
    loopCount: This is how long the phone should ring during one ring.
    loopDelay: This corresponds to the frequency of the ring.  You'll need to stay around 25 +- 6
    britishBoolean: true for "British" style ring or false for "American" style
    ledAssignment: 1 for LED1 or 2 for LED2
'''
def ringTelephone(ringCount, ringDelay, loopCount, loopDelay, britishBoolean, ledAssignment):
    iteration = 1
    print("ringCount:", ringCount,"- ringDelay:", ringDelay,"- loopCount:",loopCount,"- loopDelay:",loopDelay,"- britishBoolean:",britishBoolean,"- ledAssignment:",ledAssignment)
    for i in range(ringCount):
        if (ledAssignment == 1):
            led1.value = True
        else:
            led2.value = True
        if (britishBoolean == True):
            iteration = 2 # "British" rings are two quick rings, so setting up a loop to ring twice
        for j in range(iteration): # this for loop is for the american or british ring styles
            for x in range(loopCount): #this for loop is for the duration of the ring.
                in1.value = True
                in2.value = False
                time.sleep(loopDelay)
                in1.value = False
                in2.value = True
                time.sleep(loopDelay)
            if (britishBoolean == True):
                time.sleep(britishDelayBetweenRings) # for the british ring, delay a little bit before the next quick ring
        if (ledAssignment == 1): # turn off the led at the end of each ring
            led1.value = False
        else:
            led2.value = False
        time.sleep(ringDelay)
    time.sleep(.1)
    print("done ringing")
    return

# THIS MENU WILL BE DISPLAYED WHEN USER IS NOT PRESSING BUTTONS
if (useOLEDDisplay == True): 
    def displayMenu():
        text1 = "BUTTON A - AMERICAN\nBUTTON B - BRITISH\nBUTTON C - SINGLE"
        text_area1 = label.Label(terminalio.FONT, text=text1, color=0xFFFF00, background_color = 0x000000, x=0, y=5, line_spacing = 0.8, scale = 1)
        splash.append(text_area1)
        display.show(splash)
        return

# SETTING UP THE DISPLAY
if (useOLEDDisplay == True):    
    displayio.release_displays()
    i2c = board.I2C()
    display_bus = displayio.I2CDisplay(i2c, device_address=0x3C)
    display = adafruit_displayio_ssd1306.SSD1306(display_bus, width=128, height=32)
    # Make the display context
    splash = displayio.Group()
    display.show(splash)
    color_bitmap = displayio.Bitmap(128, 32, 1)
    color_palette = displayio.Palette(1)
    color_palette[0] = 0xFFFFFF  # White
    color_palette[0] = 0x000000 # Black
    bg_sprite = displayio.TileGrid(color_bitmap, pixel_shader=color_palette, x=0, y=0)
    splash.append(bg_sprite)
    displayMenu()

# MAIN FUNCTION HERE.  WHEN THE USER PRESSES A BUTTON, THE EVENTS ARE PRINTED, PHONE RINGS, AND DISPLAY IS UPDATED 
while True:
    if (button_A.value == False):
        displayButtonPress('a')
        ringTelephone(2,1,30,.026,False,1) # 'American Ring'
        if (useOLEDDisplay == True):
            displayMenu()
    elif (button_B.value == False):
        displayButtonPress('b')
        ringTelephone(3,1,10,.019,True,2) # 'British Ring'
        if (useOLEDDisplay == True):
            displayMenu()
    elif (button_C.value == False):
        displayButtonPress('c')
        ringTelephone(1,0,4,.026,False,2) # 'Single Ring'
        if (useOLEDDisplay == True):
            displayMenu()
    else:
        led1.value = False # turn off LEDs
        led2.value = False

