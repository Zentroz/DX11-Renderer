#pragma once

#include"Core/Handles.h"

namespace zRender {

	class ResourceGPU {
	public:
		virtual ~ResourceGPU() = default;
	};
	class MeshGPU : public ResourceGPU {
	public:
		virtual ~MeshGPU() = default;
	};
	class ShaderGPU : public ResourceGPU {
	public:
		virtual ~ShaderGPU() = default;
	};
	class MaterialGPU : public ResourceGPU {
	public:
		virtual ~MaterialGPU() = default;
	};
	class TextureGPU : public ResourceGPU {
	public:
		virtual ~TextureGPU() = default;
	};

}