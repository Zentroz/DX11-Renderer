#include"TransparentPass.h"
#include<Renderer/Render/RenderConstantBuffers.h>

namespace zRender {
	TransparentPass::TransparentPass(const TransparentPassInput& i)
		: outputRT(i.outputRT), whiteTextureHandle(i.whiteTextureHandle), staticBufferHandle(i.staticBufferHandle), frameBufferHandle(i.frameBufferHandle), 
		objectBufferHandle(i.objectBufferHandle), materialBufferHandle(i.materialBufferHandle), pipelineStateHandles(i.pipelineStateHandles)
	{}

	RenderPassDesc TransparentPass::GetDesc() const {
		return {
			"TransparentPass",
			{},
			{
				{ 0, outputRT, RenderPassResource::RTV, RenderPassResource::VS },
			},
			true
		};
	}

	void TransparentPass::Execute(const RenderPassContext& ctx) {
		ctx.ctx->BindPipeline(pipelineStateHandles);

		vec3 pos = ctx.renderCamera.position;
		FrameData fData;
		fData.vpMatrix = DirectX::XMMatrixTranspose(ctx.renderCamera.ViewProjMatrix());
		fData.cameraPosition = { pos.x, pos.y, pos.z, 1 };
		ctx.ctx->UpdateBuffer(frameBufferHandle, 0, &fData);

		ctx.ctx->BindBufferVS(0, staticBufferHandle);
		ctx.ctx->BindBufferPS(0, staticBufferHandle);
		ctx.ctx->BindBufferVS(1, frameBufferHandle);
		ctx.ctx->BindBufferPS(1, frameBufferHandle);

		for (auto& item : ctx.renderItemsTransparent) {
			MaterialData mData;
			mData.diffuseColor = item.materialData.baseColor;
			mData.roughness = item.materialData.roughness;
			mData.metallic = item.materialData.metallic;

			ObjectData oData;
			oData.modelMatrix = DirectX::XMMatrixTranspose(item.modelMatrix);

			ctx.ctx->UpdateBuffer(objectBufferHandle, sizeof(ObjectData), &oData);
			ctx.ctx->UpdateBuffer(materialBufferHandle, sizeof(MaterialData), &mData);

			ctx.ctx->BindBufferVS(2, objectBufferHandle);
			ctx.ctx->BindBufferPS(2, materialBufferHandle);

			for (uint8_t i = 0; i < 16; i++) {
				if (i >= item.materialData.textureHandles.size()) {
					if (i < 3)
						ctx.ctx->BindBufferPS(i, whiteTextureHandle);
					else
						ctx.ctx->BindBufferPS(i, {});

					continue;
				}

				ctx.ctx->BindTexturePS(i, item.materialData.textureHandles[i]);
			}

			ctx.ctx->DrawGeometryIndexed(item.meshHandle, item.subMeshIndex);
		}
	}
}