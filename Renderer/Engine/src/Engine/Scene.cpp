#include"Engine/Scene.h"

std::vector<zRender::RenderItem> Scene::GenerateDrawCalls() {
	std::vector<zRender::RenderItem> drawCalls;

	for (auto& e : m_Entities) {
		Model model = assetManager.GetModel(e.model);

		for (uint32_t i = 0; i < model.subMeshes.size(); i++) {
			Model::SubMesh& sub = model.subMeshes[i];
			Material material;

			drawCalls.push_back({});
			auto& item = drawCalls.back();

			// Mesh
			item.subMeshIndex = sub.submeshIndex;
			item.meshHandle = model.meshHandle;
			item.modelMatrix = sub.localModelMatrix * e.modelMatrix;
			item.flags = 0;
			
			// Material
			Material mat = assetManager.GetMaterial(sub.materialHandle);

			item.materialData.shaderHandle = mat.shaderHandle;
			item.materialData.baseColor = mat.baseColor;
			item.materialData.roughness = mat.roughness;
			item.materialData.metallic = mat.metallic;
		}
	}

	return drawCalls;
}

void Scene::AddEntity(Entity& e) {
	m_Entities.push_back(e);
}

bool Scene::FindEntity(std::string name, Entity& e) {
	for (auto& en : m_Entities) {
		if (en.name == name) {
			e = en;
			return true;
		}
	}

	return false;
}