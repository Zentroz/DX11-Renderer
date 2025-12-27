#include"ResourceProvider.h"
#include<fstream>

std::vector<std::vector<D3D11_INPUT_ELEMENT_DESC>> inputLayouts = {
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	},
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	}
};

D3D11ResourceProvider::D3D11ResourceProvider(D3D11Device* pDevice) : device(pDevice) {
	CreateRasterizer(zRender::RasterizerFunc_CullMode_None, zRender::RasterizerFunc_FillMode_Solid);
	CreateRasterizer(zRender::RasterizerFunc_CullMode_Front, zRender::RasterizerFunc_FillMode_Solid);
	CreateDepthStencilState();
}

Handle D3D11ResourceProvider::LoadMesh(const MeshCPU& rawMesh) {
	std::shared_ptr<D3D11Mesh> mesh = std::make_shared<D3D11Mesh>();

	D3D11_BUFFER_DESC vDesc;
	ZeroMemory(&vDesc, sizeof(vDesc));
	vDesc.Usage = D3D11_USAGE_DEFAULT;
	vDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * rawMesh.vertices.size());
	vDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = rawMesh.vertices.data();

	HRESULT hr;

	hr = device->GetDevice()->CreateBuffer(&vDesc, &initData, &mesh->vertexBuffer);

	// Index buffer
	D3D11_BUFFER_DESC ibDesc = {};
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * rawMesh.indices.size());
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData = {};
	iinitData.pSysMem = rawMesh.indices.data();

	hr = device->GetDevice()->CreateBuffer(&ibDesc, &iinitData, &mesh->indexBuffer);

	mesh->indexCount = static_cast<UINT>(rawMesh.indexCount);
	mesh->stride = sizeof(Vertex);

	Handle handle = m_MeshMap.size() + 1;
	m_MeshMap[handle] = mesh;
	return handle;
}

void LogShaderError(ID3DBlob* errBlob) {
	const char* compileErrors = static_cast<const char*>(errBlob->GetBufferPointer());

	printf("[SHADER ERR] : %s", compileErrors);

	errBlob->Release();
}

class IncludeHandler : public ID3DInclude {
public:
	HRESULT Open(
		D3D_INCLUDE_TYPE,
		LPCSTR fileName,
		LPCVOID,
		LPCVOID* data,
		UINT* size) override
	{
		std::ifstream file("Assets/Shaders/" + std::string(fileName), std::ios::binary);
		if (!file) return E_FAIL;

		file.seekg(0, std::ios::end);
		*size = (UINT)file.tellg();
		file.seekg(0);

		char* buffer = new char[*size];
		file.read(buffer, *size);
		*data = buffer;

		return S_OK;
	}

	HRESULT Close(LPCVOID data) override {
		delete[] reinterpret_cast<const char*>(data);
		return S_OK;
	}
};


Handle D3D11ResourceProvider::LoadShader(const ShaderCPU& rawShader) {
	std::shared_ptr<D3D11Shader> shader = std::make_shared<D3D11Shader>();

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	flags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;
	ID3DBlob* errBlob = nullptr;

	HRESULT hr;
	IncludeHandler handler;
	// VertexShader
	hr = D3DCompile(
		rawShader.vertexShaderSrc.c_str(),
		rawShader.vertexShaderSrc.length(),
		nullptr, nullptr, &handler,
		"VSMain", "vs_5_0",
		flags,
		0,
		&vsBlob, &errBlob
	);

	if (FAILED(hr)) {
		LogShaderError(errBlob);
		return InvalidHandle;
	}

	// PixelShader
	hr = D3DCompile(
		rawShader.pixelShaderSrc.c_str(),
		rawShader.pixelShaderSrc.length(),
		nullptr, nullptr, &handler,
		"PSMain", "ps_5_0",
		flags,
		0,
		&psBlob, &errBlob
	);

	if (FAILED(hr)) {
		LogShaderError(errBlob);
		return InvalidHandle;
	}

	hr = device->GetDevice()->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &shader->vertexShader);
	hr = device->GetDevice()->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &shader->pixelShader);

	hr = device->GetDevice()->CreateInputLayout(inputLayouts[rawShader.inputLayout].data(), inputLayouts[rawShader.inputLayout].size(), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &shader->inputLayout);

	if (vsBlob) vsBlob->Release();
	if (psBlob) psBlob->Release();

	Handle handle = m_ShaderMap.size() + 1;
	m_ShaderMap[handle] = shader;
	return handle;
}

