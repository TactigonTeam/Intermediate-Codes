import blepdroid.*;
import blepdroid.BlepdroidDevice;
import com.lannbox.rfduinotest.*;
import android.os.Bundle;
import android.content.Context;
import java.util.UUID;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import android.util.Log;




Blepdroid blepdroid;

public static String TARGET_DEVCE = "TACTI";
public static String TACTIGON_QUAT_CHAR_STRING = "bea5760d-503d-4920-b000-101e7306b005";
public static UUID TACTIGON_QUAT_UUID = UUID.fromString(TACTIGON_QUAT_CHAR_STRING);


int scanMillis = 0;

float q0, q1, q2, q3;
float roll, pitch, yaw;
float[][] m;
quaternion qq;



////////////////////////////////////////////////////////////////////////
void setup() 
{
  //size(width, height, P3D); 
  fullScreen(P3D);
  smooth();
  println(" OK ");

  //create ble handling class
  blepdroid = new Blepdroid(this);
}



////////////////////////////////////////////////////////////////////////
void draw() 
{
    //background
    background(10);
    
    
    ////stamp quaternions values  
    textSize(32);
    textAlign(LEFT);
    
    text("q0 ", 130, 60);     text(q0, 200, 60);       text("roll ", 480, 60);     text(roll, 570, 60);  
    fill(255,0,0);
    text("q1 ", 130, 100);    text(q1, 200, 100);      text("pitch ", 480, 100);     text(pitch, 570, 100); 
    fill(0,255,0);
    text("q2 ", 130, 140);    text(q2, 200, 140);      text("yaw ", 480, 140);     text(yaw, 570, 140); 
    fill(255,0,255);
    text("q3 ", 130, 180);    text(q3, 200, 180);
    
    
       
    ////cube plot
    
    //positioning
    translate(width/2, height/2, 0);
    
    //compute rotation matrix
    quaternion qq = new quaternion(q0,q1,q2,q3);
    m = qq.orthogonalMatrix();

    // Matrix for rotation around the Y axis
    applyMatrix(m[0][0], m[0][1], m[0][2], 0,
            m[1][0], m[1][1], m[1][2], 0,
            m[2][0], m[2][1], m[2][2], 0,
            0, 0, 0, 1);
    
    
    //draw cube
    fill(255,228,225);
    box(300);                     
    //fill(255);
    
    
    //continuos scan to recover from disconnection
    scanForDevices();
}





//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void scanForDevices()
{    
  if(millis() >= (scanMillis + 2000))
  {    
    scanMillis = millis();
    
    println(" scan !");
    blepdroid.scanDevices();
  }  
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public float[] getQuatFloat(byte[] data)
{
  float[] q = new float[4];
  byte[] q_stream = new byte[4]; 

  for(int i=0; i<4; i++)
  {
    System.arraycopy(data, 2 + 4*i, q_stream, 0, 4);
    q[i] = ByteBuffer.wrap(q_stream).order(ByteOrder.LITTLE_ENDIAN).getFloat();
  }
  
  return q;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void onDeviceDiscovered(BlepdroidDevice device)
{
  println("discovered device " + device.name + " address: " + device.address + " rssi: " + device.rssi );

  if (device.name != null && device.name.equals(TARGET_DEVCE))
  {
    println(TARGET_DEVCE + " Found");
    
    if (blepdroid.connectDevice(device))
    {
      println(" connected to " + TARGET_DEVCE);      
    } else
    {
      println(" couldn't connect device ");
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
    
    // this will list the UUIDs of each service, in the future we're going to make
    // this tell you more about each characteristic, e.g. whether it's readable or writable
    //println( servicesAndCharas.get(service));
    
    for(String charact: servicesAndCharas.get(service))
    {
      print( " charact: " + charact);
      if(charact.equals(TACTIGON_QUAT_CHAR_STRING))
      {        
        print( " target characteristic found! ");
        blepdroid.setCharacteristicToListen(device, TACTIGON_QUAT_UUID);
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
  //  | timeStamp(2) | q0(2) | q1(2) | q2(2) | q3(2) | acc1(2) | acc2(2) | acc3(2) |
  
  
  // get each set of two bytes
  //plotAcc(data);
  //plotQuat(data);
  
  //float q0, q1, q2, q3;
  float[] q;
  float t0,t1,t2,t3,t4,ysqr;
  
  q = getQuatFloat(data);
  
  ////////////
  q0 = q[0];
  q1 = q[1];
  q2 = q[2];
  q3 = q[3];
  
  ysqr = q2*q2;
  
  //roll (x-axis rotation)
  t0 = 2 * (q0*q1 + q2*q3);
  t1 = 1 - 2 * (q1*q1 + ysqr);
  roll = atan2(t1, t0);
  
  //pitch (y-axis rotation)
  t2 = 2 * (q0*q2 - q3*q1);
  if(t2 > 1) 
    t2 = 1; 
  if(t2 < -1) 
    t2 = -1;
  pitch = asin(t2);
  
  //yaw (z-axis rotation)
  t3 = 2 * (q0*q3 + q1*q2);
  t4 = 1 - 2 * (ysqr + q3*q3);
  yaw = atan2(t4, t3);
  
  
  //convert to degree
  roll = roll * 360/6.28;
  pitch = pitch * 360/6.28;
  yaw = yaw * 360/6.28;
  
  //String dataString = new String(data);
  //println(" onCharacteristicChanged " + characteristic + " " + dataString  );
  
  Log.v("EULER", String.format("roll, pitch, yaw [deg]: %f, %f, %f", roll, pitch, yaw));
  Log.v("QUATERNIONS", String.format("q0, q1, q2, q3: %f, %f, %f", q0, q1, q2, q3));
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
