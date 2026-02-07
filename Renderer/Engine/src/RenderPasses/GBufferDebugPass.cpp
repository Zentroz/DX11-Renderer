#include"RenderPasses/GBufferDebugPass.h"

namespace zRender {
	GBufferDebugPass::GBufferDebugPass(const GBufferDebug& data) 
		: albedoRT(data.albedoRT), normalRT(data.normalRT), materialRT(data.materialRT), renderTextureRT(data.renderTextureRT), pipeline(data.pipeline)
	{}

	RenderPassDesc GBufferDebugPass::GetDesc() const {
		return {
			"G-BufferDebug",
			{
				{ 0, albedoRT, RenderPassResource::SRV, RenderPassResource::PS },
				{ 1, normalRT, RenderPassResource::SRV, RenderPassResource::PS },
				{ 2, materialRT, RenderPassResource::SRV, RenderPassResource::PS }
			},
			{
				{ 0, renderTextureRT, RenderPassResource::RTV, RenderPassResource::PS },
			},
			false
		};
	}

	void GBufferDebugPass::Execute(const RenderPassContext& ctx) {
		ctx.ctx->BindPipeline(pipeline);
		ctx.ctx->Draw(3);
	}
}