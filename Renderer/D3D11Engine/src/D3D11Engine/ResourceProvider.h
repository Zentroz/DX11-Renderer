#pragma once

#include<d3d11.h>
#include<d3dcompiler.h>
#include<d3d11shader.h>
#include<wrl/client.h>
#include<memory>
#include<unordered_map>

#include"D3D11Resources.h"
#include"ResourcesCPU.h"
#include"GraphicsDevice.h"
#include<Renderer/Core/ResourceProviderInterface.h>
#include<Renderer/Core/PipelineStateContainer.h>

using Handle = uint32_t;

enum InputLayout { InputLayout_PNTT, InputLayout_P };

class D3D11ResourceProvider : public zRender::IRenderResourceProvider {
private:
	template <typename T>
	using ResourceMap = std::unordered_map<Handle, std::shared_ptr<T>>;

public:
	D3D11ResourceProvider(D3D11Device* pDevice);

	Handle LoadMesh(const MeshCPU& rawMesh);
	Handle LoadShader(const ShaderCPU& rawShader);
	Handle LoadTexture(const TextureCPU& rawTexture);
	Handle LoadTextureCubeMap(const TextureCPU rawTexture[6]);

	Handle CreateTexture(int width, int height, zRender::TextureFormat format);

	Handle CreateBuffer(zRender::Buffer_Usage usage, int accessFlag, UINT byteWidth, void* initData) override;
	ID3D11Buffer* GetBuffer(Handle h);
	Handle GetRasteriserHandle(zRender::RasterizerCullMode cullMode, zRender::RasterizerFillMode fillMode) override;
	ID3D11RasterizerState* GetRasterizerState();
	Handle CreateRasterizer(zRender::RasterizerCullMode cullMode, zRender::RasterizerFillMode fillMode);

	Handle CreateDepthStencilState();
	ID3D11DepthStencilState* GetDepthStencilState(Handle handle);
	ID3D11DepthStencilView* CreateDepthStencilView();

	template <typename T>
	T* GetResource(Handle handle);

private:
	D3D11Device* device = nullptr;

	ResourceMap<D3D11Mesh> m_MeshMap;
	ResourceMap<D3D11Shader> m_ShaderMap;
	ResourceMap<D3D11Texture> m_TextureMap;
	std::unordered_map<Handle, ID3D11Buffer*> m_BufferMap;
	std::unordered_map<Handle, ID3D11RenderTargetView*> m_RenderTargetMap;
	std::unordered_map<Handle, ID3D11RasterizerState*> m_RasterizerStateMap;
	std::unordered_map<Handle, ID3D11DepthStencilState*> m_DepthStencilStateMap;
};

template <typename T>
T* D3D11ResourceProvider::GetResource(Handle h) {
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