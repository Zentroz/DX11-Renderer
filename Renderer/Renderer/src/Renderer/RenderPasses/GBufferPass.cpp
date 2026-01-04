#include"Renderer/RenderPasses/GBufferPass.h"
#include"Renderer/Render/RenderConstantBuffers.h"

#include<DirectXMath.h>

namespace zRender {

	std::vector<RenderItem> renderItems;

	GBufferPass::GBufferPass(const GBuffer& gBuffer) :
		albedoRT(gBuffer.albedoRT), 
		normalRT(gBuffer.normalRT), 
		materialRT(gBuffer.materialRT), 
		depthRT(gBuffer.depthRT),
		objectBufferHandle(gBuffer.objectBufferHandle), 
		materialBufferHandle(gBuffer.materialBufferHandle),
		pipelineStateHandles(gBuffer.pipelineStateHandles)
	{
	}

	RenderPassDesc GBufferPass::GetDesc() const {
		return {
			"G-Buffer_Pass",
			{},
			{ 
				{ 0, albedoRT, RenderPassResource::RTV, RenderPassResource::VS },
				{ 1, normalRT, RenderPassResource::RTV, RenderPassResource::VS },
				{ 2, materialRT, RenderPassResource::RTV, RenderPassResource::VS },
				{ 0, depthRT, RenderPassResource::DSV, RenderPassResource::VS }
			},
			true
		};
	}

	void GBufferPass::Execute(const RenderPassContext& ctx) {
		float clearColor[4] = { 0, 0, 0, 1 };
		ctx.ctx->ClearRenderTarget(albedoRT, clearColor);
		ctx.ctx->ClearRenderTarget(normalRT, clearColor);
		ctx.ctx->ClearRenderTarget(materialRT, clearColor);
		ctx.ctx->ClearDepthStencil(depthRT);

		ctx.ctx->BindPipeline(pipelineStateHandles);

		for (auto& item : ctx.renderItems) {
			MaterialData mData;
			mData.diffuseColor = item.material.diffuseColor;
			mData.metallic = item.material.metallic;
			mData.roughness = item.material.roughness;

			ObjectData oData;
			oData.modelMatrix = DirectX::XMMatrixTranspose(item.modelMatrix);

			ctx.ctx->UpdateBuffer(objectBufferHandle, sizeof(ObjectData), &oData);
			ctx.ctx->UpdateBuffer(materialBufferHandle, sizeof(MaterialData), &mData);

			ctx.ctx->BindBufferVS(2, objectBufferHandle);
			ctx.ctx->BindBufferPS(2, materialBufferHandle);
			ctx.ctx->BindTexturePS(0, item.material.diffuseTexHandle);
			ctx.ctx->BindTexturePS(1, item.material.normalTexHandle);
			ctx.ctx->DrawGeometryIndexed(item.meshHandle);
		}
	}
}