#pragma once

#include<d3d11.h>
#include<dxgi.h>
#include<d3dcompiler.h>
#include<wrl/client.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include"Renderer/Render/GraphicsDeviceInterface.h"

namespace zRender {
    class D3D11Device : public GraphicsDevice {
    public:
        void Initialize(void* windowHandle) override;
        void Shutdown() override;

        ID3D11RenderTargetView* CreateRenderTarget();
        ID3D11Texture2D* GetBackBufferTexture();

        ID3D11Device* GetDevice() const { return m_Device.Get(); }
        IDXGISwapChain* GetSwapChain() const { return m_SwapChain.Get(); }
        ID3D11DeviceContext* GetDeviceContext() const { return m_Context.Get(); }

    private:
        Microsoft::WRL::ComPtr<ID3D11Device> m_Device;
        Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_Context;
        D3D_FEATURE_LEVEL FeatureLevel;
    };
}