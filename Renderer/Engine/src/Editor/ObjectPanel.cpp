#include"Editor/ObjectPanel.h"

#include<imgui/imgui.h>

void ObjectPanel::Draw() {
	if (ImGui::Begin("Object")) {
		ImGui::Text("Sphere");

		// Color
		ImGui::Text("Color");
		ImGui::SameLine();
		ImGui::ColorEdit3("##oColor", m_ColorInput);

		// Roughness
		ImGui::Text("Roughness");
		ImGui::SameLine();
		ImGui::SliderFloat("##oRoughness", &m_RoughnessSlider, 0, 1);

		// Metallic
		ImGui::Text("Metallic");
		ImGui::SameLine();
		ImGui::SliderFloat("##oMetallic", &m_MetallicSlider, 0, 1);
	}
	ImGui::End();
}