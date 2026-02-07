#pragma once

#include"Renderer/Render/RenderPassInterface.h"

namespace zRender {
	struct GBufferDebug {
		Handle albedoRT;
		Handle normalRT;
		Handle materialRT;
		Handle renderTextureRT;
		PipelineStateContainer pipeline;
	};

	class GBufferDebugPass : public IRenderPass {
	public:
		GBufferDebugPass(const GBufferDebug& gBuffer);
		RenderPassDesc GetDesc() const override;
		void Execute(const RenderPassContext& ctx) override;
	private:
		Handle albedoRT;
		Handle normalRT;
		Handle materialRT;
		Handle renderTextureRT;

		PipelineStateContainer pipeline;
	};
}