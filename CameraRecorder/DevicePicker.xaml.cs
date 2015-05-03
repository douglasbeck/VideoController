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
using System.Windows.Shapes;
using System.Diagnostics;
using MediaController;

namespace CameraRecorder
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    public partial class Cameras : Window
    {
        public Cameras(Capture captureEngine)
        {
            this.captureEngine = captureEngine;

            InitializeComponent();

            foreach (var videoDevice in captureEngine.GetDevices())
            {
                this.deviceList.Items.Add(videoDevice);
            }
        }

        private Capture captureEngine;

        private void Ok_Click(object sender, RoutedEventArgs e)
        {
            if (this.deviceList.SelectedItem == null)
            {
                return;
            }

            var videoDevice = (VideoDevice)this.deviceList.SelectedItem;

            captureEngine.Initialize(videoDevice);

            this.Close();

            Debug.Print(videoDevice.GetName());
        }

        private void deviceList_MouseDoubleClick(object sender, RoutedEventArgs e)
        {
            if (this.deviceList.SelectedItem == null)
            {
                return;
            }

            var videoDevice = (VideoDevice)this.deviceList.SelectedItem;

            captureEngine.Initialize(videoDevice);

            this.Close();

            Debug.Print(videoDevice.GetName());
        }
    }
}
