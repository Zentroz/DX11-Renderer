#include"CommandContext.h"

void D3D11CommandContext::EndFrame() {
	context->OMSetRenderTargets(0, nullptr, nullptr);
}

void D3D11CommandContext::SetStorage(D3D11ResourceStorage* storage) {
	this->storage = storage;
}

void D3D11CommandContext::ClearRTV(TextureHandle handle, float clearColor[4]) {
	D3D11Texture* texture = storage->GetTexture(handle);

	context->ClearRenderTargetView(texture->rtv.Get(), clearColor);
}
void D3D11CommandContext::ClearDSV(TextureHandle handle) {
	D3D11Texture* texture = storage->GetTexture(handle);

	context->ClearDepthStencilView(texture->dsv.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3D11CommandContext::SetViewport(uint32_t width, uint32_t height) {
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<FLOAT>(width);
	viewport.Height = static_cast<FLOAT>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	context->RSSetViewports(1, &viewport);
}

void D3D11CommandContext::SetVertexBuffer(BufferHandle h) {
	D3D11Mesh* mesh = storage->GetMesh(h);

	context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &mesh->strides, &mesh->offset);
}
void D3D11CommandContext::SetIndexBuffer(BufferHandle h) {
	D3D11Mesh* mesh = storage->GetMesh(h);

	context->IASetIndexBuffer(mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}

void D3D11CommandContext::UpdateBuffer(BufferHandle h, uint32_t byteWidth, void* data) {
	ID3D11Buffer* cBuffer = storage->GetBuffer(h);

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

void D3D11CommandContext::SetBufferVS(BufferHandle handle, uint32_t slot) {
	ID3D11Buffer* buffer = storage->GetBuffer(handle);
	context->VSSetConstantBuffers(slot, 1, &buffer);
}
void D3D11CommandContext::SetBufferPS(BufferHandle handle, uint32_t slot) {
	ID3D11Buffer* buffer = storage->GetBuffer(handle);
	context->PSSetConstantBuffers(slot, 1, &buffer);	
}

void D3D11CommandContext::SetTextureVS(TextureHandle handle, uint32_t slot) {
	D3D11Texture* texture = storage->GetTexture(handle);
	context->VSSetShaderResources(slot, 1, texture->srv.GetAddressOf());
}
void D3D11CommandContext::SetTexturePS(TextureHandle handle, uint32_t slot) {
	D3D11Texture* texture = storage->GetTexture(handle);
	context->PSSetShaderResources(slot, 1, texture->srv.GetAddressOf());
}

void D3D11CommandContext::SetPipeline(PipelineHandle h) {
	D3D11Pipeline* pipeline = storage->GetPipeline(h);

	context->IASetPrimitiveTopology(pipeline->topology);

	context->IASetInputLayout(pipeline->inputLayout.Get());

	context->VSSetShader(pipeline->vertexShader.Get(), nullptr, 0);
	context->PSSetShader(pipeline->pixelShader.Get(), nullptr, 0);

	context->RSSetState(pipeline->rasterizerState.Get());
	context->OMSetDepthStencilState(pipeline->depthStencilState.Get(), 0);

	if (pipeline->blendState != nullptr) {
		float blendFactor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		context->OMSetBlendState(pipeline->blendState.Get(), blendFactor, 0xFFFFFFFF);
	}
	
	ID3D11SamplerState* samplers[16];

	for (size_t i = 0; i < 16; i++) {
		samplers[i] = pipeline->samplerStates[i] == nullptr ? nullptr : pipeline->samplerStates[i].Get();
	}

	context->PSSetSamplers(0, 16, samplers);
}

void D3D11CommandContext::SetRenderTarget(RTVHandle h) {
	if (h.isNull()) {
		context->OMSetRenderTargets(0, nullptr, nullptr);
		return;
	}

	D3D11Texture* texture = storage->GetTexture(h);

	context->OMSetRenderTargets(1, texture->rtv.GetAddressOf(), nullptr);
}

void D3D11CommandContext::SetRenderTargets(size_t renderViewCount, Handle* renderViews, Handle depthView) {
	std::vector<ID3D11RenderTargetView*> views;
	views.reserve(renderViewCount);

	for (size_t i = 0; i < renderViewCount; i++) {
		if (renderViews[i].isNull()) {
			views.clear();
			continue;
		}
		D3D11Texture* texture = storage->GetTexture(renderViews[i]);
		ID3D11RenderTargetView* rtv = texture->rtv.Get();
		views.push_back(rtv);
	}


	ID3D11DepthStencilView* dsv = nullptr;

	if (!depthView.isNull()) {
		D3D11Texture* tex = storage->GetTexture(depthView);
		dsv = tex->dsv.Get();
	}

	context->OMSetRenderTargets(views.size(), views.data(), dsv);
}

void D3D11CommandContext::Draw(uint32_t vertexCount, uint32_t startVertex) {
	context->Draw(vertexCount, startVertex);
}
void D3D11CommandContext::DrawIndexed(uint32_t indexCount, uint32_t startIndexLocation, int baseVertexLocation) {
	context->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
}