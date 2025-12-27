#include"GraphicsDevice.h"

#include <cassert>

void D3D11Device::Initialize(void* windowHandle) {
    HWND hWnd = (HWND)windowHandle;

    RECT windowRect;

    if (!GetClientRect(hWnd, &windowRect))
    {
        assert("Couldn't get window rect.");
    }

    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;

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
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    D3D_FEATURE_LEVEL FeatureLevels = D3D_FEATURE_LEVEL_11_0;

    UINT creationFlags = 0;

    //#ifdef _DEBUG
    //    creationFlags |= D3D11_CREATE_DEVICE_DEBUG; // Add the debug flag
    //#endif

    D3D11CreateDeviceAndSwapChain(
        NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        creationFlags,
        &FeatureLevels,
        1,
        D3D11_SDK_VERSION,
        &sd,
        &m_SwapChain,
        &m_Device,
        &FeatureLevel,
        &m_Context
    );
}

void D3D11Device::Shutdown() {

}

ID3D11RenderTargetView* D3D11Device::CreateRenderTarget() {
    ID3D11RenderTargetView* rtv = nullptr;

    ID3D11Texture2D* pBackBuffer = nullptr;

    HRESULT hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);

    D3D11_TEXTURE2D_DESC desc;
    pBackBuffer->GetDesc(&desc);

    int m_width = desc.Width;
    int m_height = desc.Height;

    if (pBackBuffer != 0) {
        m_Device->CreateRenderTargetView(pBackBuffer, nullptr, &rtv);
        pBackBuffer->Release();
    }



    return rtv;
}