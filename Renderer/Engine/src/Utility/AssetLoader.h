#pragma once

#include<string>
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