#pragma once

#include<Renderer/Core/Math.h>
#include<Renderer/Core/Resources.h>
#include<Renderer/Core/uuid.h>
#include<Renderer/Render/RenderItem.h>
#include<vector>

struct TransformComponent {
	zRender::vec3 position;
	zRender::vec4 rotation;
	zRender::vec3 scale;

	entt::entity parent;

	TransformComponent() = default;
	TransformComponent(zRender::vec3 position, zRender::vec4 rotation, zRender::vec3 scale)
		: position(position), rotation(rotation), scale(scale)
	{}

	DirectX::XMMATRIX Model() const {
		return DirectX::XMMatrixScaling(scale.x, scale.y, scale.z)
			* DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(rotation.x, rotation.y, rotation.z, rotation.w))
			* DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	}
};

struct MeshFilter {
	uuid mesh{};
	//uint16_t subMeshIndex {};

	uint32_t vertexCount{};
	uint32_t vertexOffset{};

	uint32_t indexCount{};
	uint32_t indexOffset{};

	MeshFilter() = default;
	MeshFilter(uuid mesh, uint32_t vertexOffset, uint32_t vertexCount, uint32_t indexOffset, uint32_t indexCount)
		: mesh(mesh), vertexCount(vertexCount), vertexOffset(vertexOffset), indexOffset(indexOffset), indexCount(indexCount)
	{}
};

struct MaterialComponent {
	zRender::vec4 baseColor;

	uuid albedoTexture;
	uuid normalTexture;
	uuid ormTexture;

	float roughness;
	float metallic;
	float toughness;
	float alphaCutoff;

	SurfaceType surfaceType;

	bool alphaClipping;
};