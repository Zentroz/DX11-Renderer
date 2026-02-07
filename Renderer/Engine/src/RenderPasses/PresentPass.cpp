#include"RenderPasses/PresentPass.h"

namespace zRender {

	PresentPass::PresentPass(const PresentPassInput& input) 
		: outputTextureIndex(2),
		presentTextureHandle(input.presentTextureHandle), 
		albedoTextureHandle(input.albedoTextureHandle),
		normalTextureHandle(input.normalTextureHandle),
		materialTextureHandle(input.materialTextureHandle),
		depthTextureHandle(input.depthTextureHandle),
		backgroundTextureHandle(input.backgroundTextureHandle),
		screenTextureHandle(input.screenTextureHandle),
		outputTextureBufferHandle(input.outputTextureBufferHandle),
		pipeline(input.pipeline)
	{}

	RenderPassDesc PresentPass::GetDesc() const {
		return {
			"PresentPass",
			{ 
				{ 0, presentTextureHandle, RenderPassResource::SRV, RenderPassResource::PS },
				{ 1, albedoTextureHandle, RenderPassResource::SRV, RenderPassResource::PS },
				{ 2, normalTextureHandle, RenderPassResource::SRV, RenderPassResource::PS },
				{ 3, materialTextureHandle, RenderPassResource::SRV, RenderPassResource::PS },
				{ 4, depthTextureHandle, RenderPassResource::SRV, RenderPassResource::PS },
				{ 5, backgroundTextureHandle, RenderPassResource::SRV, RenderPassResource::PS }
			},
			{ { 0, screenTextureHandle, RenderPassResource::RTV, RenderPassResource::PS } },
			false
		};
	}

	void PresentPass::Execute(const RenderPassContext& ctx) {
		OuputConstantBuffer outputData{
			.outputTextureIndex = outputTextureIndex,
			.paddings = {0, 0, 0}
		};

		ctx.ctx->UpdateBuffer(outputTextureBufferHandle, sizeof(OuputConstantBuffer), &outputData);

		ctx.ctx->BindBufferPS(3, outputTextureBufferHandle);

		ctx.ctx->BindPipeline(pipeline);
		ctx.ctx->Draw(3);
	}
}