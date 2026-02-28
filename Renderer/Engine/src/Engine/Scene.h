#pragma once

#include<vector>
#include<DirectXMath.h>
#include<Renderer/Render/RenderItem.h>
#include<Renderer/D3D11/ResourceProvider.h>
#include<entt/entt.hpp>

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

	entt::registry& GetRegistry() { return registry; }
	AssetManager& GetAssetManager() { return assetManager; }

	std::vector<zRender::RenderItem> GenerateDrawCalls();

private:
	entt::registry registry{};
	AssetManager assetManager;

private:
	DirectX::XMMATRIX CalculateGlobalModelMatrix(entt::entity entity);
};