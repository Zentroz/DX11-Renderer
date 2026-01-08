#pragma once

#include<vector>
#include<DirectXMath.h>

#include"Renderer/Core/Handles.h"
#include"Renderer/Core/Math.h"

/// <summary>
/// Flags to enable rendering settings.
/// </summary>
enum RenderFlag { None = 0, RenderFlag_CastShadows = 1 << 0, RenderFlag_ReceiveShadows = 1 << 1 };

namespace zRender {

	/// <summary>
	/// Item to be rendered.
	/// </summary>
	struct RenderItem {
		int flags;
		uint32_t subMeshIndex;

		MeshHandle meshHandle;

		// Material
		struct Material {
			vec4 baseColor;
			float roughness;
			float metallic;

			ShaderHandle shaderHandle;
			std::vector<TextureHandle> textureHandles;
		} materialData;

		// Starts from slot 3
		std::vector<BufferHandle> constantBuffers;
		std::vector<void*> constantBuffersData;

		DirectX::XMMATRIX modelMatrix;
	};
}