#include "../include/ScreenShotLibrary.h"
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <wrl.h>
#include <stdexcept>

using namespace winrt::Windows::Graphics::Capture;
using namespace winrt::Windows::Graphics::DirectX;
using namespace winrt::Windows::Graphics::DirectX::Direct3D11;
using namespace Microsoft::WRL;

class ScreenshotCapture
{
public:
    ScreenshotCapture(HWND windowHandle)
        : m_windowHandle(windowHandle)
    {
        Initialize();
    }

    ~ScreenshotCapture()
    {
        // Clean up resources
        m_session.Close();
        m_framePool.Close();
        m_captureItem = nullptr;
        m_d3dContext->Release();
        m_d3dDevice->Release();
    }

    BitmapData CaptureWindow()
    {
        BitmapData bitmapData = { nullptr, 0, 0 };

        try
        {
            // Get the next available captured frame
            auto capturedFrame = m_framePool.TryGetNextFrame();
            auto surfaceTexture = capturedFrame.Surface().as<IDirect3DSurface>();

            // Map the surface texture to access the bitmap data
            D3D11_MAPPED_SUBRESOURCE mappedResource;
            HRESULT hr = m_d3dContext->Map(surfaceTexture.get(), 0, D3D11_MAP_READ, 0, &mappedResource);
            if (FAILED(hr))
            {
                throw std::runtime_error("Failed to map surface texture.");
            }

            // Get the bitmap dimensions
            int width = m_captureItem.Size().Width;
            int height = m_captureItem.Size().Height;
            int stride = mappedResource.RowPitch;
            uint8_t* sourceData = static_cast<uint8_t*>(mappedResource.pData);

            // Copy the bitmap data to a new buffer
            uint8_t* bitmapData = new uint8_t[width * height * 4];
            for (int y = 0; y < height; y++)
            {
                memcpy(bitmapData + y * width * 4, sourceData + y * stride, width * 4);
            }

            // Unmap the surface texture
            m_d3dContext->Unmap(surfaceTexture.get(), 0);

            // Set the bitmap data
            bitmapData.data = bitmapData;
            bitmapData.width = width;
            bitmapData.height = height;
        }
        catch (const std::exception& ex)
        {
            // Log the error message
            // You can replace this with your own logging mechanism
            OutputDebugStringA(ex.what());
        }

        return bitmapData;
    }

private:
    void Initialize()
    {
        // Create Direct3D11 device and device context
        D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
        HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, featureLevels, ARRAYSIZE(featureLevels),
            D3D11_SDK_VERSION, &m_d3dDevice, nullptr, &m_d3dContext);
        if (FAILED(hr))
        {
            throw std::runtime_error("Failed to create Direct3D11 device.");
        }

        // Create capture item for the window
        m_captureItem = CreateCaptureItemForWindow(m_windowHandle);

        // Create frame pool with captured frames
        m_framePool = Direct3D11CaptureFramePool::Create(m_d3dDevice, DirectXPixelFormat::B8G8R8A8UIntNormalized, 1, m_captureItem.Size());
        m_session = m_framePool.CreateCaptureSession(m_captureItem);

        // Start capturing frames
        m_session.StartCapture();
    }

    HWND m_windowHandle;
    GraphicsCaptureItem m_captureItem{ nullptr };
    Direct3D11CaptureFramePool m_framePool{ nullptr };
    GraphicsCaptureSession m_session{ nullptr };
    ID3D11Device* m_d3dDevice;
    ID3D11DeviceContext* m_d3dContext;
};

extern "C" __declspec(dllexport) ScreenshotCapture* CreateScreenshotCapture(HWND windowHandle)
{
    try
    {
        return new ScreenshotCapture(windowHandle);
    }
    catch (const std::exception& ex)
    {
        // Log the error message
        // You can replace this with your own logging mechanism
        OutputDebugStringA(ex.what());
        return nullptr;
    }
}

extern "C" __declspec(dllexport) void DestroyScreenshotCapture(ScreenshotCapture* capture)
{
    delete capture;
}

extern "C" __declspec(dllexport) BitmapData CaptureWindow(ScreenshotCapture* capture)
{
    if (capture != nullptr)
    {
        return capture->CaptureWindow();
    }
    else
    {
        return { nullptr, 0, 0 };
    }
}

extern "C" __declspec(dllexport) void FreeBitmapData(BitmapData* bitmapData)
{
    if (bitmapData != nullptr && bitmapData->data != nullptr)
    {
        delete[] bitmapData->data;
        bitmapData->data = nullptr;
        bitmapData->width = 0;
        bitmapData->height = 0;
    }
}
