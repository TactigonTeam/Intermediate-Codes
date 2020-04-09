/*
 * This example shows how add a writeable characteristic on the BLE interface 
 * (in addition to usual streaming characteristics)
 * This could be useful to send commands to Tactigon board via BLE
 * 
 * The example simply echo on debug serial what received on BLE
 * 
 * You can test this for example using free Android app "BLE Scanner"
 * Connect to Tactigon board, search for "bea5760d-503d-4920-b000-101e7306b001"
 * char under CUSTOM SERVICE.
 * 
 * This char should have "W" (writeable) icon
 * 
 * Click on it and write something, you should see echo on debug serial.
 * Write something in Text format to see something of human readeable on debug serial
 */

#include <tactigon_led.h>
#include <tactigon_IMU.h>
#include <tactigon_BLE.h>

T_Led rLed, bLed, gLed;

T_QUAT qMeter;
T_QData qData;

T_BLE bleManager;
UUID uuid;
T_BLE_Characteristic qChar, cmdChar;

/*--------------------------------------------------------------------------------
 * 
 * Call back function to be called at every nwe data packet on BLE interface
 * 
 ---------------------------------------------------------------------------------*/
void cbBLEcharWritten(uint8_t *pData, uint8_t dataLen)
{
  for (int i = 0; i < dataLen; i++)
    Serial.print((char)*pData++);
}

/*----------------------------------------------------------------------*/
void setup()
{
  rLed.init(T_Led::RED);
  gLed.init(T_Led::GREEN);
  bLed.init(T_Led::BLUE);

  rLed.off();
  gLed.off();
  bLed.off();

  //init name
  bleManager.setName("TACTI");

  //init role
  bleManager.InitRole(TACTIGON_BLE_PERIPHERAL);

  //add streaming characteristic
  uuid.set("bea5760d-503d-4920-b000-101e7306b005"); //quaternions as float
  qChar = bleManager.addNewChar(uuid, 18);

  //add a writable characteristic passing as parameter max len
  uuid.set("bea5760d-503d-4920-b000-101e7306b001"); //command char
  cmdChar = bleManager.addNewChar(uuid, 18, 1);

  //install a callback on writeable characteristic. It will be called at every new data packet written on characteristic
  cmdChar.setWcb(cbBLEcharWritten);

  //disable use of magnetometer in quat computing
  qMeter.enable();
  qMeter.useMag(0);
}

/*----------------------------------------------------------------------*/
void loop()
{
  static int ticks = 0;
  unsigned char buffData[24];

  //update BLE characteristics @ 50Hz (20 millisec period)
  if (millis() >= (ticks + 20))
  {
    ticks = millis();

    //get IMU data
    qData = qMeter.getQs();

    //update quat char
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

  if (bleStat == 1)
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

  if (millis() >= (ticksLed + period_msec))
  {
    ticksLed = millis();

    //blink blinkingLed
    if (stp == 0)
      blinkingLed.on();
    else if (stp == 1)
      blinkingLed.off();

    stp = (stp + 1) % 2;
  }
}