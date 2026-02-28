#pragma once

#include"Renderer/Core/Math.h"

namespace zRender {
	__declspec(align(16)) struct Light {
		int4 type; // x = { 0 = Directional / 1 = Point / 2 = Spot }, y = castShadow

		vec4 position;
		vec4 direction;
		vec4 lightColor;
		/// <summary>
		/// x = Intensity, y = Range, z = InnerCone, w = OuterCone
		/// </summary>
		vec4 lightProp;

		DirectX::XMMATRIX VPMatrix;
		DirectX::XMMATRIX invVPMatrix;
	};
	__declspec(align(16)) struct LightData {
		Light lights[8];
		int4 lightCount;
	};

	__declspec(align(16)) struct StaticData {
		vec4 mainLightDirection;
		vec4 mainLightColor;
	};

	__declspec(align(16)) struct FrameData {
		DirectX::XMMATRIX vpMatrix;
		vec4 cameraPosition;
		vec4 timeAndScreen; // x = delta, y = total, z = screenWidth, w = screenHeight
	};

	__declspec(align(16)) struct ObjectData {
		DirectX::XMMATRIX modelMatrix;
	};
	__declspec(align(16)) struct MaterialData {
		vec4 diffuseColor;
		float roughness;
		float metallic;
		float aplhaCutoff;
		float padding;
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