
#include <tactigon_led.h>
#include <tactigon_IMU.h>
#include <tactigon_BLE.h>

T_Led rLed, bLed, gLed;
T_ACC accMeter;
T_AccData accData;

T_QUAT qMeter;
T_QData qData;

T_BLE bleManager;
UUID uuid;
T_BLE_Characteristic bleChar;
boolean lastInfZ = false;
boolean lastSupZ = false;
boolean lastInfY = false;
boolean lastSupY = false;
boolean lastInfX = false;
boolean lastSupX = false;
boolean justSent = true;
float threshold = 9.6;
int ticks;
//Used for LEDs cycle
int ticksLed, stp;

void setup()
{

  ticks = 0;
  rLed.init(T_Led::RED);
  gLed.init(T_Led::GREEN);
  bLed.init(T_Led::BLUE);

  rLed.off();
  gLed.off();
  bLed.off();
  bleManager.InitRole(TACTIGON_BLE_PERIPHERAL);
  //bleManager.setName("TTGPS");
  //add acc characteristic
  uuid.set("7ac71000-503d-4920-b000-acc000000001");
  bleChar = bleManager.addNewChar(uuid, 2);
}

void flash()
{
  rLed.on();
  gLed.on();
  bLed.on();
}

void loop()
{
  unsigned char buffData[2] = {0x00, 0x00};
  if (millis() >= (ticksLed + (1000 / 50)))
  {
    //get acc data
    accData = accMeter.getAxis();
    if (!justSent)
    {
      if (accData.z > threshold && !lastSupZ)
      {
        lastSupZ = true;
        lastInfZ = false;
        lastInfX = false;
        lastSupX = false;
        lastInfY = false;
        lastSupY = false;
        buffData[0] = 0x00;
        buffData[1] = 0xFF;
        flash();
        //bleChar.update(buffData);
        justSent = true;
      }
      if (accData.z < -threshold && !lastInfZ)
      {
        lastInfZ = true;
        lastSupZ = false;
        lastInfX = false;
        lastSupX = false;
        lastInfY = false;
        lastSupY = false;
        buffData[0] = 0xFF;
        buffData[1] = 0xFF;
        flash();
        //bleChar.update(buffData);
        justSent = true;
      }
      if (accData.y > threshold && !lastSupY)
      {
        lastSupY = true;
        lastInfY = false;
        lastInfZ = false;
        lastSupZ = false;
        lastInfX = false;
        lastSupX = false;
        lastInfY = false;

        buffData[0] = 0x5A;
        buffData[1] = 0x5A;
        flash();
        bleChar.update(buffData);
        justSent = true;
      }
      if (accData.y < -threshold && !lastInfY)
      {
        lastInfY = true;
        lastSupY = false;
        lastInfZ = false;
        lastSupZ = false;
        lastInfX = false;
        lastSupX = false;
        lastSupY = false;

        buffData[0] = 0x00;
        buffData[1] = 0x88;
        flash();
        bleChar.update(buffData);
        justSent = true;
      }
      if (accData.x > threshold && !lastSupX)
      {
        lastSupX = true;
        lastInfX = false;
        lastInfZ = false;
        lastSupZ = false;
        lastInfX = false;
        lastInfY = false;
        lastSupY = false;
        buffData[0] = 0x00;
        buffData[1] = 0xBB;
        flash();
        bleChar.update(buffData);
        justSent = true;
      }
      if (accData.x < -threshold && !lastInfX)
      {
        lastInfX = true;
        lastSupX = false;
        lastInfZ = false;
        lastSupZ = false;
        lastSupX = false;
        lastInfY = false;
        lastSupY = false;

        buffData[0] = 0x15;
        buffData[1] = 0x15;
        flash();
        bleChar.update(buffData);
        justSent = true;
      }
    }
  }
  if (millis() >= (ticksLed + (1000 / 2)))
  {

    justSent = false;
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
      gLed.off();
      bLed.on();
    }
    else if (stp == 2)
    {
      rLed.off();
      gLed.on();
      bLed.off();
    }
    stp = (stp + 1) % 3;
  }
}
