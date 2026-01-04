#pragma once

#include"Renderer/Render/RenderPassInterface.h"

namespace zRender {
	class PresentPass : public IRenderPass {
	public:
		PresentPass(Handle presentTextureHandle, Handle screenTextureHandle, PipelineStateContainer pipeline);

		RenderPassDesc GetDesc() const override;

		void Execute(const RenderPassContext& ctx) override;
	private:
		Handle presentTextureHandle;
		Handle screenTextureHandle;
		PipelineStateContainer pipeline;
	};
}