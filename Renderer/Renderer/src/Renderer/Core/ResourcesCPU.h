#pragma once

#include<vector>
#include<string>

#include"Renderer/Core/Math.h"

namespace zRender {
	struct ResourceCPU {
		virtual ~ResourceCPU() = default;
	};

	struct Vertex {
		vec3 position;
		vec3 normal;
		vec3 tangent;
		vec2 uv;
	};

	struct MeshCPU : public ResourceCPU {
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		uint64_t indexCount;
	};

	struct ShaderCPU {
		std::string vertexShaderSrc;
		std::string pixelShaderSrc;
		uint32_t inputLayout;
	};

	struct TextureCPU : public ResourceCPU {
		unsigned char* pixels;
		int width, height, channels;
		uint32_t usageFlags;
	};
}