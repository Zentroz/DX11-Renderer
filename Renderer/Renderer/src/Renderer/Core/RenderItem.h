#pragma once

#include<DirectXMath.h>
#include"ResourceHandles.h"
#include"Math.h"

/// <summary>
/// Flags to enable rendering settings.
/// </summary>
enum RenderFlag { None = 0, RenderFlag_CastShadows = 1 << 0, RenderFlag_ReceiveShadows = 1 << 1 };

namespace zRender {

	struct Material {
		vec4 diffuseColor;
		vec4 ambientColor;
		vec4 specularColor;
		float shininess;
		float roughness;
		float metallic;
		Handle diffuseTexHandle = InvalidHandle;
		Handle normalTexHandle = InvalidHandle;
	};

	/// <summary>
	/// Item to be rendered.
	/// </summary>
	struct RenderItem {
		Handle textureHandle;
		Handle meshHandle;
		Handle shaderHandle;
		Material material;
		DirectX::XMMATRIX modelMatrix;
		int flags;
	};
}