# Screenshot Capture DLL

This project contains a C++ DLL for capturing screenshots of specific windows in Windows, with improved error handling. Here's how to build the DLL and use it in your C# project.

## Building the DLL

1. Prerequisites:
   - Visual Studio 2019 or later
   - Windows 10 SDK (10.0.17763.0 or later)
   - C++/WinRT extension for Visual Studio

2. Create a new project:
   - Open Visual Studio
   - Create a new project
   - Select "Dynamic-Link Library (DLL)" as the project type
   - Name your project (e.g., "ScreenshotCaptureDLL")

3. Add source files:
   - Add `ScreenShotLibrary.h` and `ScreenShot.cpp` to your project

4. Configure project settings:
   - Right-click on the project in Solution Explorer and select "Properties"
   - Under "Configuration Properties" -> "C/C++" -> "Language":
     - Set "C++ Language Standard" to "ISO C++17 Standard (/std:c++17)"
   - Under "Configuration Properties" -> "C/C++" -> "Preprocessor":
     - Add "NOMINMAX" to "Preprocessor Definitions"
   - Under "Configuration Properties" -> "Linker" -> "Input":
     - Add "d3d11.lib" and "dxgi.lib" to "Additional Dependencies"

5. Build the project:
   - Set the build configuration to "Release" and the platform to "x64"
   - Build the solution (F7 or "Build" -> "Build Solution")

The DLL will be created in the "x64/Release" folder of your project.

## Using the DLL in a C# Project

1. Create a new C# project or open an existing one

2. Add a reference to the DLL:
   - Right-click on your C# project in Solution Explorer
   - Select "Add" -> "Reference"
   - Click "Browse" and navigate to the DLL file you built
   - Select the DLL and click "Add"

3. Use the improved ScreenshotWrapper class:

```csharp
using System;
using System.Drawing;
using System.Windows.Forms;

class Program
{
    static void Main()
    {
        IntPtr hwnd = Process.GetProcessesByName("notepad")[0].MainWindowHandle; // Example: capturing Notepad's window

        try
        {
            using (var screenshotWrapper = new ScreenshotWrapper(hwnd))
            {
                byte[] screenshotData = screenshotWrapper.CaptureScreenshot(out int width, out int height);
                
                // Create a Bitmap from the raw data
                using (var bitmap = new Bitmap(width, height, System.Drawing.Imaging.PixelFormat.Format32bppArgb))
                {
                    var bitmapData = bitmap.LockBits(new Rectangle(0, 0, width, height), System.Drawing.Imaging.ImageLockMode.WriteOnly, bitmap.PixelFormat);
                    Marshal.Copy(screenshotData, 0, bitmapData.Scan0, screenshotData.Length);
                    bitmap.UnlockBits(bitmapData);

                    // Now you can use the bitmap for further processing or save it
                    bitmap.Save("screenshot.png");
                }
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Error: {ex.Message}");
            Console.WriteLine($"Last DLL error: {ScreenshotWrapper.Get
