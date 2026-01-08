#pragma once

#include<string>
#include<Renderer/Core/Handles.h>
#include<Renderer/Core/ResourcesCPU.h>

class AssetLoader {
public:
	virtual ~AssetLoader() = default;
};

class MeshLoader {
public:
	MeshLoader() = default;
	bool Load(zRender::MeshCPU& mesh, const std::string& path);
};

class TextureLoader {
public:
	TextureLoader() = default;
	void FlipImage(bool flip = true);
	bool Load(zRender::TextureCPU& texture, const std::string& path);
};

class ShaderLoader {
public:
	ShaderLoader() = default;
	bool Load(zRender::ShaderCPU& shader, const std::string& path);
};

struct ModelAsset {
	std::string name;

	zRender::MeshCPU* mesh = nullptr;

	struct Material {
		std::string name;

		std::vector<uint32_t> subMeshIndices;

		zRender::vec4 baseColor;
		float roughness;
		float metallic;

		zRender::TextureCPU* diffuseTexture = nullptr;
		zRender::TextureCPU* normalTexture = nullptr;
		zRender::TextureCPU* metallicTexture = nullptr;
		zRender::TextureCPU* roughnessTexture = nullptr;
	};

	std::vector<Material> materials;

	DirectX::XMMATRIX modelMatrix;

	~ModelAsset();
};

class ModelLoader {
public:
	ModelLoader() = default;
	bool Load(ModelAsset& model, const std::string& path);
};