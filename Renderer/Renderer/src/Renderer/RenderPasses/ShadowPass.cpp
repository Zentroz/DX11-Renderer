#include"ShadowPass.h"

#include"Renderer/Render/RenderConstantBuffers.h"

using namespace zRender;

ShadowPass::ShadowPass(const InitData& i) 
	: depthSV(i.depthSV), pipeline(i.pipeline), objectBufferHandle(i.objectBufferHandle), frameBufferHandle(i.frameBufferHandle)
{}

RenderPassDesc ShadowPass::GetDesc() const {
	return {
		"ShadowPass",
		{},
		{ { 0, depthSV, RenderPassResource::DSV, RenderPassResource::PS } },
		true
	};
}
void ShadowPass::Execute(const RenderPassContext& ctx) {
	/*
	FrameData fData;
	ObjectData oData;

	Camera cam = ctx.renderCamera;
	cam.forward = vec3(0.25, 0.5f, -0.25f) * -1;
	cam.position = vec3(0.25, 0.5f, -0.25f) * 10;
	cam.renderMode = Camera::Orthographic;

	fData.vpMatrix = cam.ViewProjMatrix();
	ctx.ctx->UpdateBuffer(frameBufferHandle, sizeof(FrameData), &fData);
	ctx.ctx->BindBufferVS(1, frameBufferHandle);

	ctx.ctx->ClearDepthStencil(depthSV);
	ctx.ctx->BindPipeline(pipeline);

	for (auto& item : ctx.renderItems) {
		oData.modelMatrix = item.modelMatrix;

		ctx.ctx->UpdateBuffer(objectBufferHandle, sizeof(ObjectData), &oData);
		ctx.ctx->BindBufferVS(2, objectBufferHandle);

		ctx.ctx->DrawGeometryIndexed(item.meshHandle);
	}
	*/
}