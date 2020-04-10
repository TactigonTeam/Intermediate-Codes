import RPi.GPIO as GPIO
import struct
import binascii
import bluepy
from bluepy import btle

# GPIO SETTINGS AND CONFIGURATIONS
GPIO.setwarnings(False)
GPIO.setmode(GPIO.BOARD)
channelRelay = [7, 11, 13, 15]
GPIO.setup(channelRelay, GPIO.OUT, initial=GPIO.LOW)
configA = [channelRelay[1], channelRelay[3]]
configB = [channelRelay[0], channelRelay[2]]

# DELEGATE CLASS FOR NOTIFICATION


class MyDelegate(btle.DefaultDelegate):
    def __init__(self):
        btle.DefaultDelegate.__init__(self)

    # MESSAGE PARSING AND GPIO CONTROL
        def handleNotification(self, cHandle, data):
            print("handleNotification()")
            val = binascii.b2a_hex(data)
            val = binascii.unhexlify(val)
            val = struct.unpack('>H', val)[0]
            if(val == 0x5A5A):
                GPIO.output(configB, GPIO.HIGH)
                GPIO.output(configA, GPIO.LOW)
                print("Configuration B ON")
            elif(val == 0x0088):
                GPIO.output(configA, GPIO.HIGH)
                GPIO.output(configB, GPIO.LOW)
                print("Configuration A ON")
            elif(val == 0x00BB):
                GPIO.output(channelRelay, GPIO.HIGH)
                print("All ON")
            elif(val == 0x1515):
                GPIO.output(channelRelay, GPIO.LOW)
                print("All OFF")


# BLE DEVICE SETTINGS
tactigon_uuid = btle.UUID("7ac71000-503d-4920-b000-acc000000001")
service_uuid = btle.UUID("bea5760d-503d-4920-b000-101e7306b000")
p = btle.Peripheral("BE:A5:7F:2F:76:68")
p.setDelegate(MyDelegate())

# CHARACTERISTICS LIST
tactigonService = p.getServiceByUUID(service_uuid)
# for ch in tactigonService.getCharacteristics():
# print str(ch)

# OBTAIN DATA FROM CHARACTERISTIC
tactigonSensorValue = tactigonService.getCharacteristics(tactigon_uuid)[0]
val = tactigonSensorValue.read()
# print "Characteristic value: ", binascii.b2a_hex(val)

# SET NOTIFICATION
notifyTactigon = tactigonService.getDescriptors(forUUID=0x2902)[0]
notifyTactigon.write(struct.pack('<bb', 0x01, 0x00))

# WAIT FOR NOTIFICATIONS
try:
    while True:
        if p.waitForNotifications(1.0):
            continue
except:
    print("Terminated")