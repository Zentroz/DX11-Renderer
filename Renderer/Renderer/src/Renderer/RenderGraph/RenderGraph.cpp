#include"Renderer/RenderGraph/RenderGraph.h"
#include<cassert>

namespace zRender {
	void RenderGraph::AddPass(IRenderPass* pass) {
		m_RenderPasses.push_back(pass);
	}
	void BindRenderPassResource(IRenderContext* ctx, const std::vector<RenderPassResource>& resources, bool isOutput) {
		std::vector<Handle> rtvs;
		Handle dsv = InvalidHandle;

		for (auto r : resources) {
			if (isOutput) {
				// Binding Types
				if (r.usage == RenderPassResource::RTV) rtvs.push_back(r.handle);
				else if (r.usage == RenderPassResource::DSV) {
					assert(dsv == InvalidHandle);
					dsv = r.handle;
				}
			}
			else {
				// Binding Type
				if (r.usage == RenderPassResource::SRV) {
					// Bind Stages
					if (r.stage == RenderPassResource::VS) ctx->BindTextureVS(r.slot, r.handle);
					else if (r.stage == RenderPassResource::PS) ctx->BindTexturePS(r.slot, r.handle);
				}
				else if (r.usage == RenderPassResource::Buffer) {
					// Bind Stages
					if (r.stage == RenderPassResource::VS) ctx->BindBufferVS(r.slot, r.handle);
					else if (r.stage == RenderPassResource::PS) ctx->BindBufferPS(r.slot, r.handle);
				}
			}
		}

		if (isOutput) ctx->BindMultiViews(rtvs.size(), rtvs.data(), dsv);
	}
	void RenderGraph::ExecutePass(IRenderPass* pass, const RenderPassContext& ctx) {
		const RenderPassDesc desc = pass->GetDesc();

		ctx.ctx->ClearStatesAndResources();

		BindRenderPassResource(ctx.ctx, desc.outputs, true);
		BindRenderPassResource(ctx.ctx, desc.inputs, false);

		pass->Execute(ctx);
	}
	void RenderGraph::Execute(const RenderPassContext& ctx) {
		for (auto pass : m_RenderPasses) {
			ExecutePass(pass, ctx);
		}
	}
}