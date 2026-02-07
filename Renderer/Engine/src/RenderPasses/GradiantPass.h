#pragma once

#pragma once

#include"Renderer/Render/RenderPassInterface.h"

namespace zRender {
	struct GradiantPassInput {
		Handle outputTextureHandle;
		PipelineStateContainer pipeline;
	};

	class GradiantPass : public IRenderPass {
	public:
		GradiantPass(const GradiantPassInput& input);

		RenderPassDesc GetDesc() const override;

		void Execute(const RenderPassContext& ctx) override;

	private:
		Handle outputTextureHandle;
		PipelineStateContainer pipeline;
	};
}