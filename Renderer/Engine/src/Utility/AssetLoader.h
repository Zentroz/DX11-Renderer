#pragma once

#include<string>
#include<D3D11Engine/ResourcesCPU.h>

class AssetLoader {
public:
	virtual ~AssetLoader() = default;
};

class MeshLoader {
public:
	MeshLoader() = default;
	bool Load(MeshCPU& mesh, const std::string& path);
};

class TextureLoader {
public:
	TextureLoader() = default;
	void FlipImage(bool flip = true);
	bool Load(TextureCPU& texture, const std::string& path);
};

class ShaderLoader {
public:
	ShaderLoader() = default;
	bool Load(ShaderCPU& shader, const std::string& path);
};