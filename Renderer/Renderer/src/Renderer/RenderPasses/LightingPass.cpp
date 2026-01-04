#include"Renderer/RenderPasses/LightingPass.h"

namespace zRender {
	LightingPass::LightingPass(const InitData& i)
		: albedoRT(i.albedoRT), normalRT(i.normalRT), materialRT(i.materialRT), depthRT(i.depthRT), outputRT(i.outputRT), 
		shadowDSV(i.shadowDSV), matricesBufferHandle(i.matricesBufferHandle), pipeline(i.pipeline)
	{}

	RenderPassDesc LightingPass::GetDesc() const {
		return {
			"LightingPass",
			{
				{ 0, albedoRT, RenderPassResource::SRV, RenderPassResource::PS },
				{ 1, normalRT, RenderPassResource::SRV, RenderPassResource::PS },
				{ 2, materialRT, RenderPassResource::SRV, RenderPassResource::PS },
				{ 3, depthRT, RenderPassResource::SRV, RenderPassResource::PS },
				//{ 4, shadowDSV, RenderPassResource::SRV, RenderPassResource::PS },
			},
			{ { 0, outputRT, RenderPassResource::RTV, RenderPassResource::PS } },
			false
		};
	}

	void LightingPass::Execute(const RenderPassContext& ctx) {
		float clearColor[4] = { 1, 0, 0, 1 };
		ctx.ctx->ClearRenderTarget(outputRT, clearColor);

		MatricesBufferData mData;
		mData.invViewProj = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, ctx.renderCamera.ViewMatrix() * ctx.renderCamera.ProjMatrix()));

		ctx.ctx->UpdateBuffer(matricesBufferHandle, sizeof(MatricesBufferData), &mData);
		ctx.ctx->BindBufferPS(2, matricesBufferHandle);

		ctx.ctx->BindPipeline(pipeline);
		ctx.ctx->Draw(3);
	}
}