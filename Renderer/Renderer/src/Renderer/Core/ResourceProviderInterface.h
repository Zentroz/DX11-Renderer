#pragma once

#include"ResourceHandles.h"

namespace zRender {

    enum Buffer_Usage { Buffer_Uasge_Default, Buffer_Uasge_Immutable, Buffer_Uasge_Dynamic, Buffer_Uasge_Staging };
    enum Buffer_CPU_AccessFlag { Buffer_CPU_None = 0, Buffer_CPU_Write = 1 << 0, Buffer_CPU_Read = 1 << 1 };
    enum class TextureFormat
    {
        // Color formats
        TextureFormat_RGBA8_UNorm,        // Standard color texture
        TextureFormat_RGBA8_sRGB,         // Albedo / color with gamma
        TextureFormat_RGBA16F,            // HDR render targets
        TextureFormat_RGBA32F,            // High precision (rare)

        // Single / dual channel
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

    enum RasterizerFillMode {
        RasterizerFunc_FillMode_Solid,
        RasterizerFunc_FillMode_Wireframe,
    };
    enum RasterizerCullMode {
        RasterizerFunc_CullMode_None,
        RasterizerFunc_CullMode_Front,
        RasterizerFunc_CullMode_Back,
    };
    
    class IRenderResourceProvider
    {
    public:
        virtual ~IRenderResourceProvider() = default;

        virtual Handle CreateBuffer(Buffer_Usage usage, int accessFlag, uint32_t byteWidth, void* initData) = 0;
        virtual Handle GetRasteriserHandle(RasterizerCullMode cullMode, RasterizerFillMode fillMode) = 0;
        //virtual Handle GetDepthStencilHandle() = 0;
    };

}