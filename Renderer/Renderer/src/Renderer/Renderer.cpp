#include"Renderer.h"

#include"Render/RenderPassInterface.h"

namespace zRender {

	void Renderer::Setup(IRenderContext* renderContext, IRenderResourceProvider* resourceProvider) {
		this->m_RenderContext = renderContext;
		this->resourceProvider = resourceProvider;
	}

	void Renderer::SetCamera(Camera& cam) {
		renderCamera = cam;
	}
	void Renderer::SetScreenSize(int width, int height) {
		this->width = width;
		this->height = height;
	}

	void Renderer::Shutdown() {}

	void Renderer::InitRender() {
		m_RenderContext->BeginFrame();
	}

	void Renderer::Render() {
		RenderPassContext ctx{
			.ctx = m_RenderContext,
			.renderCamera = &renderCamera,
			.lights = m_Lights,
			.lightCount = lightCount,
			.renderItemsOpaque = &m_RenderQueueOpaque,
			.renderItemsAplhaTest = &m_RenderQueueAplhaTest,
			.renderItemsTransparent = &m_RenderQueueTransparent
		};

		renderGraph.Execute(ctx);
	}
	void Renderer::EndRender() {
		m_RenderQueueOpaque.clear();
		m_RenderQueueAplhaTest.clear();
		m_RenderQueueTransparent.clear();
	}

	void Renderer::Queue(RenderItem item) {
		switch (item.materialData.surfaceType) {
		case Opaque:
			m_RenderQueueOpaque.push_back(item);
			break;
		case Transparent:
			m_RenderQueueTransparent.push_back(item);
			break;
		}
	}

	void Renderer::AddLight(Light light) {
		m_Lights[lightCount++] = light;
	}

	void Renderer::AddRenderPass(IRenderPass* pass) {
		renderGraph.AddPass(pass);
	}

	IRenderPass* Renderer::GetRenderPass(std::string passName) {
		for (auto pass : renderGraph.GetRenderPasses()) {
			if (pass->GetDesc().name == passName) return pass;
		}

		return nullptr;
	}
}