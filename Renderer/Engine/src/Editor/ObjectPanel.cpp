#include"Editor/ObjectPanel.h"
#include<imgui/imgui.h>
#include"Engine/Components.h"

void ObjectPanel::Draw(entt::entity entity, entt::registry& reg) {
	if (!reg.valid(entity)) return;

	auto view = reg.view<TransformComponent, MaterialComponent>();
	std::tuple<TransformComponent&, MaterialComponent&> components =  reg.get<TransformComponent, MaterialComponent>(entity);

	TransformComponent& t = std::get<0>(components);
	MaterialComponent& m = std::get<1>(components);

	if (ImGui::Begin("Entity Properties")) {
		std::string name = "Material";
		ImGui::PushID(name.c_str());
		if (ImGui::CollapsingHeader(name.c_str())) {
			//Transform
			ImGui::SeparatorText("Transform");
			float m_PositionInput[3] = { t.position.x, t.position.y, t.position.z };
			ImGui::Text("Position");
			ImGui::SameLine();
			if (ImGui::InputFloat3("##oPosition", m_PositionInput)) {
				t.position = zRender::vec3(m_PositionInput[0], m_PositionInput[1], m_PositionInput[2]);
			}
			float m_ScaleInput[3] = { t.scale.x, t.scale.y, t.scale.z };
			ImGui::Text("Scale");
			ImGui::SameLine();
			if (ImGui::InputFloat3("##oScale", m_ScaleInput)) {
				t.scale = zRender::vec3(m_ScaleInput[0], m_ScaleInput[1], m_ScaleInput[2]);
			}

			// Material
			ImGui::SeparatorText("Material");
			// Color
			float m_ColorInput[3] = { m.baseColor.x, m.baseColor.y, m.baseColor.z };
			ImGui::Text("Color");
			ImGui::SameLine();
			if (ImGui::ColorEdit3("##oColor", m_ColorInput)) {
				m.baseColor = zRender::vec4(m_ColorInput[0], m_ColorInput[1], m_ColorInput[2], 1);
			}

			// Roughness
			ImGui::Text("Roughness");
			ImGui::SameLine();
			ImGui::SliderFloat("##oRoughness", &m.roughness, 0, 1);

			// Metallic
			ImGui::Text("Metallic");
			ImGui::SameLine();
			ImGui::SliderFloat("##oMetallic", &m.metallic, 0, 1);

			// Toughness
			ImGui::Text("Toughness");
			ImGui::SameLine();
			ImGui::SliderFloat("##oToughness", &m.toughness, 0, 1);
		}
		ImGui::PopID();
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