#pragma once

#include<d3d12.h>
#include<dxgi1_6.h>
#include<d3dcompiler.h>
#include<wrl/client.h>

#include"d3dx12.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include"Renderer/Render/GraphicsDeviceInterface.h"

namespace zRender {
    class D3D12Device : public GraphicsDevice {
    public:
        void Initialize(void* windowHandle) override;
        void Release() override;

        void Resize(int newWidth, int newHeight, bool isFullscreen);

        ID3D12Device* GetDevice() const { return m_Device.Get(); }
        IDXGISwapChain* GetSwapChain() const { return m_SwapChain.Get(); }

        IRenderResourceProvider* CreateResourceProvider() override;

    private:
        static const uint8_t m_NumFrames = 3;

        Microsoft::WRL::ComPtr<IDXGIFactory7> m_Factory;
        Microsoft::WRL::ComPtr<ID3D12Device2> m_Device;
        Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList;
        Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> g_CommandAllocators[m_NumFrames];
        D3D_FEATURE_LEVEL FeatureLevel;

    private:
        IDXGIAdapter4* DetermineAdapter();
        bool CreateDevice();
        bool CreateCommandQueue(D3D12_COMMAND_LIST_TYPE type);
        bool CreateSwapChain(HWND hWnd, uint32_t width, uint32_t height, uint32_t bufferCount);
        bool CreateFence();
        HANDLE CreateEventHandle();
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE type);
    };
}