#pragma once

#include<stdint.h>
#include"Renderer/Core/uuid.h"

using Handle = uuid;
using MeshHandle = uuid;
using ShaderHandle = uuid;
using BufferHandle = uuid;
using TextureHandle = uuid;
using RasterizerHandle = uuid;
using DepthStateHandle = uuid;

struct RenderResourceHandle
{
	TextureHandle texture;
	TextureHandle srv;
	Handle dsv;
	Handle rtv;
};