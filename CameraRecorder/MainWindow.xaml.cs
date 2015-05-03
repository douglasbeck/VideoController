using System;
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

            int hr = MediaFoundation.MFStartup(MF_VERSION);

            if (hr < 0)
            {
                return;
            }


            viewer.ShowActivated = false;
            viewer.Show();
            viewer.Hide();

            var wih = new WindowInteropHelper(viewer);
            var viewerHandle = wih.Handle;

            captureEngine = new Capture(cameraControlEvents, viewerHandle);

            captureEngine.CaptureDevices();
        }

        private void DevicesMenuItem_Click(object sender, RoutedEventArgs e)
        {
            var devicesWindows = new Cameras(captureEngine);
            devicesWindows.ShowDialog();
        }

        CameraControlEvents cameraControlEvents = new CameraControlEvents();
        Capture captureEngine;
        Viewer viewer = new Viewer();

        private void Preview_Click(object sender, RoutedEventArgs e)
        {
            viewer.Show();
            captureEngine.StartPreview();
        }
    }
}
