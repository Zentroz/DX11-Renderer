#include"Renderer/RenderPasses/PresentPass.h"

namespace zRender {

	PresentPass::PresentPass(Handle presentTextureHandle, Handle screenTextureHandle, PipelineStateContainer pipeline) 
		: presentTextureHandle(presentTextureHandle), screenTextureHandle(screenTextureHandle), pipeline(pipeline)
	{}

	RenderPassDesc PresentPass::GetDesc() const {
		return {
			"PresentPass",
			{ { 0, presentTextureHandle, RenderPassResource::SRV, RenderPassResource::PS } },
			{ { 0, screenTextureHandle, RenderPassResource::RTV, RenderPassResource::PS } },
			false
		};
	}

	void PresentPass::Execute(const RenderPassContext& ctx) {
		ctx.ctx->BindPipeline(pipeline);
		ctx.ctx->Draw(3);
	}
}