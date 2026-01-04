#pragma once

#include"Renderer/Render/RenderPassInterface.h"

namespace zRender {
	struct GBuffer {
		Handle albedoRT;
		Handle normalRT;
		Handle materialRT;
		Handle depthRT;

		BufferHandle objectBufferHandle;
		BufferHandle materialBufferHandle;

		PipelineStateContainer pipelineStateHandles;
	};

	class GBufferPass : public IRenderPass {
	public:
		GBufferPass(const GBuffer& gBuffer);
		RenderPassDesc GetDesc() const override;
		void Execute(const RenderPassContext& ctx) override;
	private:
		Handle albedoRT;
		Handle normalRT;
		Handle materialRT;
		Handle depthRT;

		BufferHandle objectBufferHandle;
		BufferHandle materialBufferHandle;

		PipelineStateContainer pipelineStateHandles;
	};
}