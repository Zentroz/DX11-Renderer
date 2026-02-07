#pragma once

#include"Renderer/Core/Math.h"

namespace zRender {
	struct Light {
		enum LightType { Directional, Point, Spot } type;

		vec3 position;
		vec3 direction;
		vec3 lightColor;
		float intensity;

		// Directional Light
		// Spot Light
		float outerCone;
		float innerCone;
		// Point Light
		float range;
	};
}