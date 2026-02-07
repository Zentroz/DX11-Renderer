#pragma once

#include<Renderer/Render/RenderPassInterface.h>

namespace zRender {
	struct TransparentPassInput {
		Handle outputRT;

		Handle whiteTextureHandle;

		BufferHandle staticBufferHandle;
		BufferHandle frameBufferHandle;
		BufferHandle objectBufferHandle;
		BufferHandle materialBufferHandle;

		PipelineStateContainer pipelineStateHandles;
	};

	class TransparentPass : IRenderPass {
	public:
		TransparentPass(const TransparentPassInput& input);
		RenderPassDesc GetDesc() const override;
		void Execute(const RenderPassContext& ctx) override;
	private:
		Handle outputRT;

		Handle whiteTextureHandle;

		BufferHandle staticBufferHandle;
		BufferHandle frameBufferHandle;
		BufferHandle objectBufferHandle;
		BufferHandle materialBufferHandle;

		PipelineStateContainer pipelineStateHandles;
	};
}