#include"Engine/AssetManager.h"

#include<Renderer/Core/ResourcesCPU.h>

using namespace zRender;

Handle AssetManager::AddMaterial(Material& mat) {
	Handle h = uuid::Build();

	m_Materials[h] = mat;

	return h;
}

Material AssetManager::GetMaterial(Handle h) {
	if (!m_Materials.contains(h)) return {};

	return m_Materials[h];
}

std::vector<Material*> AssetManager::GetAllMaterials() {
	std::vector<Material*> mats;

	for (auto& [key, mat] : m_Materials) {
		mats.push_back(&mat);
	}

	return mats;
}

Handle AssetManager::AddModel(Model& model) {
	Handle h = uuid::Build();

	m_Models[h] = model;

	return h;
}

Model AssetManager::GetModel(Handle h) {
	if (!m_Models.contains(h)) return {};

	return m_Models[h];
}