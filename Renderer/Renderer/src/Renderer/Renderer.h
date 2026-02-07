#pragma once

#include<vector>

#include"Renderer/Core/Math.h"
#include"Renderer/Render/Camera.h"
#include"Renderer/Render/RenderItem.h"
#include"Renderer/Render/RenderContextInterface.h"
#include"Renderer/Render/ResourceProviderInterface.h"
#include"Renderer/Render/RenderConstantBuffers.h"
#include"Renderer/Render/RenderGraph.h"

namespace zRender {
	class Renderer {
	public:
		Renderer() = default;

		void Shutdown();

		void Queue(RenderItem item);

		void Setup(IRenderContext* renderContext, IRenderResourceProvider* resourceProvider);
		void SetCamera(Camera& cam);
		void AddLight(Light light);

		void AddRenderPass(IRenderPass* pass);

		void InitRender();
		void Render();
		void EndRender();

		//void DestroyRenderPasses();

		IRenderPass* GetRenderPass(std::string passName);

	private:
		RenderGraph renderGraph;
		std::vector<RenderItem> m_RenderQueueOpaque;
		std::vector<RenderItem> m_RenderQueueAplhaTest;
		std::vector<RenderItem> m_RenderQueueTransparent;
		IRenderResourceProvider* resourceProvider;
		IRenderContext* m_RenderContext;

		Camera renderCamera;
		Light m_Lights[8];
		int lightCount;
	};
}