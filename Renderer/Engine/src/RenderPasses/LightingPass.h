#pragma once

#include"Renderer/Render/RenderPassInterface.h"

namespace zRender {
	class LightingPass : public IRenderPass {
	public:
		struct MatricesBufferData {
			DirectX::XMMATRIX invViewProj;
		};

		struct InitData {
			Handle albedoRT;
			Handle normalRT;
			Handle materialRT;
			Handle depthRT;
			Handle shadowDSV;
			Handle outputRT;

			BufferHandle lightBufferHandle;
			BufferHandle matricesBufferHandle;

			PipelineStateContainer pipeline;
		};

		struct LightPassShaderData {
			Light lights[8];
			int4 lightCount;
			vec4 shadowBias;
			int4 pcFilterSampleCount;
		};

	public:
		LightingPass(const InitData& i);

		RenderPassDesc GetDesc() const override;

		void Execute(const RenderPassContext& ctx) override;

	private:
		Handle albedoRT;
		Handle normalRT;
		Handle materialRT;
		Handle depthRT;
		Handle shadowDSV;
		Handle outputRT;

		BufferHandle lightBufferHandle;
		BufferHandle matricesBufferHandle;

		PipelineStateContainer pipeline;
	};
}