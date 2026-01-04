#pragma once

#include"Renderer/Core/Math.h"

namespace zRender {
	__declspec(align(16)) struct Light {
		vec3 color;
		float intensity;
		vec3 vector;
		float range;
	};

	__declspec(align(16)) struct StaticData {
		vec4 mainLightDirection;
		vec4 mainLightColor;
	};

	__declspec(align(16)) struct FrameData {
		DirectX::XMMATRIX vpMatrix;
		vec4 cameraPosition;
	};

	__declspec(align(16)) struct ObjectData {
		DirectX::XMMATRIX modelMatrix;
	};
	__declspec(align(16)) struct MaterialData {
		vec4 diffuseColor;
		float roughness;
		float metallic;
		vec2 padding;
	};
	__declspec(align(16)) struct AdditionalLightData {
		int lightCount;
		vec3 padding;
		Light lights[32];
	};
	__declspec(align(16)) struct SkyboxData {
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX proj;
	};
}