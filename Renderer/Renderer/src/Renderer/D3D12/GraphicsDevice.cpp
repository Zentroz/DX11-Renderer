#include"GraphicsDevice.h"
#include "Renderer/Core/ErrorLogger.h"

#include <cassert>

using namespace Microsoft::WRL;

namespace zRender {

    constexpr D3D_FEATURE_LEVEL minimunFeatureLevel{ D3D_FEATURE_LEVEL_11_0 };

    IDXGIAdapter4* D3D12Device::DetermineAdapter() {
        IDXGIAdapter4* adapter = nullptr;

        for (int i = 0; 
            m_Factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND;
            i++) {

            if (SUCCEEDED(D3D12CreateDevice(adapter, minimunFeatureLevel, __uuidof(ID3D12Device), nullptr))) {
                return adapter;
            }

            if (adapter) adapter->Release();
        }

        return nullptr;
    }
    D3D_FEATURE_LEVEL GetMaxFeatureLevel(IDXGIAdapter4* adapter) {
        D3D_FEATURE_LEVEL featureLevels[] = {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_12_0,
            D3D_FEATURE_LEVEL_12_1,
        };

        D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevelInfos{};
        featureLevelInfos.NumFeatureLevels = _countof(featureLevels);
        featureLevelInfos.pFeatureLevelsRequested = featureLevels;

        ComPtr<ID3D12Device> device;
        D3D12CreateDevice(adapter, minimunFeatureLevel, IID_PPV_ARGS(&device));
        device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featureLevelInfos, sizeof(featureLevelInfos));

        return featureLevelInfos.MaxSupportedFeatureLevel;
    }

    void D3D12Device::Initialize(void* windowHandle) {
        HWND hWnd = (HWND)windowHandle;

        RECT windowRect;

        if (!GetClientRect(hWnd, &windowRect))
        {
            assert("Couldn't get window rect.");
        }

        uint32_t width = windowRect.right - windowRect.left;
        uint32_t height = windowRect.bottom - windowRect.top;

        bool successful = true;

        successful = CreateDevice();
        successful = CreateCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
        successful = CreateSwapChain(hWnd, width, height, m_NumFrames);
        successful = CreateFence();

        return;
    }

    void D3D12Device::Resize(int newWidth, int newHeight, bool isFullScreen) {
        
    }

    void D3D12Device::Release() {
    }

    bool D3D12Device::CreateDevice() {
        HRESULT hr;
        hr = CreateDXGIFactory2(0, __uuidof(IDXGIFactory7), &m_Factory);

        if (FAILEDLOG(hr)) {
            return false;
        }

        Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter;
        adapter.Attach(DetermineAdapter());

        D3D_FEATURE_LEVEL maxFeatureLevel{ GetMaxFeatureLevel(adapter.Get()) };

        hr = D3D12CreateDevice(adapter.Get(), maxFeatureLevel, IID_PPV_ARGS(&m_Device));
        if (FAILEDLOG(hr)) {
            return false;
        }
        
        return true;
    }

    bool D3D12Device::CreateCommandQueue(D3D12_COMMAND_LIST_TYPE type) {
        ComPtr<ID3D12CommandQueue> cmdQueue;

        D3D12_COMMAND_QUEUE_DESC desc{};
        desc.Type = type;
        desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask = 0;

        if (FAILEDLOG(m_Device->CreateCommandQueue(&desc, IID_PPV_ARGS(cmdQueue.GetAddressOf())))) {
            return false;
        }

        return true;
    }

    bool D3D12Device::CreateSwapChain(HWND hWnd, uint32_t width, uint32_t height, uint32_t bufferCount) {
        ComPtr<IDXGISwapChain4> dxgiSwapChain4;
        ComPtr<IDXGIFactory4> dxgiFactory4;

        HRESULT hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgiFactory4));

        if (FAILEDLOG(hr)) return false;

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = width;
        swapChainDesc.Height = height;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.Stereo = FALSE;
        swapChainDesc.SampleDesc = { 1, 0 };
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = bufferCount;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        swapChainDesc.Flags = 0;

        ComPtr<IDXGISwapChain1> swapChain1;
        hr = dxgiFactory4->CreateSwapChainForHwnd(
            m_CommandQueue.Get(),
            hWnd,
            &swapChainDesc,
            nullptr,
            nullptr,
            swapChain1.GetAddressOf()
        );
        if (FAILEDLOG(hr)) return false;

        hr = swapChain1.As(&dxgiSwapChain4);
        if (FAILEDLOG(hr)) return false;

        return true;
    }

    ComPtr<ID3D12CommandAllocator> D3D12Device::CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE type) {
        ComPtr<ID3D12CommandAllocator> allocator;

        m_Device->CreateCommandAllocator(type, IID_PPV_ARGS(allocator.GetAddressOf()));

        return allocator;
    }

    bool D3D12Device::CreateFence() {
        if (FAILEDLOG(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_Fence.GetAddressOf())))) return false;
        return true;
    }

    HANDLE D3D12Device::CreateEventHandle()
    {
        HANDLE fenceEvent;

        fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        assert(fenceEvent && "Failed to create fence event.");

        return fenceEvent;
    }
}