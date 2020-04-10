# BLE_Client_Apps (Android)

This is a very simple Android application written with Processing to communicate with the Tactigon board.

## Getting started

This app does:

1) search for "TACTI" device on BLE interface and connect to it

2) register on NOTIFICATION on characteristic with UUID = "bea5760d-503d-4920-b000-101e7306b007"
   On this characteristic acceleration data are streamed (refer to Tactigon [example code]())

3) unpack the binary data in this characteristic

4) plot x,y,z acceleration data in a very simple graph


This sketch can be easily modified to plot data on the other characteristics

Also it is possible extend it in order to connect to more Tactigon board at the same time


### Prerequisites

* [blepdroid library](https://github.com/joshuajnoble/blepdroid) - The BLE library used 

## Built with

* [Procesing 3.5.3](http://download.processing.org/processing-3.5.3-windows64.zip) - Win64
* [Procesing 3.5.3](http://download.processing.org/processing-3.5.3-windows32.zip) - Win32
