#include"RenderContext.h"

const float bgColor[4] = { 0.1f, 0.1f, 0.25f, 1.0f };

namespace zRender {
	D3D11RenderContext::D3D11RenderContext(ID3D11DeviceContext* context, IDXGISwapChain* swapChain, D3D11ResourceProvider* resourceProvider) :
		context(context), swapChain(swapChain), resourceProvider(resourceProvider) {

		depthStencil = resourceProvider->CreateDepthStencilView();
	}

	void D3D11RenderContext::SetRenderTargetView(ID3D11RenderTargetView* renderTarget) {
		this->renderTarget = renderTarget;
	}

	void D3D11RenderContext::UpdateBuffer(BufferHandle h, UINT byteWidth, void* data) {
		if (h == InvalidHandle) return;

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
		context->ClearRenderTargetView(renderTarget, bgColor);
		context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		context->OMSetRenderTargets(1, &renderTarget, depthStencil);

		SetViewport(1536, 793);
	}

	void BindMesh(const D3D11Mesh& mesh, ID3D11DeviceContext* context) {
		context->IASetVertexBuffers(0, 1, &mesh.vertexBuffer, &mesh.stride, &mesh.offset);
		context->IASetIndexBuffer(mesh.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	}

	void D3D11RenderContext::DrawGeometryIndexed(MeshHandle handle) {
		if (handle == InvalidHandle) return;

		D3D11Mesh* mesh = resourceProvider->GetResource<D3D11Mesh>(handle);

		if (!mesh) {
			printf("No Mesh to bind of, Handle: %d \n", handle);
			return;
		}

		BindMesh(*mesh, context);

		context->DrawIndexed(mesh->indexCount, 0, 0);
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
		if (handle == InvalidHandle) return;

		ID3D11Buffer* buffer = resourceProvider->GetBuffer(handle);
		if (!buffer) printf("No Buffer to bind to VS of, Handle: %d \n", handle);
		context->VSSetConstantBuffers(slot, 1, &buffer);
	}

	void D3D11RenderContext::BindBufferPS(uint32_t slot, BufferHandle handle) {
		if (handle == InvalidHandle) return;

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
		if (!shader) printf("No Shader to bind.\n");
		if (!rasterizer) printf("No Rasterizer to bind.\n");
		if (!depthStencil) printf("No DepthStencilState to bind.\n");

		context->OMSetDepthStencilState(depthStencil, 0);

		context->RSSetState(rasterizer);

		context->IASetPrimitiveTopology(MyPrimitiveTopologyToD3D11(pipelineState.topology));

		context->IASetInputLayout(shader->inputLayout);

		context->VSSetShader(shader->vertexShader, nullptr, 0);
		context->PSSetShader(shader->pixelShader, nullptr, 0);
	}
}