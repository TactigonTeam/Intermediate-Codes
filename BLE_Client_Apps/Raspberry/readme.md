# BLE_Client_Apps (Raspberry Pi)

This is a python example running on a Raspberry PI3.

## Getting started

It shows how:

* continuously scan for target BLE device in order to recover from disconnection
* connect to target device and register on NOTIFICATION on target characteristic
* unpack acceleration data coming from Tactigon board

It creates a Thread for every connected device so it can be extended to connect to multiple
Tactigon board at the same time.

### Prerequisites

* Python 2.7
* [BlueZ library](https://learn.adafruit.com/install-bluez-on-the-raspberry-pi/installation) - BlueZ library with support for BLE
* [Bluepy library 1.2.0](https://github.com/IanHarvey/bluepy) - Use v1.2.0, newer versions causes troubles with internal functions... 
