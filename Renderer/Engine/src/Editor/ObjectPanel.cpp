#include"Editor/ObjectPanel.h"
#include<imgui/imgui.h>

void ObjectPanel::Draw(std::vector<Material*> drawMaterials) {
	if (ImGui::Begin("Objects")) {
		for (auto mat : drawMaterials) {
			ImGui::PushID(mat->name.c_str());
			if (ImGui::CollapsingHeader(mat->name.c_str())) {
				// Color
				float m_ColorInput[3] = { mat->baseColor.x, mat->baseColor.y, mat->baseColor.z };
				ImGui::Text("Color");
				ImGui::SameLine();
				if (ImGui::ColorEdit3("##oColor", m_ColorInput)) {
					mat->baseColor = zRender::vec4(m_ColorInput[0], m_ColorInput[1], m_ColorInput[2], 1);
				}

				// Roughness
				ImGui::Text("Roughness");
				ImGui::SameLine();
				ImGui::SliderFloat("##oRoughness", &mat->roughnessFactor, 0, 1);

				// Metallic
				ImGui::Text("Metallic");
				ImGui::SameLine();
				ImGui::SliderFloat("##oMetallic", &mat->metallicFactor, 0, 1);
			}

			ImGui::PopID();
		}
	}
	ImGui::End();

	if (ImGui::Begin("Output")) {
		ImGui::PushID("OutputTextureRadioBtn");
		ImGui::RadioButton("Result", &selectedOutput, 0);
		ImGui::RadioButton("Albedo", &selectedOutput, 1);
		ImGui::RadioButton("Normal", &selectedOutput, 2);
		ImGui::RadioButton("Material", &selectedOutput, 3);
		ImGui::RadioButton("Depth", &selectedOutput, 4);
		ImGui::PopID();
	}
	ImGui::End();
}