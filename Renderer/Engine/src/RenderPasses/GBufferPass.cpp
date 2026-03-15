#include"RenderPasses/GBufferPass.h"
#include<Renderer/Render/RenderConstantBuffers.h>

#include<DirectXMath.h>
#include<imgui/imgui.h>

namespace zRender {

	std::vector<RenderItem> renderItems;

	GBufferPass::GBufferPass(const GBuffer& gBuffer) :
		albedoRT(gBuffer.albedoRT), 
		normalRT(gBuffer.normalRT), 
		materialRT(gBuffer.materialRT), 
		depthRT(gBuffer.depthRT),
		whiteTextureHandle(gBuffer.whiteTextureHandle),
		staticBufferHandle(gBuffer.staticBufferHandle),
		frameBufferHandle(gBuffer.frameBufferHandle),
		objectBufferHandle(gBuffer.objectBufferHandle), 
		materialBufferHandle(gBuffer.materialBufferHandle),
		pipelineStateHandle(gBuffer.pipelineStateHandle)
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
		float clearColor[4] = { 0, 0, 0.25f, 1 };
		ctx.cmdCtx->ClearRTV(albedoRT, clearColor);
		ctx.cmdCtx->ClearRTV(normalRT, clearColor);
		ctx.cmdCtx->ClearRTV(materialRT, clearColor);
		ctx.cmdCtx->ClearDSV(depthRT);

		ctx.cmdCtx->SetViewport(ctx.renderCamera->width, ctx.renderCamera->height);

		ctx.cmdCtx->SetPipeline(pipelineStateHandle);

		auto& io = ImGui::GetIO();

		vec3 pos = ctx.renderCamera->position;
		FrameData fData;
		fData.vpMatrix = DirectX::XMMatrixTranspose(ctx.renderCamera->ViewProjMatrix());
		fData.cameraPosition = { pos.x, pos.y, pos.z, 1 };
		fData.timeAndScreen = {
			.x = io.DeltaTime, 
			.y = totalTime += io.DeltaTime, 
			.z = (float)ctx.renderCamera->width, 
			.w = (float)ctx.renderCamera->height
		};
		ctx.cmdCtx->UpdateBuffer(frameBufferHandle, sizeof(FrameData), &fData);

		ctx.cmdCtx->SetBufferVS(staticBufferHandle, 0);
		ctx.cmdCtx->SetBufferPS(staticBufferHandle, 0);
		ctx.cmdCtx->SetBufferVS(frameBufferHandle, 1);
		ctx.cmdCtx->SetBufferPS(frameBufferHandle, 1);

		Render(ctx.cmdCtx, ctx.renderItemsOpaque);
		Render(ctx.cmdCtx, ctx.renderItemsAplhaTest);
	}

	void GBufferPass::Render(ICommandContext* ctx, const std::vector<RenderItem>* items) {
		for (auto& item : *items) {
			MaterialData mData;
			mData.diffuseColor = item.materialData.baseColor;
			mData.roughness = item.materialData.roughness;
			mData.metallic = item.materialData.metallic;
			mData.aplhaCutoff = item.materialData.aplhaCutoff;

			ObjectData oData;
			oData.modelMatrix = DirectX::XMMatrixTranspose(item.modelMatrix);

			ctx->UpdateBuffer(objectBufferHandle, sizeof(ObjectData), &oData);
			ctx->UpdateBuffer(materialBufferHandle, sizeof(MaterialData), &mData);

			ctx->SetBufferVS(objectBufferHandle, 2);
			ctx->SetBufferPS(materialBufferHandle, 2);

			ctx->SetVertexBuffer(item.meshHandle);
			ctx->SetIndexBuffer(item.meshHandle);

			for (uint8_t i = 0; i < 16; i++) {
				if (i >= item.materialData.textureHandles.size()) {
					ctx->SetTexturePS(whiteTextureHandle, i);
					continue;
				}

				ctx->SetTexturePS(item.materialData.textureHandles[i].isNull() ? whiteTextureHandle : item.materialData.textureHandles[i], i);
			}

			ctx->DrawIndexed(item.indexCount, item.baseIndexLocation, item.baseVertexLocation);
		}
	}
}