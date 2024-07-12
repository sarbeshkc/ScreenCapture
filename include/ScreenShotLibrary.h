#pragma once

#include <cstdint>
#include <windows.h>

struct BitmapData
{
    uint8_t* data;
    int width;
    int height;
};

class ScreenshotCapture;

extern "C"
{
    __declspec(dllexport) ScreenshotCapture* CreateScreenshotCapture(HWND windowHandle);
    __declspec(dllexport) void DestroyScreenshotCapture(ScreenshotCapture* capture);
    __declspec(dllexport) BitmapData CaptureWindow(ScreenshotCapture* capture);
    __declspec(dllexport) void FreeBitmapData(BitmapData* bitmapData);
}
