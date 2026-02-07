#include"RenderPasses/GradiantPass.h"

namespace zRender {

	GradiantPass::GradiantPass(const GradiantPassInput& input)
		: outputTextureHandle(input.outputTextureHandle),
		pipeline(input.pipeline)
	{
	}

	RenderPassDesc GradiantPass::GetDesc() const {
		return {
			"GradiantPass",
			{},
			{ { 0, outputTextureHandle, RenderPassResource::RTV, RenderPassResource::PS } },
			false
		};
	}

	void GradiantPass::Execute(const RenderPassContext& ctx) {
		ctx.ctx->BindPipeline(pipeline);
		ctx.ctx->Draw(3);
	}
}