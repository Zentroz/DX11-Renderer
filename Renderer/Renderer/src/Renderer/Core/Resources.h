#pragma once

#include<d3d11.h>
#include<vector>
#include<string>
#include<DirectXMath.h>

#include"Renderer/Core/Math.h"
#include"Renderer/Core/Handles.h"

namespace zRender {
	struct Resource {
		std::string name;

		virtual ~Resource() = default;
		virtual void Release() = 0;
	};

	struct SubMeshGPU {
		UINT vertexOffset;
		UINT vertexCount;

		UINT indexOffset;
		UINT indexCount;
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

		int parentSubMeshIndex = -1;
		DirectX::XMMATRIX localModel;
	};

	struct Mesh : public Resource {
		// Raw
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<SubMesh> subMeshes;

		// GPU
		MeshHandle gpuResource;
		std::vector<SubMeshGPU> subMeshesGPU;

		void Release() override {
			vertices.clear();
			indices.clear();
			subMeshes.clear();
		}
	};

	struct Shader : public Resource {
		std::string vertexShaderSrc;
		std::string pixelShaderSrc;

		ShaderHandle gpuResource;

		void Release() override {
			vertexShaderSrc.clear();
			pixelShaderSrc.clear();
		}
	};

	struct Pixel {
		unsigned char r, g, b, a;
	};

	struct Texture : public Resource {
		// Raw
		Pixel* pixels;
		int width, height, channels;
		uint32_t usageFlags;
		enum FilterMode {
			Point = 0,
			Linear = 1
		} filterMode;

		// GPU
		TextureHandle gpuResource;

		void Release() override {
			if (pixels) {
				delete pixels;
				pixels = nullptr;
			}
		}
	};
}