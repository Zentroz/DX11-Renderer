#pragma once

#include<d3d11.h>
#include"Renderer/D3D11/Resources.h"
#include"Renderer/Render/ResourceStorageInterface.h"

struct D3D11ResourceStorage : public IResourceStorage {
public:
	// Add Functions
	Handle AddBuffer(ID3D11Buffer* buffer) {
		uuid id = uuid::Build();

		m_BufferMap[id] = buffer;
		return id;
	}
	Handle AddPipeline(const D3D11Pipeline& pipeline) {
		uuid id = uuid::Build();

		m_PipelineMap[id] = std::make_shared<D3D11Pipeline>(pipeline);
		return id;
	}
	Handle AddTexture(D3D11Texture texture) {
		uuid id = uuid::Build();

		m_TextureMap[id] = std::make_shared<D3D11Texture>(texture);
		return id;
	}
	Handle AddMesh(D3D11Mesh mesh) {
		uuid id = uuid::Build();

		m_MeshMap[id] = std::make_shared<D3D11Mesh>(mesh);
		return id;
	}

	// Delete Functions
	void RemoveBuffer(BufferHandle handle) {
		if (!m_BufferMap.contains(handle)) ThrowMissingResource(handle, "Buffer");

		m_BufferMap.erase(handle);
	}
	void RemovePipeline(PipelineHandle handle) {
		if (!m_PipelineMap.contains(handle)) ThrowMissingResource(handle, "Pipeline");

		m_PipelineMap.erase(handle);
	}
	void RemoveTexture(TextureHandle handle) {
		if (!m_TextureMap.contains(handle)) ThrowMissingResource(handle, "Texture");

		m_TextureMap.erase(handle);
	}
	void RemoveMesh(MeshHandle handle) {
		if (!m_MeshMap.contains(handle)) ThrowMissingResource(handle, "Mesh");

		m_MeshMap.erase(handle);
	}

	// Retrive Functions
	ID3D11Buffer* GetBuffer(BufferHandle handle) {
		if (!m_BufferMap.contains(handle)) ThrowMissingResource(handle, "Buffer");

		return m_BufferMap[handle].Get();
	}
	D3D11Pipeline* GetPipeline(PipelineHandle handle) {
		if (!m_PipelineMap.contains(handle)) ThrowMissingResource(handle, "Pipeline");

		return m_PipelineMap[handle].get();
	}
	D3D11Texture* GetTexture(TextureHandle handle) {
		if (!m_TextureMap.contains(handle)) ThrowMissingResource(handle, "Texture");

		return m_TextureMap[handle].get();
	}
	D3D11Mesh* GetMesh(MeshHandle handle) {
		if (!m_MeshMap.contains(handle)) ThrowMissingResource(handle, "Mesh");

		return m_MeshMap[handle].get();
	}

private:
	ResourceMapComPtr<ID3D11Buffer> m_BufferMap;
	ResourceMapComPtr<ID3D11SamplerState> m_SamplerStates;
	ResourceMapSharedPtr<D3D11Texture> m_TextureMap;
	ResourceMapSharedPtr<D3D11Pipeline> m_PipelineMap;
	ResourceMapSharedPtr<D3D11Mesh> m_MeshMap;
};