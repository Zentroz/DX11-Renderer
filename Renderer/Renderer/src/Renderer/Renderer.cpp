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

	void Renderer::Shutdown() {}

	void Renderer::InitRender() {
		m_RenderContext->BeginFrame();
	}

	void Renderer::Render() {
		RenderPassContext ctx(m_RenderContext, renderCamera, m_RenderQueueOpaque, m_RenderQueueAplhaTest, m_RenderQueueTransparent, m_Lights, lightCount);
		renderGraph.Execute(ctx);
	}
	void Renderer::EndRender() {
		m_RenderQueueOpaque.clear();
		m_RenderQueueAplhaTest.clear();
		m_RenderQueueTransparent.clear();
	}

	void Renderer::Queue(RenderItem item) {
		switch (item.materialData.renderMode) {
		case RenderItem::Material::Opaque:
			m_RenderQueueOpaque.push_back(item);
			break;
		case RenderItem::Material::AplhaTest:
			m_RenderQueueAplhaTest.push_back(item);
			break;
		case RenderItem::Material::Transparent:
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