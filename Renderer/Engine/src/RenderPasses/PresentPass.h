#pragma once

#include"Renderer/Render/RenderPassInterface.h"

namespace zRender {
	struct PresentPassInput {
		Handle presentTextureHandle;
		Handle albedoTextureHandle;
		Handle normalTextureHandle;
		Handle materialTextureHandle;
		Handle depthTextureHandle;
		Handle backgroundTextureHandle;
		Handle screenTextureHandle;

		BufferHandle outputTextureBufferHandle;

		PipelineStateContainer pipeline;
	};

	class PresentPass : public IRenderPass {
	public:
		__declspec(align(16)) struct OuputConstantBuffer {
			int outputTextureIndex;
			int paddings[3];
		};
	public:
		PresentPass(const PresentPassInput& input);

		RenderPassDesc GetDesc() const override;

		void Execute(const RenderPassContext& ctx) override;

		void SetOutputTextureIndex(int index) { outputTextureIndex = index; }
	private:
		int outputTextureIndex;

		Handle presentTextureHandle;
		Handle albedoTextureHandle;
		Handle normalTextureHandle;
		Handle materialTextureHandle;
		Handle depthTextureHandle;
		Handle backgroundTextureHandle;
		Handle screenTextureHandle;

		BufferHandle outputTextureBufferHandle;

		PipelineStateContainer pipeline;
	};
}