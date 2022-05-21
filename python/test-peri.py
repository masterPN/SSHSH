from practicum import find_mcu_boards, McuBoard, PeriBoard
from time import sleep

import paho.mqtt.client as mqtt

devs = find_mcu_boards()

if len(devs) == 0:
    print("*** No practicum board found.")
    exit(1)

mcu = McuBoard(devs[0])
print("*** Practicum board found")
print("*** Manufacturer: %s" % \
        mcu.handle.getString(mcu.device.iManufacturer, 256))
print("*** Product: %s" % \
        mcu.handle.getString(mcu.device.iProduct, 256))
peri = PeriBoard(mcu)

file1 = open("command.txt","w")
file1.write("none")
file1.close()
# This is the Subscriber
isOnTimer = False
secure = False
auto = False
timer = 0
RED = 0
ORANGE = 1
GREEN = 2
BLUE = 3

def on_connect(client, userdata, flags, rc):
    client.subscribe("projectsshsh")

def on_message(client, userdata, msg):
    command = msg.payload.decode()
    print(command)
    file2 = open("command.txt","w")
    file2.write(command)
    file2.close()

client = mqtt.Client()
client.connect("test.mosquitto.org",1883,60)

client.on_connect = on_connect
client.on_message = on_message

client.loop_start()

while True:
    print("==================")
    file1 = open("command.txt","r")
    command = file1.read()
    sw = peri.get_switch()
    light = peri.get_light()
    if (command == "security on"):
        secure = True
    if (command == "security off"):
        secure = False
    if (command == "auto on"):
        auto = True
    if (command == "auto off"):
        auto = False
    if (command.isdigit() and isOnTimer is False):
        timer = int(command)
    if (command == "NaN"):
        isOnTimer = False
        timer = 0
    print(light)
    print(timer)
    #Check Security
    if (secure):
        peri.set_led(RED, 1)
        peri.set_led(GREEN, 0)
        print("Security On")
    else:
        peri.set_led(RED, 0)
        peri.set_led(GREEN, 1)
    #Alert
    if (secure and sw):
        peri.set_led(ORANGE, 1)
        peri.set_buzzer(1)
        print("Alert")
    else:
        peri.set_led(ORANGE, 0)
        peri.set_buzzer(0)
    #Timer
    if (isOnTimer and timer > 0):
        timer -= 1
        if timer == 0:
            peri.set_led(BLUE, 0)
            isOnTimer = False
            file1 = open("command.txt","w")
            file1.write("none")
            file1.close()
    #Get Timer
    if isOnTimer is False:
        if timer != 0:
            auto = False
            isOnTimer = True
            peri.set_led(BLUE, 1)
            #print("On Timer")

    #Auto On/Off
    if (light >= 500 and auto):
        peri.set_led(BLUE, 1)
    elif (light < 500 and auto):
        peri.set_led(BLUE, 0)
    sleep(1.0)
