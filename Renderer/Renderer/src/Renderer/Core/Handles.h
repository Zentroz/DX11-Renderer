#pragma once

#include<stdint.h>

using Handle = uint32_t;
using MeshHandle = uint32_t;
using ShaderHandle = uint32_t;
using BufferHandle = uint32_t;
using TextureHandle = uint32_t;
using RasterizerHandle = uint32_t;
using DepthStateHandle = uint32_t;

constexpr uint32_t InvalidHandle = 0;

struct RenderResourceHandle
{
	TextureHandle texture = InvalidHandle;
	TextureHandle srv = InvalidHandle;
	Handle dsv = InvalidHandle;
	Handle rtv = InvalidHandle;
};