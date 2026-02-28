#pragma once
#include<entt/entt.hpp>
#include<Renderer/Core/Math.h>
#include"Engine/AssetManager.h"

class ObjectPanel {
public:
	ObjectPanel() = default;

	void Draw(entt::entity entity, entt::registry& registry);

	int GetSelectedRenderOutput() { return selectedOutput; };
private:
	int selectedOutput = 0;
};