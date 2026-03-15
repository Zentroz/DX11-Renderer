#include"ShadowPass.h"

#include"Renderer/Render/RenderConstantBuffers.h"

using namespace zRender;

ShadowPass::ShadowPass(const InitData& i) 
	: depthSV(i.depthSV), pipelineHandle(i.pipelineHandle), objectBufferHandle(i.objectBufferHandle), lightBufferHandle(i.lightBufferHandle), debugRT(i.debugRT)
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

	ctx.cmdCtx->SetViewport(4096, 4096);
	ctx.cmdCtx->ClearRTV(debugRT, debugColor);
	ctx.cmdCtx->ClearDSV(depthSV);

	ObjectData oData{};

	Light& light = ctx.lights[0];

	LightBuffer lightData{};
	lightData.light[0] = light;

	lightData.light[0].VPMatrix = DirectX::XMMatrixTranspose(light.VPMatrix);

	ctx.cmdCtx->UpdateBuffer(lightBufferHandle, sizeof(LightBuffer), &lightData);
	ctx.cmdCtx->SetBufferVS(lightBufferHandle, 3);

	ctx.cmdCtx->SetPipeline(pipelineHandle);

	for (auto& item : *ctx.renderItemsOpaque) {
		oData.modelMatrix = DirectX::XMMatrixTranspose(item.modelMatrix);
	
		ctx.cmdCtx->UpdateBuffer(objectBufferHandle, sizeof(ObjectData), &oData);
		ctx.cmdCtx->SetBufferVS(objectBufferHandle, 2);
	
		ctx.cmdCtx->DrawIndexed(item.indexCount, item.baseIndexLocation, item.baseVertexLocation);
	}
}