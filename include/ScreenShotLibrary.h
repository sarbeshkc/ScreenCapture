#pragma once

#include <cstdint>
#include <windows.h>
#include <string>

struct BitmapData
{
    uint8_t* data;
    int width;
    int height;
};

struct CaptureError
{
    int errorCode;
    std::string errorMessage;
};

class ScreenshotCapture;

extern "C"
{
    __declspec(dllexport) ScreenshotCapture* CreateScreenshotCapture(HWND windowHandle, CaptureError* error);
    __declspec(dllexport) void DestroyScreenshotCapture(ScreenshotCapture* capture);
    __declspec(dllexport) BitmapData CaptureWindow(ScreenshotCapture* capture, CaptureError* error);
    __declspec(dllexport) void FreeBitmapData(BitmapData* bitmapData);
    __declspec(dllexport) const char* GetLastErrorMessage();
}
