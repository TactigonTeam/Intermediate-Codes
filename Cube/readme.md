# Cube

The aim of this project is showing a 3D cube on a Smartphone which orientation in updated in real time depending on actual orientation of TSkin/Tactigon

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

## Structure
This project is composed by a Arduino sketch running on TSkin/Tactigon side and a Processing sketch running on a Android smartphone.

* Arduino sketch: [TactigonCube]() folder

* Processing sketch: [sketch_ble_cube]() folder

## Development

On Tactigon board side project has been developed with Arduino IDE v.1.8.9 configured with support for Tactigon board.
Refer to this link for Arduino API: https://www.thetactigon.com/arduino/doxygen/index.html

On Smartphone side project has been developed with Processing v.3.5.3 with support for Android mode
Tested on Android 8 running on Smartphone with Bluetooth version 4.1 – 5.0

## Testing and use

On Tactigon board side: just upload sketch and run it

On Smartphone side: 
- enable Bluetooth
- launch application: it continuously search for TSkin/Tactigon device, connect to it and display cube


## Built with

* [Procesing 3.5.3](http://download.processing.org/processing-3.5.3-windows64.zip) - Win64
* [Procesing 3.5.3](http://download.processing.org/processing-3.5.3-windows32.zip) - Win32