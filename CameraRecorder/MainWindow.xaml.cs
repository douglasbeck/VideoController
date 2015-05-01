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
        public MainWindow()
        {
            InitializeComponent();

            captureEngine = new Capture(cameraControlEvents);

            captureEngine.CaptureDevices();
        }

        private void DevicesMenuItem_Click(object sender, RoutedEventArgs e)
        {
            var devicesWindows = new Cameras(captureEngine);
            devicesWindows.ShowDialog();
        }

        CameraControlEvents cameraControlEvents = new CameraControlEvents();
        Capture captureEngine;
    }
}
