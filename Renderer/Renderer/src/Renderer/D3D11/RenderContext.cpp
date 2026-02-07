#include"RenderContext.h"

const float bgColor[4] = { 0.1f, 0.1f, 0.25f, 1.0f };

namespace zRender {
	void D3D11RenderContext::Set(ID3D11DeviceContext* context, IDXGISwapChain* swapChain) {
		this->context = context;
		this->swapChain = swapChain;
	}

	D3D11RenderContext::D3D11RenderContext(ID3D11DeviceContext* context, IDXGISwapChain* swapChain, D3D11ResourceProvider* resourceProvider) :
		context(context), swapChain(swapChain), resourceProvider(resourceProvider) {

		//depthStencil = resourceProvider->CreateDepthStencilView();
	}

	void D3D11RenderContext::SetScreenSize(int width, int height) {
		this->width = width;
		this->height = height;
	}

	void D3D11RenderContext::ClearRenderTarget(TextureHandle handle, float clearColor[4]) {
		D3D11Texture* tex = resourceProvider->GetResource<D3D11Texture>(handle);
		context->ClearRenderTargetView(tex->renderTargetView, clearColor);
	}

	void D3D11RenderContext::ClearDepthStencil(TextureHandle handle) {
		D3D11Texture* tex = resourceProvider->GetResource<D3D11Texture>(handle);
		if (tex->depthStencilView)
			context->ClearDepthStencilView(tex->depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	void D3D11RenderContext::ClearStatesAndResources() {
		context->OMSetRenderTargets(0, nullptr, nullptr);

		ID3D11ShaderResourceView* nullSrvs[16] = {};
		context->PSSetShaderResources(0, 16, nullSrvs);
	}

	void D3D11RenderContext::BindMultiViews(size_t renderViewCount, Handle* renderViews, Handle depthView) {
		std::vector<ID3D11RenderTargetView*> views;
		views.reserve(renderViewCount);

		for (size_t i = 0; i < renderViewCount; i++) {
			if (renderViews[i].isNull()) {
				views.clear();
				continue;
			}
			D3D11Texture* texture = resourceProvider->GetResource<D3D11Texture>(renderViews[i]);
			ID3D11RenderTargetView* rtv = texture->renderTargetView;
			views.push_back(rtv);
		}


		ID3D11DepthStencilView* dsv = nullptr;

		if (!depthView.isNull()) {
			auto tex = resourceProvider->GetResource<D3D11Texture>(depthView);
			dsv = tex->depthStencilView;
		}

		context->OMSetRenderTargets(views.size(), views.data(), dsv);
	}

	void D3D11RenderContext::UpdateBuffer(BufferHandle h, UINT byteWidth, void* data) {
		if (h.isNull()) return;

		ID3D11Buffer* cBuffer = nullptr;
		if ((cBuffer = resourceProvider->GetBuffer(h)) == nullptr) return;

		D3D11_BUFFER_DESC desc;
		cBuffer->GetDesc(&desc);

		if (desc.Usage != D3D11_USAGE_DYNAMIC) {
			context->UpdateSubresource(cBuffer, 0, nullptr, data, 0, 0);
			return;
		}

		D3D11_MAPPED_SUBRESOURCE mapped;
		ZeroMemory(&mapped, sizeof(D3D11_MAPPED_SUBRESOURCE));
		context->Map(cBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
		memcpy(mapped.pData, data, byteWidth);
		context->Unmap(cBuffer, 0);
	}

	void D3D11RenderContext::SetViewport(int width, int height) {
		D3D11_VIEWPORT viewport = {};
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = static_cast<FLOAT>(width);
		viewport.Height = static_cast<FLOAT>(height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		context->RSSetViewports(1, &viewport);
	}

	void D3D11RenderContext::BeginFrame() {
		SetViewport(width, height);
	}

	void D3D11RenderContext::DrawGeometryIndexed(MeshHandle handle, uint32_t subMeshIndex) {
		if (handle.isNull()) return;

		D3D11Mesh* mesh = resourceProvider->GetResource<D3D11Mesh>(handle);

		if (!mesh) {
			printf("No Mesh to bind of, Handle: %d \n", handle);
			return;
		}

		UINT offset = 0;
		UINT stride = mesh->vertexStride;

		context->IASetVertexBuffers(0, 1, &mesh->vertexBuffer, &stride, &offset);
		context->IASetIndexBuffer(mesh->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		const auto& subMesh = mesh->subMeshes[subMeshIndex];

		context->DrawIndexed(subMesh.indexCount, subMesh.indexOffset, subMesh.vertexOffset);
	}

	void D3D11RenderContext::Draw(uint64_t count) {
		context->Draw(count, 0);
	}

	void D3D11RenderContext::DrawIndexed(uint64_t count) {
		context->DrawIndexed(count, 0, 0);
	}

	void D3D11RenderContext::EndFrame() {
		swapChain->Present(1, 0);
		context->OMSetRenderTargets(0, nullptr, nullptr);
	}

	D3D11_PRIMITIVE_TOPOLOGY MyPrimitiveTopologyToD3D11(zRender::PrimitiveTopology topo) {
		switch (topo) {
		case zRender::PrimitiveTopology_Triangelist:
			return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		}

		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}

	void D3D11RenderContext::BindBufferVS(uint32_t slot, BufferHandle handle) {
		if (handle.isNull()) return;

		ID3D11Buffer* buffer = resourceProvider->GetBuffer(handle);
		if (!buffer) printf("No Buffer to bind to VS of, Handle: %d \n", handle);
		context->VSSetConstantBuffers(slot, 1, &buffer);
	}

	void D3D11RenderContext::BindBufferPS(uint32_t slot, BufferHandle handle) {
		if (handle.isNull()) return;

		ID3D11Buffer* buffer = resourceProvider->GetBuffer(handle);
		if (!buffer) printf("No Buffer to bind to PS of, Handle: %d \n", handle);
		context->PSSetConstantBuffers(slot, 1, &buffer);
	}

	void D3D11RenderContext::BindTextureVS(uint32_t slot, TextureHandle handle) {
		D3D11Texture* tex = resourceProvider->GetResource<D3D11Texture>(handle);
		if (!tex) {
			printf("No Texture to bind to VS of, Handle: %d \n", handle);
			ID3D11ShaderResourceView* srv = nullptr;
			ID3D11SamplerState* sampler = nullptr;
			context->VSSetShaderResources(slot, 1, &srv);
			context->VSSetSamplers(slot, 1, &sampler);
			return;
		}

		ID3D11ShaderResourceView* srv = tex->shaderResourceView;
		ID3D11SamplerState* sampler = tex->samplerState;
		context->VSSetShaderResources(slot, 1, &srv);
		context->VSSetSamplers(slot, 1, &sampler);
	}
	void D3D11RenderContext::BindTexturePS(uint32_t slot, TextureHandle handle) {
		D3D11Texture* tex = resourceProvider->GetResource<D3D11Texture>(handle);
		if (!tex) {
			printf("No Texture to bind to PS of, Handle: %d \n", handle);
			ID3D11ShaderResourceView* srv = nullptr;
			ID3D11SamplerState* sampler = nullptr;
			context->PSSetShaderResources(slot, 1, &srv);
			context->PSSetSamplers(slot, 1, &sampler);
			return;
		}

		ID3D11ShaderResourceView* srv = tex->shaderResourceView;
		ID3D11SamplerState* sampler = tex->samplerState;
		context->PSSetShaderResources(slot, 1, &srv);
		context->PSSetSamplers(slot, 1, &sampler);
	}

	void D3D11RenderContext::BindPipeline(const zRender::PipelineStateContainer& pipelineState) {
		D3D11Shader* shader = resourceProvider->GetResource<D3D11Shader>(pipelineState.shaderHandle);
		ID3D11RasterizerState* rasterizer = resourceProvider->GetRasterizerState(pipelineState.rasterizerHandle);
		ID3D11DepthStencilState* depthStencil = resourceProvider->GetDepthStencilState(pipelineState.depthStencilHandle);
		ID3D11BlendState* blendState = resourceProvider->GetBlendState(pipelineState.blendHandle);

		context->OMSetBlendState(blendState, nullptr, 0xFFFFFFFFu);

		context->OMSetDepthStencilState(depthStencil, 0);

		context->RSSetState(rasterizer);

		context->IASetPrimitiveTopology(MyPrimitiveTopologyToD3D11(pipelineState.topology));

		context->IASetInputLayout(shader ? shader->inputLayout : nullptr);

		context->VSSetShader(shader ? shader->vertexShader : nullptr, nullptr, 0);
		context->PSSetShader(shader ? shader->pixelShader : nullptr, nullptr, 0);
	}
}