#pragma once

#include<string>
#include<vector>

#include"Renderer/Core/Handles.h"
#include"Renderer/Render/Camera.h"
#include"Renderer/Render/RenderItem.h"
#include"Renderer/Render/RenderContextInterface.h"
#include"Renderer/Render/RenderConstantBuffers.h"

namespace zRender {
	struct RenderPassResource {
		uint32_t slot;
		Handle handle;
		enum Usage { SRV, RTV, DSV, Buffer } usage;
		enum State { VS, PS,CS } stage;
	};

	struct RenderPassContext {
		IRenderContext* ctx;
		Camera& renderCamera;

		Light* lights;
		int lightCount;

		const std::vector<RenderItem>& renderItemsOpaque;
		const std::vector<RenderItem>& renderItemsAplhaTest;
		const std::vector<RenderItem>& renderItemsTransparent;

		RenderPassContext(
			IRenderContext* ctx, Camera& renderCamera,
			const std::vector<RenderItem>& renderItems,
			const std::vector<RenderItem>& renderItemsAplhaTest,
			const std::vector<RenderItem>& renderItemsTransparent,
			Light* lights,
			int lightCount
		)
			: ctx(ctx), renderCamera(renderCamera), renderItemsOpaque(renderItems), renderItemsTransparent(renderItemsTransparent), 
			renderItemsAplhaTest(renderItemsAplhaTest), lights(lights), lightCount(lightCount)
		{}
	};

	struct RenderPassDesc {
		std::string name;

		std::vector<RenderPassResource> inputs;
		std::vector<RenderPassResource> outputs;

		bool usesDepth;
	};

	class IRenderPass {
	public:
		virtual RenderPassDesc GetDesc() const = 0;
		virtual void Execute(const RenderPassContext& ctx) = 0;
		virtual void Destroy() {};
	};
}