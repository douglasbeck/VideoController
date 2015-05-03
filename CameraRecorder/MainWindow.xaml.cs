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

            cameraControlEvents = new CameraControlEvents(this);

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
            var devicesWindows = new Cameras(this, captureEngine);
            devicesWindows.ShowDialog();
        }

        private void PreviewMenuItem_Click(object sender, RoutedEventArgs e)
        {
            viewer.Show();
            captureEngine.StartPreview();
        }

        CameraControlEvents cameraControlEvents;
        Capture captureEngine;
        Viewer viewer = new Viewer();

    }
}
