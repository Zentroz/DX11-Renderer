#pragma once

#include"Renderer/Core/Handles.h"
#include"Renderer/Core/Helpers.h"

namespace zRender {

    enum Buffer_Usage { Buffer_Uasge_Default, Buffer_Uasge_Immutable, Buffer_Uasge_Dynamic, Buffer_Uasge_Staging };
    enum Buffer_CPU_AccessFlag { Buffer_CPU_None = 0, Buffer_CPU_Write = 1 << 0, Buffer_CPU_Read = 1 << 1 };
    enum class TextureUsageFlags : uint32_t {
        TextureUsageFlag_None = 0,
        TextureUsageFlag_ShaderResource = 1 << 0,
        TextureUsageFlag_RenderTarget = 1 << 1,
        TextureUsageFlag_DepthStencil = 1 << 2

    };
    FLAG_ENUM_OVERLOADS(TextureUsageFlags)
    enum TextureFormat
    {
        // Color formats
        TextureFormat_RGBA8_UNorm,        // Standard color texture
        TextureFormat_RGBA8_sRGB,         // Albedo / color with gamma
        TextureFormat_RGBA16F,            // HDR render targets
        TextureFormat_RGBA32F,            // High precision (rare)
        TextureFormat_RGB10A2_UNorm,

        // Single / dual channel
        TextureFormat_R32_Typeless,
        TextureFormat_R8_UNorm,           // Masks, grayscale
        TextureFormat_R16F,
        TextureFormat_R32F,
        TextureFormat_RG16F,
        TextureFormat_RG32F,

        // Depth formats
        TextureFormat_Depth16,
        TextureFormat_Depth24Stencil8,
        TextureFormat_Depth32,

        // Special / fallback
        TextureFormat_Unknown
    };
    enum TextureFilter {
        Point,
        Linear
    };

    enum RasterizerFillMode {
        RasterizerFunc_FillMode_Solid,
        RasterizerFunc_FillMode_Wireframe,
    };
    enum RasterizerCullMode {
        RasterizerFunc_CullMode_None,
        RasterizerFunc_CullMode_Front,
        RasterizerFunc_CullMode_Back,
    };
    enum DepthWriteMask {
        DepthWriteMask_All,
        DepthWriteMask_Zero
    };
    enum DepthFunc {
        DepthFunc_Always,
        DepthFunc_Never,
        DepthFunc_LessEqual
    };
    
    class IRenderResourceProvider
    {
    public:
        virtual ~IRenderResourceProvider() = default;

        virtual TextureHandle CreateTexture(int width, int height, TextureFormat format, TextureUsageFlags usageFlags, TextureFilter filter) = 0;
        virtual BufferHandle CreateBuffer(Buffer_Usage usage, int accessFlag, uint32_t byteWidth, void* initData) = 0;
        virtual RasterizerHandle GetRasteriserHandle(RasterizerCullMode cullMode, RasterizerFillMode fillMode) = 0;
        virtual DepthStateHandle GetDepthStateHandle(DepthWriteMask write, DepthFunc func) = 0;

        TextureHandle GetScreenTextureHandle() { return screenTextureHandle; }

        void SetDefaultGeometryShaderHandle(ShaderHandle handle) { geometryShaderHandle = handle; };
        void SetDefaultLightingShaderHandle(ShaderHandle handle) { lightingShaderHandle = handle; };
        ShaderHandle GetDefaultGeometryShaderHandle() { return geometryShaderHandle; };
        ShaderHandle GetDefaultLightingShaderHandle() { return lightingShaderHandle; };

        void AddPipelineStateContainer(PipelineStateContainer state) { pipelineStates.push_back(state); }
        PipelineStateContainer GetPipelineStateContainer(std::string name) {
            for (auto& state : pipelineStates) {
                if (state.name == name) return state;
            }
        }

        void AddNamedResourceHandle(std::string name, Handle handle) { m_NamedResourceHandles.push_back({ name, handle }); }
        Handle GetNamedResourceHandle(std::string name) {
            for (auto& h : m_NamedResourceHandles) {
                if (h.name == name) return h.handle;
            }
        }

    protected:
        TextureHandle screenTextureHandle;
        std::vector<PipelineStateContainer> pipelineStates;
    private:
        struct NamedResourceHandle {
            std::string name;
            Handle handle;
        };
        ShaderHandle geometryShaderHandle;
        ShaderHandle lightingShaderHandle;

        std::vector<NamedResourceHandle> m_NamedResourceHandles;
    };

}