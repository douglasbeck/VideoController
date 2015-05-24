using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace CameraRecorder
{
    class MediaFoundation
    {
        [DllImport("Mfplat.dll")]
        public static extern int MFStartup(uint Version, uint dwFlags = 0);
    }
}
