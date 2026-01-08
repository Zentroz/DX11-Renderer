#pragma once

#include<vector>
#include<string>
#include<DirectXMath.h>

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

	struct SubMesh {
		std::string name;

		uint32_t vertexOffset;
		uint32_t vertexCount;
		uint32_t indexOffset;
		uint32_t indexCount;

		DirectX::XMMATRIX localModel;
	};

	struct MeshCPU : public ResourceCPU {
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		std::vector<SubMesh> subMeshes;
	};

	struct ShaderCPU : public ResourceCPU {
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