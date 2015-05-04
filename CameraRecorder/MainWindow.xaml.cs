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
        }

        private void DevicesMenuItem_Click(object sender, RoutedEventArgs e)
        {
            var devicesWindows = new Cameras(this, captureEngine);
            devicesWindows.ShowDialog();
        }

        private void PreviewMenuItem_Click(object sender, RoutedEventArgs e)
        {
            viewer.ShowActivated = false;
            viewer.Show();

            var wih = new WindowInteropHelper(viewer);
            var viewerHandle = wih.Handle;

            captureEngine.StartPreview(viewerHandle);
        }

        protected override void OnClosing(CancelEventArgs e)
        {
            this.viewer.Close();
            base.OnClosing(e);
        }

        CameraControlEvents cameraControlEvents;
        Capture captureEngine;
        Viewer viewer = new Viewer();

    }
}
