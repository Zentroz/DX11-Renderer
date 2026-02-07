#pragma once

#include<string>
#include<unordered_map>
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

	struct LoadedTexture {
		enum TextureType {
			TextureType_Albedo, TextureType_Normal, TextureType_Metal, TextureType_Rough, 
			TextureType_RoughMetal_RG, TextureType_MetalRough_RG, TextureType_ORM_RGB
		} type;
		std::string name;
		
	};

	std::unordered_map<std::string, zRender::TextureCPU*> textures;

	struct Material {
		std::string name;

		enum RenderMode {
			Opaque,
			AplhaTest,
			Transparent
		} renderMode;

		std::vector<uint32_t> subMeshIndices;

		zRender::vec4 baseColor;
		float roughnessFactor;
		float metallicFactor;
		float aplhaCutoff;

		std::string albedoTextureName;
		std::string normalTextureName;
		std::string rmTextureName;
	};

	std::vector<Material> materials;

	DirectX::XMMATRIX modelMatrix;

	~ModelAsset();
	void Dispose();
};

class ModelLoader {
public:
	ModelLoader() = default;
	bool Load(ModelAsset& model, const std::string& path);
};