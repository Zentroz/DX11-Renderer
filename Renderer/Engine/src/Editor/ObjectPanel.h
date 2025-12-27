#pragma once
#include"Renderer/Core/Math.h"

class ObjectPanel {
public:
	ObjectPanel() = default;

	zRender::vec4 GetColor() { return { m_ColorInput[0], m_ColorInput[1], m_ColorInput[2], 1 }; }
	float GetRoughness() { return m_RoughnessSlider; };
	float GetMetallic() { return m_MetallicSlider; };

	void Draw();
private:
	float m_ColorInput[3] = { 1, 0, 0 };
	float m_RoughnessSlider;
	float m_MetallicSlider;
};