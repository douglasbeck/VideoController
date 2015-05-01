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
        public void OnPreviewCompleted()
        {
            Debug.Print("OnPreviewCompleted");
        }
    }
}
