/*
 * 
 * simple sketch showing how use TTL serial port available 
 * on J502 connector
 * 
 * Refer to https://github.com/TactigonTeam/Docs/blob/master/Tactigon%20ONE%20V1%20Pinout.pdf for TactigonV1 pinout
 * 
 * 
 * Echo back all received bytes
 * 
 */

#include <tactigon_led.h>
#include <tactigon_IMU.h>
#include <tactigon_BLE.h>
#include <tactigon_Battery.h>
#include <tactigon_Env.h>
#include <tactigon_UserSerial.h>

T_Led rLed, bLed, gLed;

T_UserSerial tSerial;

/*----------------------------------------------------------------------*/
void setup()
{

  rLed.init(T_Led::RED);
  gLed.init(T_Led::GREEN);
  bLed.init(T_Led::BLUE);

  rLed.off();
  gLed.off();
  bLed.off();

  //init user serial
  tSerial.init(T_UserSerial::B_115200, T_UserSerial::T_SERIAL1);
}

/*----------------------------------------------------------------------*/
void loop()
{
  int avlb, i;

  //check for available bytes
  avlb = tSerial.available();

  //echo back all received bytes
  for (i = 0; i < avlb; i++)
  {
    uint8_t b;

    b = tSerial.read();
    tSerial.write((char *)&b, 1);
  }
}