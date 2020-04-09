
#include <tactigon_led.h>
#include <tactigon_IMU.h>
#include <tactigon_BLE.h>
#include "IMU_main.h"

T_Led rLed, bLed, gLed;

T_QUAT qMeter;
T_QData qData;

T_GYRO gMeter;

T_BLE bleManager;
UUID uuid;
T_BLE_Characteristic bleChar;

///////////////////////////////////////////////////////////////////////
void setup()
{
  rLed.init(T_Led::RED);
  gLed.init(T_Led::GREEN);
  bLed.init(T_Led::BLUE);

  rLed.off();
  gLed.off();
  bLed.off();

  //init role
  bleManager.InitRole(TACTIGON_BLE_PERIPHERAL);

  //set BLE name
  bleManager.setName("TACTI");

  //add data characteristic
  uuid.set("bea5760d-503d-4920-b000-101e7306b005");
  bleChar = bleManager.addNewChar(uuid, 18);

  //
  qMeter.useMag(0);

  //calibration
  //gMeter.startCalib();
}

///////////////////////////////////////////////////////////////////////
void loop()
{
  static int ticks = 0;
  unsigned char buffData[24];

  //update BLE characteristic @ 50Hz
  if (millis() >= (ticks + (1000 / 50)))
  {
    ticks = millis();

    //get quaternions data
    qData = qMeter.getQs();

    //prepare buffer
    memset(buffData, 0, sizeof(buffData));
    memcpy(&buffData[2], &qData.q0f, 4);
    memcpy(&buffData[6], &qData.q1f, 4);
    memcpy(&buffData[10], &qData.q2f, 4);
    memcpy(&buffData[14], &qData.q3f, 4);

    //update ble characteristic
    bleChar.update(buffData);
  }

  //mange leds
  ledHandling();
}

///////////////////////////////////////////////////////////////////////
void ledHandling()
{
  static int ticksLed = 0;
  static int stp = 0;

  //rotate rgb led
  if (millis() >= (ticksLed + (1000 / 1)))
  {
    ticksLed = millis();

    if (stp == 0)
    {
      rLed.on();
      gLed.off();
      bLed.off();
    }
    else if (stp == 1)
    {
      rLed.off();
      gLed.on();
      bLed.off();
    }
    else if (stp == 2)
    {
      rLed.off();
      gLed.off();
      bLed.on();
    }

    stp = (stp + 1) % 3;
  }
}