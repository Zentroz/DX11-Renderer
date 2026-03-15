#include"Engine/Scene.h"
#include"Engine/Components.h"

std::vector<zRender::RenderItem> Scene::GenerateDrawCalls() {
	std::vector<zRender::RenderItem> drawCalls;
	
	auto view = registry.view<const TransformComponent, const MeshFilter, const MaterialComponent>();

	view.each(
		[&](const auto e, const TransformComponent& t, const MeshFilter& mf, const MaterialComponent& mc) {
			drawCalls.push_back({});
			auto& item = drawCalls.back();

			// Mesh
			//item.subMeshIndex = mf.subMeshIndex;
			item.baseVertexLocation = mf.vertexOffset;
			item.baseIndexLocation = mf.indexOffset;
			item.indexCount = mf.indexCount;
			item.meshHandle = mf.mesh;
			item.modelMatrix = CalculateGlobalModelMatrix(e);
			item.flags = 0;

			// Material
			item.materialData.baseColor = mc.baseColor;
			item.materialData.roughness = mc.roughness;
			item.materialData.metallic = mc.metallic;
			item.materialData.aplhaCutoff = mc.alphaCutoff;
			item.materialData.aplhaClipping = mc.alphaClipping;
			item.materialData.surfaceType = mc.surfaceType;

			item.materialData.textureHandles.push_back(mc.albedoTexture);
			item.materialData.textureHandles.push_back(mc.normalTexture);
			item.materialData.textureHandles.push_back(mc.ormTexture);
		}
	);

	return drawCalls;
}

DirectX::XMMATRIX Scene::CalculateGlobalModelMatrix(entt::entity entity) {
	if (!registry.valid(entity)) {
		return DirectX::XMMatrixIdentity();
	}

	const TransformComponent& t = registry.get<const TransformComponent>(entity);

	return CalculateGlobalModelMatrix(t.parent) * t.Model();
}