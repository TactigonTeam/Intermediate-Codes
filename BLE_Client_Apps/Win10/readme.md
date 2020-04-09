# BLE_Client_Apps (Windows 10)

Windows BLE support is available only from Windows 10.

Since this version BLE library are available in C#, however their use is sometimes difficult. Also Microsoft official examples are too big and structured to easily get a straightforward example.
This project is an example of an easy way to connect and read data from BLE interface using C# language

## Structure

The project is a VisualStudio 2017 Solution available in [AppBleLib]() folder.
Solution includes two project:

* a library that provides class to connect, and register on notifications to a BLE characteristics and rise callback on new data event

* a very simple app that use the library to search a device and get data from two characteristics

Splitting app / library lets to easily port all the BLE handling procedures included in the library in a more complex application

## Testing and use

On Tactigon board side: just upload [sketch]() and run it.

On the C# side of the code the app use the BLE library to:

* continuously search for device named “TACTI”
* register on notification on following characteristics:

	ACCELEROMETER_CHARACTERISTIC = "bea5760d-503d-4920-b000-101e7306b007";
       EULERO_CHARACTERISTIC = "bea5760d-503d-4920-b000-101e7306b009";

* show real time data in simple text box

### Prerequisites
* Universal Windows Platform development enabled for your Visual Studio installation
* Developer Mode enabled (needed to deploy the VS solution, follow this [MS article](https://docs.microsoft.com/en-us/windows/uwp/get-started/enable-your-device-for-development) on how to do it)

## Built with

* Visual Studio 2017 (any newer releases works just fine)