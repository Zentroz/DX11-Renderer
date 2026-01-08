#pragma once

#include<vector>
#include<unordered_map>
#include<Renderer/Core/Handles.h>

struct EngineMaterial {
	void* shaderData;
	uint64_t shaderDataByteWidth;
	ShaderHandle shaderHandle;
	BufferHandle shaderBufferHandle;
	std::vector<TextureHandle> textures;

	EngineMaterial() 
		: shaderData(nullptr), shaderDataByteWidth(0), shaderHandle(InvalidHandle), shaderBufferHandle(InvalidHandle), textures({}) 
	{}
};

class MaterialManager {
public:
	MaterialManager() = default;

	Handle Add(const EngineMaterial& mat);
	EngineMaterial Get(Handle handle);

private:
	std::unordered_map<Handle, EngineMaterial> m_Materials;
	uint64_t m_MaterialCounts;
};