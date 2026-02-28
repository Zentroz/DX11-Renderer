#pragma once

#include<entt/entt.hpp>
#include<Renderer/Render/RenderPassInterface.h>

namespace zRender {
	class PostProcessingPass : IRenderPass {
	public:
		__declspec(align(16))
		struct InitData {
			entt::entity outlineEntityID;
		};

	public:
		PostProcessingPass(const InitData& i);

		RenderPassDesc GetDesc() const override;

		void Execute(const RenderPassContext& ctx) override;
	private:
	};
}