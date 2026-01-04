#pragma once

#include<d3d11.h>
#include<d3dcompiler.h>
#include<d3d11shader.h>
#include<wrl/client.h>
#include<memory>
#include<unordered_map>

#include"Renderer/Core/ResourcesCPU.h"
#include"Renderer/Core/Handles.h"
#include"Renderer/D3D11/GraphicsDevice.h"
#include"Renderer/D3D11/D3D11Resources.h"
#include"Renderer/Render/ResourceProviderInterface.h"
#include"Renderer/Render/PipelineStateContainer.h"

namespace zRender {
	enum InputLayout { InputLayout_None, InputLayout_PNTT, InputLayout_P };

	class D3D11ResourceProvider : public IRenderResourceProvider {
	private:
		template <typename T>
		using ResourceMap = std::unordered_map<uint32_t, std::shared_ptr<T>>;

	public:
		D3D11ResourceProvider(D3D11Device* pDevice);

		MeshHandle LoadMesh(const MeshCPU& rawMesh);
		ShaderHandle LoadShader(const ShaderCPU& rawShader);
		TextureHandle LoadTexture(const TextureCPU& rawTexture);
		TextureHandle LoadTextureCubeMap(const TextureCPU rawTexture[6]);

		TextureHandle CreateTexture(int width, int height, zRender::TextureFormat format, TextureUsageFlags usageFlags) override;

		BufferHandle CreateBuffer(zRender::Buffer_Usage usage, int accessFlag, UINT byteWidth, void* initData) override;
		ID3D11Buffer* GetBuffer(BufferHandle h);
		RasterizerHandle GetRasteriserHandle(RasterizerCullMode cullMode, RasterizerFillMode fillMode) override;
		RasterizerHandle CreateRasterizer(RasterizerCullMode cullMode, RasterizerFillMode fillMode);
		ID3D11RasterizerState* GetRasterizerState(RasterizerHandle handle);

		DepthStateHandle CreateDepthStencilState(DepthWriteMask write, DepthFunc func);
		DepthStateHandle GetDepthStateHandle(DepthWriteMask write, DepthFunc func) override;
		ID3D11DepthStencilState* GetDepthStencilState(DepthStateHandle handle);
		ID3D11DepthStencilView* CreateDepthStencilView();

		template <typename T>
		T* GetResource(uint32_t handle);

	private:
		D3D11Device* device = nullptr;

		ResourceMap<D3D11Mesh> m_MeshMap;
		ResourceMap<D3D11Shader> m_ShaderMap;
		ResourceMap<D3D11Texture> m_TextureMap;
		std::unordered_map<uint32_t, ID3D11RenderTargetView*> m_RenderTargetMap;
		std::unordered_map<BufferHandle, ID3D11Buffer*> m_BufferMap;
		std::unordered_map<DepthStateHandle, ID3D11DepthStencilState*> m_DepthStencilStateMap;
		std::unordered_map<RasterizerHandle, ID3D11RasterizerState*> m_RasterizerStateMap;

	private:
		TextureHandle CreateTextureResource(ID3D11Texture2D* texture, TextureUsageFlags usageFlags);
	};

	template <typename T>
	T* D3D11ResourceProvider::GetResource(uint32_t h) {
		T* resource = nullptr;
		if (h == InvalidHandle) return resource;

		if (typeid(T) == typeid(D3D11Mesh)) {
			if (!m_MeshMap.contains(h)) return resource;
			resource = static_cast<T*>(static_cast<D3D11Resource*>(m_MeshMap[h].get()));
		}
		else if (typeid(T) == typeid(D3D11Shader)) {
			if (!m_ShaderMap.contains(h)) return resource;
			resource = static_cast<T*>(static_cast<D3D11Resource*>(m_ShaderMap[h].get()));
		}
		else if (typeid(T) == typeid(D3D11Texture)) {
			if (!m_TextureMap.contains(h)) return resource;
			resource = static_cast<T*>(static_cast<D3D11Resource*>(m_TextureMap[h].get()));
		}

		return resource;
	}
}