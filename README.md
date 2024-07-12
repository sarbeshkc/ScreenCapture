# Screenshot Capture Library

This is a high-performance C++ library for capturing screenshots of specific windows, even if the window is covered (but not minimized). The library utilizes the Windows.Graphics.Capture API and Direct3D11 to achieve efficient GPU-accelerated capture.

## Features

- Capture screenshots of specific windows using the window handle (HWND)
- High-performance capture utilizing the Windows.Graphics.Capture API and Direct3D11
- Supports running multiple instances simultaneously, with each instance capturing a different window sequentially
- Callable from C# by compiling the library as a DLL

## Prerequisites

- Windows 10 or later
- Visual Studio with C++ and C# support
- Windows SDK
- DirectX SDK

## Building the DLL

1. Open the solution in Visual Studio.
2. Build the project in Release configuration.
3. The output will be a DLL file named `ScreenshotLibrary.dll`.

## Using the DLL in a C# Project

1. Add a reference to the `ScreenshotLibrary.dll` file in your C# project.
   - Right-click on your project in the Solution Explorer and select "Add Reference".
   - Browse to the location of the `ScreenshotLibrary.dll` file and select it.
   - Click "OK" to add the reference.

2. Import the functions from the DLL using the `DllImport` attribute. Create a static class to hold the imported functions:

```csharp
using System;
using System.Runtime.InteropServices;

public static class ScreenshotLibrary
{
    [DllImport("ScreenshotLibrary.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr CreateScreenshotCapture(IntPtr windowHandle);

    [DllImport("ScreenshotLibrary.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern void DestroyScreenshotCapture(IntPtr capture);

    [DllImport("ScreenshotLibrary.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern BitmapData CaptureWindow(IntPtr capture);

    [DllImport("ScreenshotLibrary.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern void FreeBitmapData(ref BitmapData bitmapData);
}

[StructLayout(LayoutKind.Sequential)]
public struct BitmapData
{
    public IntPtr Data;
    public int Width;
    public int Height;
}
