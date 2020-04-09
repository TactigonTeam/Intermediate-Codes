from bluepy.btle import Scanner, DefaultDelegate, Peripheral, AssignedNumbers, BTLEException
import threading
import binascii
import sys
import json
import logging
from ConfigParser import SafeConfigParser
from struct import *
import pygame
import time
import numpy as np
import math
import numpy

# init target device
# Only connect to devices advertising this name
DEVICE_TO_FIND = "TACTI"
# acceleration characteristic
TARGET_CHARACTERISTIC = "bea5760d-503d-4920-b000-101e7306b007"


#
logging.basicConfig(level=logging.DEBUG,
                    format='[%(levelname)s] (%(threadName)-10s) %(message)s')


def log_it(*args):
    msg = " ".join([str(a) for a in args])
    logging.debug(msg)

#
# delegate used to process data coming from BLE characteristic
#
class MyDelegate(DefaultDelegate):

    def __init__(self, addr):
        DefaultDelegate.__init__(self)
        self.id = addr

    # Called by BluePy when an event was received.
    def handleNotification(self, cHandle, data):

        # expected data: timestamp_unused (2) | acc.x(4) | acc.y(4) | acc.z(4)

        # getacc x,y,z from characteristic
        ax = unpack('f', data[2:6])[0]
        ay = unpack('f', data[6:10])[0]
        az = unpack('f', data[10:14])[0]

        logging.info("Received notification: acc: %f %f %f", ax, ay, az)

#
# Thread handling a single Tactigon board
#
class BleThread(Peripheral, threading.Thread):
    # @var WAIT_TIME
    # Time of waiting for notifications in seconds
    WAIT_TIME = 0.1

    # @var EXCEPTION_WAIT_TIME
    # Time of waiting after an exception has been raiesed or connection lost
    EXCEPTION_WAIT_TIME = 10

    # register on NOTIFICATION on this characteristic
    txUUID = TARGET_CHARACTERISTIC

    def __init__(self, peripheral_addr):
        try:
            Peripheral.__init__(self, peripheral_addr)
        except Exception, e:
            logging.warning("Problem initializing Peripheral %s", e.message)
            raise

        threading.Thread.__init__(self)

        # Set up our WRITE characteristic
        try:
            self.txh = self.getCharacteristics(uuid=self.txUUID)[0]
        except Exception, e:
            logging.debug(
                "Problem getting characteristics from %s. %s", self.addr, e.message)

        # Create the BluePy objects for this node
        self.delegate = MyDelegate(peripheral_addr)
        self.withDelegate(self.delegate)
        self.connected = True

        logging.info("Configuring RX to notify me on change")
        try:
            # notify us on characteristic change
            handle = self.txh.getHandle()
            self.writeCharacteristic(handle+1, b"\x01\x00", withResponse=True)
        except Exception, e:
            logging.debug(
                "Problem subscribing to RX notifications: %s", e.message)

    def run(self):
        while self.connected:
            try:
                self.waitForNotifications(self.WAIT_TIME)

            except BaseException, be:
                logging.debug("BaseException caught: %s", be.message)
                self.connected = False

            except BTLEException, te:
                logging.debug("BTLEException caught for %s. %s",
                              self.addr, te.message)
                if str(te.message) == 'Device disconnected':
                    logging.debug("Device disconnected: %s", self.addr)
                    self.connected = False
                    # We don't want to call waitForNotifications and fail too often
                    time.sleep(self.EXCEPTION_WAIT_TIME)
                else:
                    raise

            except Exception, e:
                logging.debug("Peripheral exception for %s. %s",
                              self.addr, e.message)
                self.connected = False



#######################################################################################
# APP STARTS HERE
logging.info("Looking for devices named: %s", DEVICE_TO_FIND)


# Initialize Peripheral scanner
scanner = Scanner(0)

# always scan for Tactigon BLE devices
done = False
while not done:
    devices = scanner.scan(2)
    for d in devices:
        try:
            # If scan returns a known addr that's already in the collection, it means it disconnected
            # Remove record and treat it as new

            for (adtype, desc, value) in d.getScanData():

                # search for Tactigon devices
                if(value.find(DEVICE_TO_FIND) == 0):

                    # start thread
                    t = BleThread(d.addr)
                    logging.info("Starting Thread for %s", d.addr)
                    t.daemon = True
                    t.start()

        except Exception, e:
            logging.debug("Unknown error %s", e.message)