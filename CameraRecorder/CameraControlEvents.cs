using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using MediaController;

namespace CameraRecorder
{
    public class CameraControlEvents : ICaptureEventSink
    {
        public CameraControlEvents(MainWindow mainWindow)
        {
            this.mainWindow = mainWindow;
        }
        public void OnPreviewStarted()
        {
            Debug.Print("OnPreviewStarted");
        }
        public void OnPreviewStopped()
        {
            Debug.Print("OnPreviewStopped");
        }

        private MainWindow mainWindow;
    }
}
