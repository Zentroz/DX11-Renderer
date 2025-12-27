#pragma once

#include<DirectXMath.h>
#include"Renderer/Core/Handles.h"
#include"Renderer/Core/Math.h"

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
		TextureHandle diffuseTexHandle = InvalidHandle;
		TextureHandle normalTexHandle = InvalidHandle;
	};

	/// <summary>
	/// Item to be rendered.
	/// </summary>
	struct RenderItem {
		TextureHandle textureHandle;
		MeshHandle meshHandle;
		ShaderHandle shaderHandle;
		Material material;
		DirectX::XMMATRIX modelMatrix;
		int flags;
	};
}