#pragma once

#include<d3d11.h>
#include<vector>
#include<string>
#include<DirectXMath.h>

#include"Renderer/Core/Math.h"

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
		std::vector<SubMeshGPU> subMeshesGPU;
		ID3D11Buffer* vertexBuffer = nullptr;
		ID3D11Buffer* indexBuffer = nullptr;
		UINT vertexStride = 0;

		void Release() override {
			vertices.clear();
			indices.clear();
			subMeshes.clear();

			subMeshesGPU.clear();
			if (vertexBuffer) {
				vertexBuffer->Release();
				vertexBuffer = nullptr;
			}
			if (indexBuffer) {
				indexBuffer->Release();
				indexBuffer = nullptr;
			}
		}
	};

	struct Shader : public Resource {
		// Raw
		std::string vertexShaderSrc;
		std::string pixelShaderSrc;
		uint32_t inputLayoutFlag;

		// GPU
		ID3D11VertexShader* vertexShader = nullptr;
		ID3D11PixelShader* pixelShader = nullptr;
		ID3D11InputLayout* inputLayout = nullptr;

		void Release() override {
			vertexShaderSrc.clear();
			pixelShaderSrc.clear();

			if (vertexShader) {
				vertexShader->Release();
				vertexShader = nullptr;
			}
			if (pixelShader) {
				pixelShader->Release();
				pixelShader = nullptr;
			}
			if (inputLayout) {
				inputLayout->Release();
				inputLayout = nullptr;
			}
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
		ID3D11Texture2D* texture = nullptr;
		ID3D11ShaderResourceView* shaderResourceView = nullptr;
		ID3D11RenderTargetView* renderTargetView = nullptr;
		ID3D11DepthStencilView* depthStencilView = nullptr;
		ID3D11SamplerState* samplerState = nullptr;

		void Release() override {
			if (pixels) {
				delete pixels;
				pixels = nullptr;
			}

			if (texture) {
				texture->Release();
				texture = nullptr;
			}
			if (renderTargetView) {
				renderTargetView->Release();
				renderTargetView = nullptr;
			}
			if (depthStencilView) {
				depthStencilView->Release();
				depthStencilView = nullptr;
			}
			if (shaderResourceView) {
				shaderResourceView->Release();
				shaderResourceView = nullptr;
			}
			if (samplerState) {
				samplerState->Release();
				samplerState = nullptr;
			}
		}
	};
}