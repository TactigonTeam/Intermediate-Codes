/*
 * 
 * In this example Tactigon board is CENTRAL on BLE interface.
 * 
 * Setting target MAC and characteristic UUID, Tactigon continuously 
 * scan the BLE interface for this target and connect to it
 * 
 * In this example the target is a Adafruit BLE-serial adapter
 * 
 * Tactigon board writes Euler angles in ascii format so that, connecting
 * Adafruit adapter to a PC, it is possible get angles in a human readeable
 * format on a standard serial terminal.
 * 
 * 
 * 
 * Max single write is 20 byte
 * 
 * 
 */

#include <tactigon_led.h>
#include <tactigon_IMU.h>
#include <tactigon_BLE.h>
#include <tactigon_IO.h>

T_Led rLed, bLed, gLed;
T_ACC accMeter;
T_AccData accData;
T_GYRO gyro;

T_QUAT qMeter;
T_QData qData;

T_BLE bleManager;

UUID targetUUID;

//GPIO
T_GPP gpp1;
T_GPP gpp2;
T_GPP gpp3;
T_GPP gpp4;

const uint8_t BUFF_SIZE = 20;
char buffData[BUFF_SIZE];

float roll;
float pitch;
float yaw;

int ticksProc;

//Used for LEDs cycle
int ticksLed, stp;

//Adafruit BLE-Serial adapter
uint8_t targetMAC[6] = {0xee, 0xca, 0x7c, 0x3d, 0xe5, 0x20};

/*------------------------------------------------------------------------*/
void setup()
{
  ticksProc = 0;
  ticksLed = 0;
  rLed.init(T_Led::RED);
  gLed.init(T_Led::GREEN);
  bLed.init(T_Led::BLUE);

  rLed.off();
  gLed.off();
  bLed.off();

  //set BLE role: CENTRAL
  bleManager.InitRole(TACTIGON_BLE_CENTRAL);

  //set BLE name
  bleManager.setName("TCNTR");

  //set target peripheral (name and characteristic) to write to
  targetUUID.set("6e400002-b5a3-f393-e0a9-e50e24dcca9e");
  bleManager.setTarget(targetMAC, targetUUID);

  //enable quaternions/Euler angles computing
  qMeter.enable();
  qMeter.useMag(0);
}

/*-------------------------------------------------------------------------------------*/
void loop()
{
  //get IMU data and write to target char @ 50Hz (20msec))
  if (millis() >= (ticksProc + 20))
  {
    ticksProc = millis();

    //get Euler angles.
    qData = qMeter.getQs();

    //convert to deg and remap roll
    roll = rollZeroRemap(radToDeg(qData.roll));
    pitch = radToDeg(qData.pitch);
    yaw = radToDeg(qData.yaw);

    //write to peripheral
    memset(buffData, 0, BUFF_SIZE);

    snprintf(buffData, BUFF_SIZE, "%04d %04d %04d\n", (int)roll, (int)pitch, (int)yaw);
    bleManager.writeToPeripheral((unsigned char *)buffData, BUFF_SIZE);
  }
}

/*----------------------------------------------------------------------
 * 
 * convert [rad] to [deg]
 * 
 ---------------------------------------------------------------------*/
float radToDeg(float rad)
{
  return rad * 180 / PI;
}

/*----------------------------------------------------------------------
 *
 * remap roll in order to have 0 deg in horizontal position
 * and +180,-180 periodicity around this zero position
 *
 *----------------------------------------------------------------------*/
float rollZeroRemap(float r)
{
  if ((r > -180) && (r < 90))
    r = r + 90;
  else
    r = -270 + r;

  return r;
}