Handle D3D11ResourceProvider::LoadTexture(const TextureCPU& rawTexture) {
	std::shared_ptr<D3D11Texture> texture = std::make_shared<D3D11Texture>();

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = rawTexture.width;
	desc.Height = rawTexture.height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;

	if (rawTexture.channels == 4 || rawTexture.channels == 3) {
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	}

	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = rawTexture.pixels;
	initData.SysMemPitch = rawTexture.width * 4;
	initData.SysMemSlicePitch = 0;

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	HRESULT hr;
	if (FAILED(hr = device->GetDevice()->CreateSamplerState(&sampDesc, &texture->samplerState))) {
		printf("Failed to create sampler.");
		return InvalidHandle;
	}
	if (FAILED(hr = device->GetDevice()->CreateTexture2D(&desc, &initData, &texture->texture))) {
		printf("Failed to create texture2d.");
		return InvalidHandle;
	}
	if (FAILED(hr = device->GetDevice()->CreateShaderResourceView(texture->texture, nullptr, &texture->shaderResourceView))) {
		printf("Failed to create srv.");
		return InvalidHandle;
	}

	Handle handle = m_TextureMap.size() + 1;
	m_TextureMap[handle] = texture;
	return handle;
}

// Order of Textures : 0 = +X, 1 = -X, 2 = +Y, 3 = -Y, 4 = +Z, 5 = -Z
Handle D3D11ResourceProvider::LoadTextureCubeMap(const TextureCPU rawTextures[6]) {
	std::shared_ptr<D3D11Texture> texture = std::make_shared<D3D11Texture>();

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = rawTextures[0].width;
	desc.Height = rawTextures[0].height;
	desc.MipLevels = 1;
	desc.ArraySize = 6;
	if (rawTextures[0].channels == 4 || rawTextures[0].channels == 3) {
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	}
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	std::vector<D3D11_SUBRESOURCE_DATA> subresources(6);

	for (int i = 0; i < 6; ++i)
	{
		subresources[i].pSysMem = rawTextures[i].pixels;
		subresources[i].SysMemPitch = static_cast<UINT>(rawTextures[i].width * 4);
		subresources[i].SysMemSlicePitch = 0;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = 1;

	ID3D11ShaderResourceView* skyboxSRV = nullptr;

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	HRESULT hr;
	if (FAILED(hr = device->GetDevice()->CreateSamplerState(&sampDesc, &texture->samplerState))) {
		printf("Failed to create sampler.");
		return InvalidHandle;
	}
	if (FAILED(hr = device->GetDevice()->CreateTexture2D(&desc, subresources.data(), &texture->texture))) {
		printf("Failed to create texture2d.");
		return InvalidHandle;
	}
	if (FAILED(hr = device->GetDevice()->CreateShaderResourceView(texture->texture, &srvDesc, &texture->shaderResourceView))) {
		printf("Failed to create srv.");
		return InvalidHandle;
	}

	Handle handle = m_TextureMap.size() + 1;
	m_TextureMap[handle] = texture;
	return handle;
}

Handle D3D11ResourceProvider::CreateBuffer(zRender::Buffer_Usage usage, int accessFlags, UINT byteWidth, void* initData) {
	ID3D11Buffer* constantBuffer = nullptr;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = (D3D11_USAGE)usage;
	desc.ByteWidth = byteWidth;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	UINT flags = 0;
	if ((accessFlags & zRender::Buffer_CPU_Write) != zRender::Buffer_CPU_None) flags |= D3D11_CPU_ACCESS_WRITE;
	if ((accessFlags & zRender::Buffer_CPU_Read) != zRender::Buffer_CPU_None) flags |= D3D11_CPU_ACCESS_READ;

	desc.CPUAccessFlags = flags;

	D3D11_SUBRESOURCE_DATA initialData;
	initialData.pSysMem = initData;

	device->GetDevice()->CreateBuffer(&desc, &initialData, &constantBuffer);

	Handle h = m_BufferMap.size() + 1;
	m_BufferMap[h] = constantBuffer;
	return h;
}

ID3D11Buffer* D3D11ResourceProvider::GetBuffer(Handle h) {
	if (!m_BufferMap.contains(h)) return nullptr;
	return m_BufferMap[h];
}

ID3D11RasterizerState* D3D11ResourceProvider::GetRasterizerState() {
	return m_RasterizerStateMap[1];
}

ID3D11DepthStencilView* D3D11ResourceProvider::CreateDepthStencilView() {
	ID3D11DepthStencilView* view = nullptr;

	// Creating depth stencil texture
	ID3D11Texture2D* pDepthStencil = nullptr;
	D3D11_TEXTURE2D_DESC descDepth = {};
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = 1536;
	descDepth.Height = 793;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	device->GetDevice()->CreateTexture2D(&descDepth, nullptr, &pDepthStencil);

	// Creating depth stencil view
	if (pDepthStencil) device->GetDevice()->CreateDepthStencilView(pDepthStencil, nullptr, &view);
	else return nullptr;

	pDepthStencil->Release();

	return view;
}

Handle D3D11ResourceProvider::CreateDepthStencilState() {
	ID3D11DepthStencilState* state;

	D3D11_DEPTH_STENCIL_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.DepthEnable = TRUE;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	desc.StencilEnable = FALSE;

	device->GetDevice()->CreateDepthStencilState(&desc, &state);

	Handle h = m_DepthStencilStateMap.size() + 1;
	m_DepthStencilStateMap[h] = state;
	return h;
}
ID3D11DepthStencilState* D3D11ResourceProvider::GetDepthStencilState(Handle h) {
	if (!m_DepthStencilStateMap.contains(h) || h == InvalidHandle) return nullptr;

	return m_DepthStencilStateMap[h];
}

DXGI_FORMAT MyTextureFormatToDX11(zRender::TextureFormat format) {
	switch (format) {
		case zRender::TextureFormat::TextureFormat_Depth16:
			return DXGI_FORMAT_D16_UNORM;
		case zRender::TextureFormat::TextureFormat_Depth32:
			return DXGI_FORMAT_D32_FLOAT;
		case zRender::TextureFormat::TextureFormat_Depth24Stencil8:
			return DXGI_FORMAT_D24_UNORM_S8_UINT;
		case zRender::TextureFormat::TextureFormat_R16F:
			return DXGI_FORMAT_R16_FLOAT;
		case zRender::TextureFormat::TextureFormat_R32F:
			return DXGI_FORMAT_R32_FLOAT;
		case zRender::TextureFormat::TextureFormat_RGBA8_sRGB:
			return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		case zRender::TextureFormat::TextureFormat_RGBA8_UNorm:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
	}

	return DXGI_FORMAT_R8G8B8A8_UNORM;
}

Handle D3D11ResourceProvider::CreateTexture(int width, int height, zRender::TextureFormat format) {
	ID3D11Texture2D* texture = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;
	ID3D11SamplerState* sampler = nullptr;

	D3D11_TEXTURE2D_DESC tDesc;
	ZeroMemory(&tDesc, sizeof(tDesc));
	tDesc.Format = MyTextureFormatToDX11(format);
	tDesc.Width = static_cast<UINT>(width);
	tDesc.Height = static_cast<UINT>(height);
	tDesc.MipLevels = 1;
	tDesc.ArraySize = 1;
	tDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	device->GetDevice()->CreateTexture2D(&tDesc, nullptr, &texture);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = MyTextureFormatToDX11(format);
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	if (!texture) return InvalidHandle;
	device->GetDevice()->CreateShaderResourceView(texture, &srvDesc, &srv);

	D3D11_SAMPLER_DESC sDesc;
	sDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	sDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sDesc.BorderColor[0] = 1.0f;
	sDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;

	std::shared_ptr<D3D11Texture> tex = std::make_shared<D3D11Texture>();
	tex->texture = texture;
	tex->shaderResourceView = srv;
	tex->samplerState = sampler;

	Handle h = m_TextureMap.size() + 1;
	m_TextureMap[h] = tex;
	return h;
}

D3D11_CULL_MODE MyCullToD3D11(zRender::RasterizerCullMode mode) {
	switch (mode) {
	case zRender::RasterizerFunc_CullMode_Back:
		return D3D11_CULL_BACK;
	case zRender::RasterizerFunc_CullMode_Front:
		return D3D11_CULL_FRONT;
	}

	return D3D11_CULL_NONE;
}

D3D11_FILL_MODE MyFillToD3D11(zRender::RasterizerFillMode mode) {
	switch (mode) {
	case zRender::RasterizerFunc_FillMode_Solid:
		return D3D11_FILL_SOLID;
	case zRender::RasterizerFunc_FillMode_Wireframe:
		return D3D11_FILL_WIREFRAME;
	}

	return D3D11_FILL_SOLID;
}

Handle D3D11ResourceProvider::CreateRasterizer(zRender::RasterizerCullMode cullMode, zRender::RasterizerFillMode fillMode) {
	ID3D11RasterizerState* state = nullptr;

	D3D11_RASTERIZER_DESC solidDesc;
	ZeroMemory(&solidDesc, sizeof(solidDesc));
	solidDesc.CullMode = MyCullToD3D11(cullMode);
	solidDesc.FillMode = MyFillToD3D11(fillMode);
	solidDesc.FrontCounterClockwise = FALSE;
	solidDesc.DepthClipEnable = TRUE;

	device->GetDevice()->CreateRasterizerState(&solidDesc, &state);

	Handle h = m_RasterizerStateMap.size() + 1;
	m_RasterizerStateMap[h] = state;
	return h;
}

Handle D3D11ResourceProvider::GetRasteriserHandle(zRender::RasterizerCullMode cullMode, zRender::RasterizerFillMode fillMode) {
	for (const auto& [key, val] : m_RasterizerStateMap) {
		D3D11_CULL_MODE dxCull = MyCullToD3D11(cullMode);
		D3D11_FILL_MODE dxFill = MyFillToD3D11(fillMode);

		D3D11_RASTERIZER_DESC desc;
		val->GetDesc(&desc);

		if (desc.CullMode == dxCull && desc.FillMode == desc.FillMode) return key;
	}

	return InvalidHandle;
}