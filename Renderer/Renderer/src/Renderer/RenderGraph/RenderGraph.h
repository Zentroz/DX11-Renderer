#pragma once

#include"Renderer/Render/RenderPassInterface.h"

namespace zRender {
	class RenderGraph {
	public:
		RenderGraph() = default;

		void AddPass(IRenderPass* pass);
		void ExecutePass(IRenderPass* pass, const RenderPassContext& ctx);
		void Execute(const RenderPassContext& ctx);
	private:
		std::vector<IRenderPass*> m_RenderPasses;
	};
}