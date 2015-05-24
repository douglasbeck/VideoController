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

        public void OnPreviewStarted(int hResult)
        {
            Debug.Print("OnPreviewStarted");
            mainWindow.Dispatcher.Invoke(new Action<int>(mainWindow.OnPreviewStarted), hResult);
        }

        public void OnPreviewStopped(int hResult)
        {
            Debug.Print("OnPreviewStopped");
            mainWindow.Dispatcher.Invoke(new Action<int>(mainWindow.OnPreviewStopped), hResult);
        }


        public void OnRecordStarted(int hResult)
        {
            Debug.Print("OnRecordStarted");
            mainWindow.Dispatcher.Invoke(new Action<int>(mainWindow.OnRecordStarted), hResult);
        }

        public void OnRecordStopped(int hResult)
        {
            Debug.Print("OnRecordStopped");
            mainWindow.Dispatcher.Invoke(new Action<int>(mainWindow.OnRecordStopped), hResult);
        }

        private MainWindow mainWindow;
    }
}
