#pragma once

#include<d3d11.h>

namespace zRender {
	struct D3D11Resource {
		virtual ~D3D11Resource() = default;
		virtual void Release() = 0;
	};

	struct SubMeshGPU {
		UINT vertexOffset;
		UINT vertexCount;

		UINT indexOffset;
		UINT indexCount;
	};

	struct D3D11Mesh : public D3D11Resource {
		ID3D11Buffer* vertexBuffer = nullptr;
		ID3D11Buffer* indexBuffer = nullptr;

		UINT vertexStride = 0;

		std::vector<SubMeshGPU> subMeshes;

		void Release() override {
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

	struct D3D11Shader : public D3D11Resource {
		ID3D11VertexShader* vertexShader = nullptr;
		ID3D11PixelShader* pixelShader = nullptr;
		ID3D11InputLayout* inputLayout = nullptr;

		void Release() override {
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

	struct D3D11Texture : public D3D11Resource {
		ID3D11Texture2D* texture = nullptr;
		ID3D11ShaderResourceView* shaderResourceView = nullptr;
		ID3D11RenderTargetView* renderTargetView = nullptr;
		ID3D11DepthStencilView* depthStencilView = nullptr;
		ID3D11SamplerState* samplerState = nullptr;

		void Release() override {
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