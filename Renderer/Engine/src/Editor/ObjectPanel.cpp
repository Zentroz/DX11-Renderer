#include"Editor/ObjectPanel.h"
#include<imgui/imgui.h>

void ObjectPanel::Draw(std::vector<Material*> drawMaterials) {
	if (ImGui::Begin("Objects")) {
		for (auto mat : drawMaterials) {
			if (ImGui::CollapsingHeader(mat->name.c_str())) {
				// Color
				float m_ColorInput[3] = { mat->baseColor.x, mat->baseColor.y, mat->baseColor.z };
				ImGui::Text("Color");
				ImGui::SameLine();
				if (ImGui::ColorEdit3(("##oColor" + mat->name).c_str(), m_ColorInput)) {
					mat->baseColor = zRender::vec4(m_ColorInput[0], m_ColorInput[1], m_ColorInput[2], 1);
				}

				// Roughness
				ImGui::Text("Roughness");
				ImGui::SameLine();
				ImGui::SliderFloat(("##oRoughness" + mat->name).c_str(), &mat->roughness, 0, 1);

				// Metallic
				ImGui::Text("Metallic");
				ImGui::SameLine();
				ImGui::SliderFloat(("##oMetallic" + mat->name).c_str(), &mat->metallic, 0, 1);
			}
		}
	}
	ImGui::End();
}