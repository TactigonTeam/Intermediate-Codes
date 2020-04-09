using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Core;
using Windows.ApplicationModel.Core;

using bleLib;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace AppBleLib
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        TBleScanner bleScanner;
        TBleDevice dev1;

        string ACCELEROMETER_CHARACTERISTIC = "bea5760d-503d-4920-b000-101e7306b007";
        string EULERO_CHARACTERISTIC = "bea5760d-503d-4920-b000-101e7306b009";



        //---------------------------------------------------------------------------------------
        public MainPage()
        {
            this.InitializeComponent();
            
            //init target devices
            dev1 = new TBleDevice();
            dev1.name = "TACTI";
            dev1.targetCharacteristics.Add(ACCELEROMETER_CHARACTERISTIC);            //accelerometer characteristic
            dev1.targetCharacteristics.Add(EULERO_CHARACTERISTIC);                  //eulero angles characteristic
            dev1.charNCallBack = notifyCB;

            //init scanner
            bleScanner = new TBleScanner();
            //bleScanner.charNCallBack = notifyCB;
            bleScanner.targetDevices.Add(dev1);
            bleScanner.start();

            //init gui
            textQuat.Text = "";
            textAcc.Text = "";
        }


        //---------------------------------------------------------------------------------------
        private void notifyCB(byte[] charData, string charUUID)
        {
            if (charUUID == ACCELEROMETER_CHARACTERISTIC)
            {
                float ax = BitConverter.ToSingle(charData, 2);
                float ay = BitConverter.ToSingle(charData, 6);
                float az = BitConverter.ToSingle(charData, 10);

                interThreadAction(() =>
                {
                    textAcc.Text = String.Format("Acc [x, y, z] = [{0:0.0}  {1:0.0}  {2:0.0}]", ax, ay, az);
                });
            }
            else if (charUUID == EULERO_CHARACTERISTIC)
            {
                float roll = BitConverter.ToSingle(charData, 2);
                float pitch = BitConverter.ToSingle(charData, 6);
                float yaw = BitConverter.ToSingle(charData, 10);

                interThreadAction(() =>
                {
                    textQuat.Text = String.Format("Angles [roll, pitch, yaw] = [{0:0.0}  {1:0.0}  {2:0.0}]", radToDeg(roll), radToDeg(pitch), radToDeg(yaw));
                });
            }                      
        }

        //---------------------------------------------------------------------------------------
        double radToDeg(float rad)
        {
            return rad * 180 / Math.PI;
        }

        //---------------------------------------------------------------------------------------
        private async void interThreadAction(DispatchedHandler h)
        {
            await CoreApplication.MainView.CoreWindow.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, h);
        }
    }
}
