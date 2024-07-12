using System;
using System.Runtime.InteropServices;

public class ScreenshotWrapper : IDisposable
{
    private IntPtr _capturePtr;

    [StructLayout(LayoutKind.Sequential)]
    private struct CaptureError
    {
        public int errorCode;
        [MarshalAs(UnmanagedType.LPStr)]
        public string errorMessage;
    }

    [DllImport("ScreenshotCaptureDLL.dll")]
    private static extern IntPtr CreateScreenshotCapture(IntPtr windowHandle, ref CaptureError error);

    [DllImport("ScreenshotCaptureDLL.dll")]
    private static extern void DestroyScreenshotCapture(IntPtr capture);

    [DllImport("ScreenshotCaptureDLL.dll")]
    private static extern BitmapData CaptureWindow(IntPtr capture, ref CaptureError error);

    [DllImport("ScreenshotCaptureDLL.dll")]
    private static extern void FreeBitmapData(ref BitmapData bitmapData);

    [DllImport("ScreenshotCaptureDLL.dll")]
    private static extern IntPtr GetLastErrorMessage();

    [StructLayout(LayoutKind.Sequential)]
    public struct BitmapData
    {
        public IntPtr data;
        public int width;
        public int height;
    }

    public ScreenshotWrapper(IntPtr windowHandle)
    {
        CaptureError error = new CaptureError();
        _capturePtr = CreateScreenshotCapture(windowHandle, ref error);
        if (_capturePtr == IntPtr.Zero)
        {
            throw new Exception($"Failed to create screenshot capture. Error code: {error.errorCode}, Message: {error.errorMessage}");
        }
    }

    public byte[] CaptureScreenshot(out int width, out int height)
    {
        CaptureError error = new CaptureError();
        BitmapData bitmapData = CaptureWindow(_capturePtr, ref error);
        if (bitmapData.data == IntPtr.Zero)
        {
            throw new Exception($"Failed to capture screenshot. Error code: {error.errorCode}, Message: {error.errorMessage}");
        }

        width = bitmapData.width;
        height = bitmapData.height;
        int dataSize = width * height * 4; // Assuming 32-bit RGBA format

        byte[] managedArray = new byte[dataSize];
        Marshal.Copy(bitmapData.data, managedArray, 0, dataSize);

        FreeBitmapData(ref bitmapData);

        return managedArray;
    }

    public void Dispose()
    {
        if (_capturePtr != IntPtr.Zero)
        {
            DestroyScreenshotCapture(_capturePtr);
            _capturePtr = IntPtr.Zero;
        }
    }

    public static string GetLastError()
    {
        IntPtr errorPtr = GetLastErrorMessage();
        return Marshal.PtrToStringAnsi(errorPtr);
    }
}
