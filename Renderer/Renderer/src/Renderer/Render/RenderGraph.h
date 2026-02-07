#pragma once

#include"Renderer/Render/RenderPassInterface.h"

namespace zRender {
	class RenderGraph {
	public:
		RenderGraph() = default;

		void AddPass(IRenderPass* pass);
		void ExecutePass(IRenderPass* pass, const RenderPassContext& ctx);
		void Execute(const RenderPassContext& ctx);

		const std::vector<IRenderPass*>& GetRenderPasses() const { return m_RenderPasses; }
	private:
		std::vector<IRenderPass*> m_RenderPasses;
	};
}