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

			BufferHandle matricesBufferHandle;

			PipelineStateContainer pipeline;
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

		BufferHandle matricesBufferHandle;

		PipelineStateContainer pipeline;
	};
}