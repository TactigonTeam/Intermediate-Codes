
#include <tactigon_led.h>
#include <tactigon_IMU.h>
#include <tactigon_BLE.h>

/*
 * 
 * get IMU data and expose them on differernt BLE characteristics
 * 
 */



T_Led rLed, bLed, gLed;

T_ACC accMeter;
T_AccData accData;

T_GYRO gyroMeter;
T_GyroData gyroData;

T_QUAT qMeter;
T_QData qData;

T_BLE bleManager;
UUID uuid;
T_BLE_Characteristic accChar, gyroChar, eChar, qChar;







/*----------------------------------------------------------------------*/
void setup() 
{
  
  rLed.init(T_Led::RED);
  gLed.init(T_Led::GREEN);
  bLed.init(T_Led::BLUE);

  rLed.off();
  gLed.off();
  bLed.off();

  //init BLE name
  bleManager.setName("TACTI");

  //init BLE role  
  bleManager.InitRole(TACTIGON_BLE_PERIPHERAL);

  //add needed characteristic passing as parameter the size (in bytes) 
  //of the characteristic
  //Every characteristic is identified by a unique identifier UUID
  
  uuid.set("bea5760d-503d-4920-b000-101e7306b007");   //acc
  accChar = bleManager.addNewChar(uuid, 14);
  
  uuid.set("bea5760d-503d-4920-b000-101e7306b008");   //gyro
  gyroChar = bleManager.addNewChar(uuid, 14);
  
  uuid.set("bea5760d-503d-4920-b000-101e7306b009");   //eulero angles as float
  eChar = bleManager.addNewChar(uuid, 14);
  
  uuid.set("bea5760d-503d-4920-b000-101e7306b005");   //quaternions as float (used by Cube app)
  qChar = bleManager.addNewChar(uuid, 18);


  //disable use of magnetometer in quat computing
  qMeter.enable();
  qMeter.useMag(0);
}



/*----------------------------------------------------------------------*/
void loop() 
{
  static int ticks = 0;
  unsigned char buffData[24];  

  //update BLE characteristics @ 50Hz (20 millisec)
  if(millis() >= (ticks + 20))
  {    
    ticks = millis();
                       
    //get IMU data
    //refer to API page https://www.thetactigon.com/arduino/doxygen/annotated.html
    //for data format
    //
    accData = accMeter.getAxis();			  //accelerations data
    gyroData = gyroMeter.getAxis();			//angular speed data
    qData = qMeter.getQs();             //quaternions and Eulero Angles data

    //update acc characteristic (acc values are float):   timestamp-unused (2bytes) | acc.x (4bytes) | acc.y (4bytes) | acc.z (4bytes)
    memset(buffData, 0, sizeof(buffData)); 
    memcpy(&buffData[2], &accData.x, 4);
    memcpy(&buffData[6], &accData.y, 4);
    memcpy(&buffData[10], &accData.z, 4);
    accChar.update(buffData);

    //update gyro characteristic (gyro values are float):   timestamp-unused (2bytes) | gyro.x (4bytes) | gyro.y (4bytes) | gyro.z (4bytes)
    memset(buffData, 0, sizeof(buffData)); 
    memcpy(&buffData[2], &gyroData.x, 4);
    memcpy(&buffData[6], &gyroData.y, 4);
    memcpy(&buffData[10], &gyroData.z, 4);
    gyroChar.update(buffData);

    //update Eulero characteristic (angles values are float):   timestamp-unused (2bytes) | roll (4bytes) | pitch (4bytes) | yaw (4bytes)
    memset(buffData, 0, sizeof(buffData)); 
    memcpy(&buffData[2], &qData.roll, 4);
    memcpy(&buffData[6], &qData.pitch, 4);
    memcpy(&buffData[10], &qData.yaw, 4);
    eChar.update(buffData);
    
    //update quaternions characteristic (quaternios values are float):   timestamp-unused (2bytes) | q0 (4bytes) | q1 (4bytes) | q2 (4bytes) | q3 (4bytes)
    memset(buffData, 0, sizeof(buffData)); 
    memcpy(&buffData[2], &qData.q0f, 4);
    memcpy(&buffData[6], &qData.q1f, 4);
    memcpy(&buffData[10], &qData.q2f, 4);
    memcpy(&buffData[14], &qData.q3f, 4);    
    qChar.update(buffData);    
  }


  //manage leds depending on BLE pairing status
  ledHandling();  
}



/*--------------------------------------------------------------
 * 
 * manage leds depending on BLE pairing status
 * 
 -------------------------------------------------------------*/
void ledHandling()
{
  int bleStat;
  
  bleStat = bleManager.getStatus();

  if(bleStat == 1)
  {
    //paired
    rLed.off();
    blinkLed(bLed, 300);
  }
  else
  {
    //not paired
    bLed.off();
    blinkLed(rLed, 500);
  }
}


/*--------------------------------------------------------------
 * 
 * blink led
 * 
 -------------------------------------------------------------*/
void blinkLed(T_Led blinkingLed, int period_msec)
{
  static unsigned int ticksLed = 0;
  static int stp = 0;
  
  if(millis() >= (ticksLed + period_msec))
  {    
    ticksLed = millis();

    //blink blinkingLed
    if(stp == 0)    
      blinkingLed.on();          
    else if (stp == 1)    
      blinkingLed.off();         
    
    stp = (stp + 1) % 2;
  }
}
