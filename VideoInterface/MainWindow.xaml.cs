using System;
using System.ComponentModel;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Diagnostics;
using VideoController;

namespace CameraRecorder
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            cameraControlEvents = new CameraControlEvents(this);

            int hResult = Capture.Create(cameraControlEvents, ref captureEngine);

            if (hResult < 0)
            {
                MessageBox.Show(String.Format("MediaFound startup failure: {0}", hResult));
                Application.Current.Shutdown();
            }

            captureEngine.CaptureDevices();
            devices.SubmenuOpened += devices_SubmenuOpened;
            this.Closing += OnClosing;
        }

        public void OnClosing(Object sender, CancelEventArgs e)
        {
            if(captureEngine != null)
            {
                captureEngine.DestroyCaptureEngine();
            }
        }

        void devices_SubmenuOpened(object sender, RoutedEventArgs e)
        {
            devices.Items.Clear();

            foreach(var device in captureEngine.GetDevices())
            {
                MenuItem item = new MenuItem();
                item.Header = device.GetName();
                item.Click += DevicesMenuItem_Click;
                item.Tag = device;
                devices.Items.Add(item);
            }
        }

        private void DevicesMenuItem_Click(object sender, RoutedEventArgs e)
        {
            var videoDevice = (VideoDevice)((MenuItem)sender).Tag;

            int hResult = captureEngine.Initialize(videoDevice);

            if (hResult >= 0)
            {
                control.IsEnabled = true;
                Debug.Print(videoDevice.GetName());
            }
            else
            {
                MessageBox.Show(String.Format("Device initialization failed: {0}", hResult));
            }
        }

        private void StartPreviewMenuItem_Click(object sender, RoutedEventArgs e)
        {
            captureEngine.StartPreview(panel.Handle);
        }

        private void StopPreviewMenuItem_Click(object sender, RoutedEventArgs e)
        {
            captureEngine.StopPreview();
        }

        private void StartContinuousRecordMenuItem_Click(object sender, RoutedEventArgs e)
        {
            captureEngine.StartContinuousRecord();
            continuouslyRecording = true;
        }

        private void StopContinuousRecordMenuItem_Click(object sender, RoutedEventArgs e)
        {
            continuouslyRecording = false;
            captureEngine.StopContinuousRecord();
        }

        private void StartRecordMenuItem_Click(object sender, RoutedEventArgs e)
        {
            captureEngine.StartRecord();
        }

        private void StopRecordMenuItem_Click(object sender, RoutedEventArgs e)
        {
            captureEngine.StopRecord();
        }

        protected override void OnClosing(CancelEventArgs e)
        {
            base.OnClosing(e);
        }

        public void OnPreviewStarted(int hResult)
        {
            Debug.Print("MainWindow.OnPreviewStarted hResult = {0}", hResult);

            if(hResult < 0)
            {
                MessageBox.Show(string.Format("Start Preview Failed: {0}", hResult), "Start Preview Error");
                return;
            }

            var startPreviewMenuItem            = control.Items[0] as MenuItem;
            var stopPreviewMenuItem             = control.Items[1] as MenuItem;
            var startRecordMenuItem             = control.Items[2] as MenuItem;
            var startContinuousRecordMenuItem   = control.Items[4] as MenuItem;

            if(startPreviewMenuItem != null)
            {
                startPreviewMenuItem.IsEnabled = false;
            }

            if (stopPreviewMenuItem != null)
            {
                stopPreviewMenuItem.IsEnabled = true;
            }

            if (startRecordMenuItem != null)
            {
                startRecordMenuItem.IsEnabled = true;
            }

            if (startContinuousRecordMenuItem != null)
            {
                startContinuousRecordMenuItem.IsEnabled = true;
            }
        }

        public void OnPreviewStopped(int hResult)
        {
            Debug.Print("MainWindow.OnPreviewStopped hResult = {0}, hResult");

            if (hResult < 0)
            {
                MessageBox.Show(string.Format("Stop Preview Failed: {0}", hResult), "Stop Preview Error");
            }

            var startPreviewMenuItem = control.Items[0] as MenuItem;
            var stopPreviewMenuItem = control.Items[1] as MenuItem;
            var startRecordMenuItem = control.Items[2] as MenuItem;
            var startContinuousRecordMenuItem = control.Items[4] as MenuItem;
            var stopContinuousRecordMenuItem = control.Items[5] as MenuItem;

            if (startPreviewMenuItem != null)
            {
                startPreviewMenuItem.IsEnabled = true;
            }

            if (stopPreviewMenuItem != null)
            {
                stopPreviewMenuItem.IsEnabled = false;
            }

            if (startRecordMenuItem != null)
            {
                startRecordMenuItem.IsEnabled = false;
            }

            if (startContinuousRecordMenuItem != null)
            {
                startContinuousRecordMenuItem.IsEnabled = false;
            }

            if (stopContinuousRecordMenuItem != null)
            {
                stopContinuousRecordMenuItem.IsEnabled = false;
            }
        }

        public void OnRecordStarted(int hResult)
        {
            Debug.Print("MainWindow.OnRecordStarted hResult = {0}", hResult);


            if (hResult < 0)
            {
                MessageBox.Show(string.Format("Start Record Failed: {0}", hResult), "Start Record Error");
                return;
            }

            var startPreviewMenuItem            = control.Items[0] as MenuItem;
            var stopPreviewMenuItem             = control.Items[1] as MenuItem;
            var startRecordMenuItem             = control.Items[2] as MenuItem;
            var stopRecordMenuItem              = control.Items[3] as MenuItem;
            var startContinuousRecordMenuItem   = control.Items[4] as MenuItem;
            var stopContinuousRecordMenuItem    = control.Items[5] as MenuItem;

            if (startPreviewMenuItem != null)
            {
                startPreviewMenuItem.IsEnabled = false;
            }

            if (stopPreviewMenuItem != null)
            {
                stopPreviewMenuItem.IsEnabled = false;
            }

            if (startRecordMenuItem != null)
            {
                startRecordMenuItem.IsEnabled = false;
            }

            if (stopRecordMenuItem != null && continuouslyRecording == false)
            {
                stopRecordMenuItem.IsEnabled = true;
            }

            if (startContinuousRecordMenuItem != null)
            {
                startContinuousRecordMenuItem.IsEnabled = false;
            }

            if (stopContinuousRecordMenuItem != null && continuouslyRecording == true)
            {
                stopContinuousRecordMenuItem.IsEnabled = true;
            }
        }

        public void OnRecordStopped(int hResult)
        {
            Debug.Print("MainWindow.OnRecordStopped hResult = {0}", hResult);

            if (hResult < 0)
            {
                MessageBox.Show(string.Format("Stop Record Failed: {0}", hResult), "Stop Record Error");
                return;
            }

            if (continuouslyRecording == true)
            {
                return;
            }

            var startPreviewMenuItem            = control.Items[0] as MenuItem;
            var stopPreviewMenuItem             = control.Items[1] as MenuItem;
            var startRecordMenuItem             = control.Items[2] as MenuItem;
            var stopRecordMenuItem              = control.Items[3] as MenuItem;
            var startContinuousRecordMenuItem   = control.Items[4] as MenuItem;
            var stopContinuousRecordMenuItem    = control.Items[5] as MenuItem;

            if (stopPreviewMenuItem != null)
            {
                stopPreviewMenuItem.IsEnabled = true;
            }

            if (stopRecordMenuItem != null)
            {
                stopRecordMenuItem.IsEnabled = false;
            }

            if (startContinuousRecordMenuItem != null)
            {
                startContinuousRecordMenuItem.IsEnabled = true;
            }

            if (stopContinuousRecordMenuItem != null)
            {
                stopContinuousRecordMenuItem.IsEnabled = false;
            }
        }

        CameraControlEvents cameraControlEvents;
        Capture captureEngine;
        bool continuouslyRecording = false;
    }
}
