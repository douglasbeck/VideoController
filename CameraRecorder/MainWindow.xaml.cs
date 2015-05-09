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
using MediaController;

namespace CameraRecorder
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        const int MF_VERSION = 0x0002 << 16 | 0x0070;

        public MainWindow()
        {
            InitializeComponent();

            int hResult = MediaFoundation.MFStartup(MF_VERSION);

            if (hResult < 0)
            {
                MessageBox.Show(String.Format("MediaFound startup failure: {0}", hResult));
                Application.Current.Shutdown();
            }

            cameraControlEvents = new CameraControlEvents(this);
            captureEngine = new Capture(cameraControlEvents);
            captureEngine.CaptureDevices();

            devices.SubmenuOpened += devices_SubmenuOpened;
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

        protected override void OnClosing(CancelEventArgs e)
        {
            base.OnClosing(e);
        }

        public void OnPreviewStarted(int hResult)
        {
            Debug.Print("MainWindow.OnPreviewStarted hResult = {0}");

            if(hResult < 0)
            {
                MessageBox.Show(string.Format("Start Preview Failed: {0}", hResult), "Start Preview Error");
                return;
            }

            var startPreviewMenuItem = control.Items[0] as MenuItem;
            var stopPreviewMenuItem = control.Items[1] as MenuItem;

            if(startPreviewMenuItem != null)
            {
                startPreviewMenuItem.IsEnabled = false;
            }

            if (stopPreviewMenuItem != null)
            {
                stopPreviewMenuItem.IsEnabled = true;
            }
        }

        public void OnPreviewStopped(int hResult)
        {
            Debug.Print("MainWindow.OnPreviewStopped hResult = {0}");

            if (hResult < 0)
            {
                MessageBox.Show(string.Format("Stop Preview Failed: {0}", hResult), "Stop Preview Error");
            }

            var startPreviewMenuItem = control.Items[0] as MenuItem;
            var stopPreviewMenuItem = control.Items[1] as MenuItem;

            if (startPreviewMenuItem != null)
            {
                startPreviewMenuItem.IsEnabled = true;
            }

            if (stopPreviewMenuItem != null)
            {
                stopPreviewMenuItem.IsEnabled = false;
            }
        }

        CameraControlEvents cameraControlEvents;
        Capture captureEngine;

        private void control_Click(object sender, RoutedEventArgs e)
        {

        }

    }
}
