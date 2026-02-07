#include"GraphicsDevice.h"
#include "Renderer/Core/ErrorLogger.h"

#include <cassert>

namespace zRender {
    void D3D11Device::Initialize(void* windowHandle) {
        HWND hWnd = (HWND)windowHandle;

        RECT windowRect;

        if (!GetClientRect(hWnd, &windowRect))
        {
            assert("Couldn't get window rect.");
        }

        int width = windowRect.right - windowRect.left;
        int height = windowRect.bottom - windowRect.top;

        UINT creationFlags = 0;
        D3D_FEATURE_LEVEL FeatureLevels = D3D_FEATURE_LEVEL_11_0;

        D3D11CreateDevice(
            NULL,
            D3D_DRIVER_TYPE_HARDWARE,
            NULL,
            creationFlags,
            &FeatureLevels,
            1,
            D3D11_SDK_VERSION,
            &m_Device,
            &FeatureLevel,
            &m_Context
        );

        UINT quality = 0;
        HRESULT hr = m_Device->CheckMultisampleQualityLevels(
            DXGI_FORMAT_R8G8B8A8_UNORM,
            4,
            &quality
        );

        //bool supports4xMSAA = SUCCEEDED(hr) && quality > 0;
        bool supports4xMSAA = false;

        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = 1;
        sd.BufferDesc.Width = width;
        sd.BufferDesc.Height = height;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hWnd;
        sd.Windowed = TRUE;
        sd.SampleDesc.Count = supports4xMSAA ? 4 : 1;
        sd.SampleDesc.Quality = supports4xMSAA ? quality - 1 : 0;

        IDXGIDevice* dxgiDevice;
        m_Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);

        IDXGIAdapter* adapter;
        dxgiDevice->GetAdapter(&adapter);

        IDXGIFactory* factory;
        adapter->GetParent(__uuidof(IDXGIFactory), (void**)&factory);

        factory->CreateSwapChain(m_Device.Get(), &sd, &m_SwapChain);
    }

    void D3D11Device::Resize(int newWidth, int newHeight, bool isFullScreen) {
        m_SwapChain->SetFullscreenState(isFullScreen, nullptr);
        HRESULT hr = m_SwapChain->ResizeBuffers(0, newWidth, newHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
        FAILEDLOG(hr);
        assert(SUCCEEDED(hr));
    }

    void D3D11Device::Release() {
        m_Device->Release();
        m_SwapChain->Release();
        m_Context->Release();
    }

    ID3D11RenderTargetView* D3D11Device::CreateRenderTarget() {
        ID3D11RenderTargetView* rtv = nullptr;

        ID3D11Texture2D* pBackBuffer = nullptr;

        HRESULT hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);

        if (pBackBuffer != 0) {
            m_Device->CreateRenderTargetView(pBackBuffer, nullptr, &rtv);
            pBackBuffer->Release();
        }

        return rtv;
    }

    ID3D11Texture2D* D3D11Device::GetBackBufferTexture() {
        ID3D11Texture2D* pBackBuffer = nullptr;
        HRESULT hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
        return pBackBuffer;
    }
}