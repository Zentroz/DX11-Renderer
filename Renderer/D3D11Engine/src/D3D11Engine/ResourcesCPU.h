#pragma once

#include<vector>
#include<string>

#include"BMath.h"

struct ResourceCPU {
	virtual ~ResourceCPU() = default;
};

struct Vertex {
	float3 position;
	float3 normal;
	float3 tangent;
	float2 uv;
};

struct MeshCPU : public ResourceCPU {
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	uint64_t indexCount;
};

struct ShaderCPU {
	std::string vertexShaderSrc;
	std::string pixelShaderSrc;
	uint32_t inputLayout;
};

struct TextureCPU : public ResourceCPU {
	unsigned char* pixels;
	int width, height, channels;
};