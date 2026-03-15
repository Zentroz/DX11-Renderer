#pragma once

#include<d3d11.h>
#include<d3dcompiler.h>
#include<d3d11shader.h>
#include<wrl.h>
#include<memory>
#include<unordered_map>

#include"Renderer/Core/Resources.h"
#include"Renderer/D3D11/Resources.h"
#include"Renderer/D3D11/ResourceStorage.h"
#include"Renderer/Render/RenderPassInterface.h"
#include"Renderer/Render/ResourceProviderInterface.h"

namespace zRender {

	class D3D11ResourceProvider : public IRenderResourceProvider {
	public:
		D3D11ResourceProvider(ID3D11Device* pDevice, ID3D11Texture2D* backBufferTexture);
		~D3D11ResourceProvider() = default;

		D3D11ResourceStorage* GetStorage() { return &storage; }

		void ReleaseScreenTexture();
		void RecreateScreenTextureHandle();

		MeshHandle LoadMesh(const Mesh& rawMesh) override;
		TextureHandle LoadTexture(const Texture& rawTexture) override;
		TextureHandle LoadTextureCubeMap(const Texture rawTexture[6]);

		TextureHandle CreateTexture(int width, int height, TextureUsageFlags usageFlags, TextureFilter filter, vec4 initialColor) override;
		TextureHandle CreateTexture(int width, int height, zRender::TextureFormat format, TextureUsageFlags usageFlags, TextureFilter filter) override;

		BufferHandle CreateBuffer(zRender::Buffer_Usage usage, int accessFlag, UINT byteWidth, void* initData) override;

		ID3D11DepthStencilView* CreateDepthStencilView();

		PipelineHandle CreatePipeline(const PipelineDesc& desc) override;

	private:
		ID3D11Device* device;
		D3D11ResourceStorage storage{};

		ID3D11Texture2D* backBufferTexture = nullptr;

	private:
		TextureHandle CreateTextureResource(ID3D11Texture2D* texture, TextureFormat format, TextureUsageFlags usageFlags, TextureFilter filter);
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> CreateRasterizer(RasterizerCullMode cullMode, RasterizerFillMode fillMode);
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> CreateDepthStencilState(DepthWriteMask write, DepthFunc func);
		Microsoft::WRL::ComPtr<ID3D11BlendState> CreateBlendState(bool blendEnable);
		Microsoft::WRL::ComPtr<ID3D11SamplerState> CreateSamplerState(TextureFilter sampleMode);
		Microsoft::WRL::ComPtr<ID3D11InputLayout> CreateInputLayout(Microsoft::WRL::ComPtr<ID3DBlob> vsBlob);
		Microsoft::WRL::ComPtr<ID3D11VertexShader> LoadVertexShader(const std::string& vertexShaderSrc, Microsoft::WRL::ComPtr<ID3DBlob>& blob);
		Microsoft::WRL::ComPtr<ID3D11PixelShader> LoadPixelShader(const std::string& pixelShaderSrc, Microsoft::WRL::ComPtr<ID3DBlob>& blob);
	};
}