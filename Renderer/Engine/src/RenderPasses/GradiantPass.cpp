#include"RenderPasses/GradiantPass.h"

namespace zRender {

	GradiantPass::GradiantPass(const GradiantPassInput& input)
		: outputTextureHandle(input.outputTextureHandle),
		pipelineHandle(input.pipelineHandle)
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
		ctx.cmdCtx->SetViewport(ctx.renderCamera->width, ctx.renderCamera->height);
		ctx.cmdCtx->SetPipeline(pipelineHandle);
		ctx.cmdCtx->Draw(3, 0);
	}
}