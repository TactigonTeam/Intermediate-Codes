import blepdroid.*;
import blepdroid.BlepdroidDevice;
import com.lannbox.rfduinotest.*;
import android.os.Bundle;
import android.content.Context;
import java.util.UUID;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;



//BLE handler class from blepdroid library
Blepdroid blepdroid;


String[] DEVICES = {"TACTI"};
String[] DEVICES_CHAR_TO_READ = {"bea5760d-503d-4920-b000-101e7306b007"};



int scanMillis = 0;




///////////////////////////////////////////////////////////////////////////////////////////
void setup() 
{
  fullScreen();
  smooth();
  println(" OK ");

  //create BLE class
  blepdroid = new Blepdroid(this);
  
  //init GUI for device 1
  demo1_init();
}


///////////////////////////////////////////////////////////////////////////////////////////
void draw() 
{

  background(250, 250, 250);
    
    
  //plot GUI for device 1
  demo1_plot();
    
  
  //continously search for device in order to recover from disconnection
  scanForDevices();
}





//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void scanForDevices()
{  
  //scan BLE interfaces every 2 sec
  if(millis() >= (scanMillis + 2000))
  {    
    scanMillis = millis();
    
    println(" scan !");
    blepdroid.scanDevices();
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void onDeviceDiscovered(BlepdroidDevice device)
{
  println("discovered device " + device.name + " address: " + device.address + " rssi: " + device.rssi );

  for(int i=0; i<DEVICES.length; i++)
  {
    String dev = DEVICES[i];
    
    if (device.name != null && device.name.equals(dev))
    {
      println(dev + " Found");
      
      if (blepdroid.connectDevice(device))
      {
        println(" connected to " + dev);      
      } else
      {
        println(" couldn't connect device " + dev);
      }
    } 
  }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void onServicesDiscovered(BlepdroidDevice device, int status)
{
  
  HashMap<String, ArrayList<String>> servicesAndCharas = blepdroid.findAllServicesCharacteristics(device);
  
  for( String service : servicesAndCharas.keySet())
  {
    print( service + " has " );
    
    
    //check if discovered device is in target device list
    int i;
    for(i=0; i<DEVICES.length; i++)
    {
      if(device.name.equals(DEVICES[i]))
        break;
    }
    
    
    //if discovered device is in terget device list check if 
    //it provides related target characteristic
    if(i < DEVICES.length)
    {
      String charString = DEVICES_CHAR_TO_READ[i];
      
      for(String charact: servicesAndCharas.get(service))
      {
        print( " charact: " + charact);
        if(charact.equals(charString))
        {
          //target characteristic found: register to NOTIFICATION
          print( " target characteristic found! ");
          blepdroid.setCharacteristicToListen(device, UUID.fromString(charString));
        }
      }
    }
    
  }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// these are all the BLE callbacks
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void onBluetoothRSSI(BlepdroidDevice device, int rssi)
{
  println(" onBluetoothRSSI " + device.address + " " + Integer.toString(rssi));
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void onBluetoothConnection( BlepdroidDevice device, int state)
{
  blepdroid.discoverServices(device);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void onCharacteristicChanged(BlepdroidDevice device, String characteristic, byte[] data)
{  
  //every new characteristic fresh data I get her: handle with device callback
  if(device.name.equals(DEVICES[0]))
  {
    //data callback for device 1
    demo1_CB(data);    
  }
  
  
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void onDescriptorWrite(BlepdroidDevice device, String characteristic, String data)
{
  println(" onDescriptorWrite " + characteristic + " " + data);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void onDescriptorRead(BlepdroidDevice device, String characteristic, String data)
{
  println(" onDescriptorRead " + characteristic + " " + data);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void onCharacteristicRead(BlepdroidDevice device, String characteristic, byte[] data)
{
  println(" onCharacteristicRead " + characteristic + " " + data);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void onCharacteristicWrite(BlepdroidDevice device, String characteristic, byte[] data)
{
  println(" onCharacteristicWrite " + characteristic + " " + data);
}
