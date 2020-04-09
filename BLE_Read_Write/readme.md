# BLE_Read_Write

This example shows how add a writeable characteristic on the BLE interface (in addition to usual streaming characteristics).

This could be useful to send commands to Tactigon board via BLE.

## Testing

The example simply echo on debug serial what received on BLE
 
 You can test this for example using free Android app "BLE Scanner".

 Connect to Tactigon board, search for "bea5760d-503d-4920-b000-101e7306b001"
 char under CUSTOM SERVICE.
 
 This char should have "W" (writeable) icon.
 
 Click on it and write something, you should see echo on debug serial.
 Write something in Text format to see something of human readable on debug serial.