#include"Renderer/Render/RenderGraph.h"
#include<cassert>

namespace zRender {
	void RenderGraph::AddPass(IRenderPass* pass) {
		m_RenderPasses.push_back(pass);
	}
	void BindRenderPassResource(ICommandContext* ctx, const std::vector<RenderPassResource>& resources, bool isOutput) {
		std::vector<Handle> rtvs;
		Handle dsv{};

		for (auto r : resources) {
			if (isOutput) {
				// Binding Types
				if (r.usage == RenderPassResource::RTV) rtvs.push_back(r.handle);
				else if (r.usage == RenderPassResource::DSV) {
					assert(dsv.isNull());
					dsv = r.handle;
				}
			}
			else {
				// Binding Type
				if (r.usage == RenderPassResource::SRV) {
					// Bind Stages
					if (r.stage == RenderPassResource::VS) ctx->SetTextureVS(r.handle, r.slot);
					else if (r.stage == RenderPassResource::PS) ctx->SetTexturePS(r.handle, r.slot);
				}
				else if (r.usage == RenderPassResource::Buffer) {
					// Bind Stages
					if (r.stage == RenderPassResource::VS) ctx->SetBufferVS(r.handle, r.slot);
					else if (r.stage == RenderPassResource::PS) ctx->SetBufferPS(r.handle, r.slot);
				}
			}
		}

		if (isOutput) ctx->SetRenderTargets(rtvs.size(), rtvs.data(), dsv);
	}
	void RenderGraph::ExecutePass(IRenderPass* pass, const RenderPassContext& ctx) {
		const RenderPassDesc desc = pass->GetDesc();

		ctx.cmdCtx->SetRenderTargets(0, nullptr, uuid());

		BindRenderPassResource(ctx.cmdCtx, desc.outputs, true);
		BindRenderPassResource(ctx.cmdCtx, desc.inputs, false);

		pass->Execute(ctx);
	}
	void RenderGraph::Execute(const RenderPassContext& ctx) {
		for (auto pass : m_RenderPasses) {
			ExecutePass(pass, ctx);
		}
	}
}