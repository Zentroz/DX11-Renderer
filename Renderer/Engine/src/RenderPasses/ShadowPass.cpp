#include"ShadowPass.h"

#include"Renderer/Render/RenderConstantBuffers.h"

using namespace zRender;

ShadowPass::ShadowPass(const InitData& i) 
	: depthSV(i.depthSV), pipeline(i.pipeline), objectBufferHandle(i.objectBufferHandle), lightBufferHandle(i.lightBufferHandle), debugRT(i.debugRT)
{}

RenderPassDesc ShadowPass::GetDesc() const {
	return {
		"ShadowPass",
		{},
		{ { 0, debugRT, RenderPassResource::RTV, RenderPassResource::PS }, { 0, depthSV, RenderPassResource::DSV, RenderPassResource::PS } },
		true
	};
}
void ShadowPass::Execute(const RenderPassContext& ctx) {
	float debugColor[4] = {0, 0, 0, 1};

	ctx.ctx->SetViewport(4096, 4096);
	ctx.ctx->ClearRenderTarget(debugRT, debugColor);
	ctx.ctx->ClearDepthStencil(depthSV);

	ObjectData oData{};

	Light& light = ctx.lights[0];

	LightBuffer lightData{};
	lightData.light[0] = light;

	lightData.light[0].VPMatrix = DirectX::XMMatrixTranspose(light.VPMatrix);

	ctx.ctx->UpdateBuffer(lightBufferHandle, sizeof(LightBuffer), &lightData);
	ctx.ctx->BindBufferVS(3, lightBufferHandle);

	ctx.ctx->BindPipeline(pipeline);

	for (auto& item : *ctx.renderItemsOpaque) {
		oData.modelMatrix = DirectX::XMMatrixTranspose(item.modelMatrix);
	
		ctx.ctx->UpdateBuffer(objectBufferHandle, sizeof(ObjectData), &oData);
		ctx.ctx->BindBufferVS(2, objectBufferHandle);
	
		ctx.ctx->DrawGeometryIndexed(item.meshHandle, item.subMeshIndex);
	}
}