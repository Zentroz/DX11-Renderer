#pragma once

#include"Renderer/Render/RenderPassInterface.h"

namespace zRender {
	struct GBuffer {
		Handle albedoRT;
		Handle normalRT;
		Handle materialRT;
		Handle depthRT;

		Handle whiteTextureHandle;

		BufferHandle staticBufferHandle;
		BufferHandle frameBufferHandle;
		BufferHandle objectBufferHandle;
		BufferHandle materialBufferHandle;

		PipelineHandle pipelineStateHandle;
	};

	class GBufferPass : public IRenderPass {
	public:
		GBufferPass(const GBuffer& gBuffer);
		RenderPassDesc GetDesc() const override;
		void Execute(const RenderPassContext& ctx) override;
	private:
		Handle albedoRT;
		Handle normalRT;
		Handle materialRT;
		Handle depthRT;

		Handle whiteTextureHandle;

		BufferHandle staticBufferHandle;
		BufferHandle frameBufferHandle;
		BufferHandle objectBufferHandle;
		BufferHandle materialBufferHandle;

		PipelineHandle pipelineStateHandle;

		float totalTime = 0.0f;

	private:
		void Render(ICommandContext* ctx, const std::vector<RenderItem>* items);
	};
}