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

using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Windows.Devices.Bluetooth;
using Windows.Devices.Bluetooth.GenericAttributeProfile;
using Windows.Devices.Enumeration;
using Windows.UI.Popups;
using Windows.UI.Xaml.Media.Imaging;
using Windows.Foundation.Metadata;
using System.Collections;
using System.Collections.Generic;
using Windows.Devices.Bluetooth.Advertisement;
using Windows.Storage.Streams;

namespace bleLib
{
    //char changed callback
    public delegate void CharNotifyCB(byte[] charData, string characteristicUUID);


    //TSkin device
    public class TBleDevice
    {
        public string name;
        public List<String> targetCharacteristics;
        public CharNotifyCB charNCallBack;

        public TBleDevice()
        {
            targetCharacteristics = new List<string>();
        }

        
    }

    
    //TSkin scanner
    public class TBleScanner
    {
        public List<TBleDevice> targetDevices;

        string[] requestedProperties =
        {
            "System.Devices.Aep.DeviceAddress",
            "System.Devices.Aep.IsPresent",
            "System.Devices.Aep.Bluetooth.Le.IsConnectable",

            "System.Devices.Aep.CanPair"
        };

        //device watcher
        DeviceWatcher deviceWatcher;

        //ble device
        BluetoothLEDevice btdev;
        

        //------------------------------------------------------------------------------------
        public TBleScanner()
        {
            targetDevices = new List<TBleDevice>();            
        }

        //------------------------------------------------------------------------------------
        public void start()
        {
            //create device watcher and start it
            deviceWatcher = DeviceInformation.CreateWatcher(
                //BluetoothLEDevice.GetDeviceSelectorFromPairingState(false),
                BluetoothLEDevice.GetDeviceSelectorFromConnectionStatus(BluetoothConnectionStatus.Disconnected),  //selector
                requestedProperties,
                DeviceInformationKind.AssociationEndpoint);
            deviceWatcher.Added += DeviceWatcher_Added;
            deviceWatcher.Updated += DeviceWatcher_Updated;
            deviceWatcher.Removed += DeviceWatcher_Removed;
            deviceWatcher.EnumerationCompleted += DeviceWatcher_EnumerationCompleted;
            deviceWatcher.Stopped += DeviceWatcher_Stopped;
            deviceWatcher.Start();
        }

        //------------------------------------------------------------------------------------
        private TBleDevice findInTargetList(string tname)
        {
            TBleDevice tdev;

            tdev = targetDevices.Find(x => x.name == tname);
            return tdev;
        }

        //------------------------------------------------------------------------------------
        private void DeviceWatcher_Stopped(DeviceWatcher sender, object args)
        {
            deviceWatcher.Start();
        }


        //------------------------------------------------------------------------------------
        private void DeviceWatcher_EnumerationCompleted(DeviceWatcher sender, object args)
        {
            deviceWatcher.Stop();
        }


        //------------------------------------------------------------------------------------
        private async void DeviceWatcher_Added(DeviceWatcher sender, DeviceInformation args)
        {
            try
            {
                //get added device end search it in target devices
                btdev = await BluetoothLEDevice.FromIdAsync(args.Id);                
                TBleDevice dev = findInTargetList(btdev.Name);

                //if found deal with its services/characteristics
                if (dev != null)
                {
                   
                    //get SERVICES
                    GattDeviceServicesResult result = await btdev.GetGattServicesAsync();
                   
                    //explore SERVICES
                    foreach (GattDeviceService service in result.Services)
                    {
                        //if custom SERVICE found explore characteristics
                        //if (customGuid == service.Uuid)
                        //{

                        //
                        GattCharacteristicsResult cresult = await service.GetCharacteristicsAsync();                        
                        foreach (GattCharacteristic ccc in cresult.Characteristics)
                        {
                            if(dev.targetCharacteristics.Exists(x => ccc.Uuid == new Guid(x)))                            
                            {
                                await ccc.WriteClientCharacteristicConfigurationDescriptorAsync(GattClientCharacteristicConfigurationDescriptorValue.Notify);
                                ccc.ValueChanged += Characteristic_ValueChanged;                                
                            }
                        }

                        //}
                    }
                }
            }
            catch (Exception e)
            {                
            }
        }


        //------------------------------------------------------------------------------------
        void Characteristic_ValueChanged(GattCharacteristic sender, GattValueChangedEventArgs args)
        {
            try
            {
                TBleDevice dev;
               
                dev = findInTargetList(tname: sender.Service.Device.Name);

                if (dev != null)
                {
                    string charString = sender.Uuid.ToString();
                    DataReader reader = DataReader.FromBuffer(args.CharacteristicValue);

                    //get new bytes
                    byte[] input = new byte[reader.UnconsumedBufferLength];
                    reader.ReadBytes(input);

                    //call CallBack
                    dev.charNCallBack(input, charString);
                }
            }
            catch (Exception e)
            {

            }
        }


        //------------------------------------------------------------------------------------
        private async void DeviceWatcher_Removed(DeviceWatcher sender, DeviceInformationUpdate args)
        {
        }


        //------------------------------------------------------------------------------------
        private async void DeviceWatcher_Updated(DeviceWatcher sender, DeviceInformationUpdate args)
        {
        }
    }
    


}
