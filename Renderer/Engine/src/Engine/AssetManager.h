#pragma once

#include<iostream>
#include<unordered_map>
#include<Renderer/Core/Handles.h>
#include<Renderer/Core/ResourcesCPU.h>

#include"Utility/AssetLoader.h"

struct Material {
	std::string name;

	ShaderHandle shaderHandle{};
	TextureHandle albedo{};
	TextureHandle normal{};
	TextureHandle orm{}; // R = AO, G = Roughness, B = Metallic

	zRender::vec4 baseColor;
	float roughnessFactor;
	float metallicFactor;
	float aplhaCutoff;

	enum RenderMode { Opaque, AplhaTest, Transparent } renderMode;
};

struct Model {
	MeshHandle meshHandle{};
	
	struct SubMesh {
		uint32_t submeshIndex;
		Handle materialHandle{};
		DirectX::XMMATRIX localModelMatrix;
	};

	std::vector<SubMesh> subMeshes;
};

class AssetManager {
public:
	AssetManager() = default;

	template<typename T>
	T* Get(Handle h);

	template<typename T>
	Handle Add(std::shared_ptr<T> asset);

	Handle AddModel(Model& model);
	Model GetModel(Handle handle);

	Handle AddMaterial(Material& mat);
	Material GetMaterial(Handle handle);
	std::vector<Material*> GetAllMaterials();

private:
	std::unordered_map<Handle, std::shared_ptr<zRender::ResourceCPU>> m_Assets;
	std::unordered_map<Handle, Material> m_Materials;
	std::unordered_map<Handle, Model> m_Models;
	Handle m_MaterialCount;
	Handle m_ModelCounts;
	Handle m_AssetCount;
};

template<typename T>
T* AssetManager::Get(Handle h) {
	if (!m_Assets.contains(h)) return nullptr;

	return static_cast<T*>(m_Assets[h].get());
}

template<typename T>
Handle AssetManager::Add(std::shared_ptr<T> asset) {
	Handle h = uuid::Build();

	m_Assets[h] = static_cast<std::shared_ptr<zRender::ResourceCPU>>(asset);

	return h;
}