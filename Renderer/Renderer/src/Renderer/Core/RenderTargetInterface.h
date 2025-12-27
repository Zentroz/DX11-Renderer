// RenderTarget.h
#pragma once

#include <cstdint>

namespace zRender {

    enum class TextureFormat
    {
        RGBA8,
        BGRA8,
        RGBA16F,
        D24S8,
        D32F,
    };

    struct RenderTargetDesc
    {
        uint32_t width;
        uint32_t height;
        TextureFormat format;
        bool isDepth = false;
        bool allowSampling = true;
    };

    class IRenderTarget
    {
    public:
        virtual ~IRenderTarget() = default;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual TextureFormat GetFormat() const = 0;

        virtual void Resize(uint32_t width, uint32_t height) = 0;

        // For binding during rendering
        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        // Backend access (optional, controlled escape hatch)
        virtual void* GetNativeHandle() const = 0;
    };

}