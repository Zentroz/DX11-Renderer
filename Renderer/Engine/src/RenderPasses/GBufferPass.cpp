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

		auto& io = ImGui::GetIO();

		vec3 pos = ctx.renderCamera.position;
		FrameData fData;
		fData.vpMatrix = DirectX::XMMatrixTranspose(ctx.renderCamera.ViewProjMatrix());
		fData.cameraPosition = { pos.x, pos.y, pos.z, 1 };
		fData.time = { io.DeltaTime, totalTime += io.DeltaTime, 0, 0 };
		ctx.ctx->UpdateBuffer(frameBufferHandle, 0, &fData);
		

		ctx.ctx->BindBufferVS(0, staticBufferHandle);
		ctx.ctx->BindBufferPS(0, staticBufferHandle);
		ctx.ctx->BindBufferVS(1, frameBufferHandle);
		ctx.ctx->BindBufferPS(1, frameBufferHandle);

		Render(ctx.ctx, ctx.renderItemsOpaque);
		Render(ctx.ctx, ctx.renderItemsAplhaTest);
	}

	void GBufferPass::Render(IRenderContext* ctx, const std::vector<RenderItem>& items) {
		for (auto& item : items) {
			MaterialData mData;
			mData.diffuseColor = item.materialData.baseColor;
			mData.roughness = item.materialData.roughness;
			mData.metallic = item.materialData.metallic;
			mData.aplhaCutoff = item.materialData.aplhaCutoff;

			ObjectData oData;
			oData.modelMatrix = DirectX::XMMatrixTranspose(item.modelMatrix);

			ctx->UpdateBuffer(objectBufferHandle, sizeof(ObjectData), &oData);
			ctx->UpdateBuffer(materialBufferHandle, sizeof(MaterialData), &mData);

			ctx->BindBufferVS(2, objectBufferHandle);
			ctx->BindBufferPS(2, materialBufferHandle);

			for (uint8_t i = 0; i < 16; i++) {
				if (i >= item.materialData.textureHandles.size()) {
					if (i < 3)
						ctx->BindBufferPS(i, whiteTextureHandle);
					else
						ctx->BindBufferPS(i, {});

					continue;
				}

				ctx->BindTexturePS(i, item.materialData.textureHandles[i].isNull() ? whiteTextureHandle : item.materialData.textureHandles[i]);
			}

			ctx->DrawGeometryIndexed(item.meshHandle, item.subMeshIndex);
		}
	}
}