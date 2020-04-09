# BLE_Central_Mode

In this example Tactigon board is CENTRAL on BLE interface.

Setting target MAC and characteristic UUID, Tactigon continuously 
scan the BLE interface for this target and connect to it.

In this example the target is a Adafruit BLE-serial adapter.

Tactigon board writes Euler angles in ascii format so that, connecting Adafruit adapter to a PC, it is possible get angles in a human readable
format on a standard serial terminal.

Max single write is 20 byte.