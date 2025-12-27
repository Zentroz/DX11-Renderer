#pragma once

#include<vector>

#include"Core/RenderItem.h"
#include"Core/RenderContextInterface.h"
#include"Core/ResourceProviderInterface.h"
#include"Core/Math.h"
#include"Core/Camera.h"

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
	/*
	struct OpaquePassContext {
		IRenderContext* renderContext;
	};
	struct TransparentPassContext {
		IRenderContext* renderContext;
	};
	struct SkyboxPassContext {
		IRenderContext* renderContext;
	};*/

	class Renderer {
	public:
		Renderer() = default;

		void Shutdown();

		void Queue(RenderItem item);

		void Setup(IRenderContext* renderContext, IRenderResourceProvider* resourceProvider);
		void SetCamera(Camera& cam);
		void SetSkybox(Handle textureHandle);
		void SetPipelineShader(PipelineStateType pipelineStateType, Handle shaderHandle);
		void AddLight(Light light);

		void RenderOpaque();
		void RenderSkybox();

		void InitRender();
		void Render();
		void EndRender();

	private:
		IRenderContext* m_RenderContext;
		IRenderResourceProvider* resourceProvider;
		std::vector<RenderItem> m_RenderQueue;
		PipelineStateContainer m_PipelineStates[8];

		// Constant Buffers
		Handle staticBufferHandle = InvalidHandle;
		Handle frameBufferHandle = InvalidHandle;
		Handle objectBufferHandle = InvalidHandle;
		Handle materialBufferHandle = InvalidHandle;
		Handle skyboxBufferHandle = InvalidHandle;

		Camera renderCamera;
		Light m_Lights[32];
		int lightCount;
		Handle skyboxTextureHandle = InvalidHandle;
		float check = 1.0f;
	private:
		PipelineStateContainer& GetPipelineState(PipelineStateType state);
	};
}