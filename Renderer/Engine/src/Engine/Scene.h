#pragma once

#include<vector>
#include<DirectXMath.h>
#include<Renderer/Render/RenderItem.h>
#include<Renderer/D3D11/ResourceProvider.h>

#include"Engine/MaterialManager.h"
#include"Engine/AssetManager.h"

struct Entity {
	std::string name;
	Handle model;
	DirectX::XMMATRIX modelMatrix;

	zRender::vec4 baseColor;
	float roughness;
	float metallic;
};

class Scene {
public:
	Scene() = default;

	void AddEntity(Entity& entity);
	bool FindEntity(std::string name, Entity& entity);

	AssetManager& GetAssetManager() { return assetManager; }

	std::vector<zRender::RenderItem> GenerateDrawCalls();

private:
	std::vector<Entity> m_Entities;
	AssetManager assetManager;
};