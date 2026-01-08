#include"ResourceProvider.h"
#include<fstream>

#include <iostream>
#include <comdef.h>
#include <string>

std::vector<std::vector<D3D11_INPUT_ELEMENT_DESC>> inputLayouts = {
	{},
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

namespace zRender {
	D3D11ResourceProvider::D3D11ResourceProvider(D3D11Device* pDevice) : device(pDevice) {
		CreateRasterizer(RasterizerFunc_CullMode_None, RasterizerFunc_FillMode_Solid);
		CreateRasterizer(RasterizerFunc_CullMode_Back, RasterizerFunc_FillMode_Solid);
		CreateRasterizer(RasterizerFunc_CullMode_Front, RasterizerFunc_FillMode_Solid);
		CreateRasterizer(RasterizerFunc_CullMode_None, RasterizerFunc_FillMode_Wireframe);
		CreateDepthStencilState(DepthWriteMask_All, DepthFunc_LessEqual);
		CreateDepthStencilState(DepthWriteMask_Zero, DepthFunc_LessEqual);
		screenTextureHandle = CreateTextureResource(device->GetBackBufferTexture(), TextureUsageFlags::TextureUsageFlag_RenderTarget);
	}

	MeshHandle D3D11ResourceProvider::LoadMesh(const MeshCPU& rawMesh) {
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

		mesh->vertexStride = sizeof(Vertex);

		for (const auto& sub : rawMesh.subMeshes) {
			mesh->subMeshes.push_back({});

			SubMeshGPU& subMesh = mesh->subMeshes.back();

			subMesh.vertexCount = sub.vertexCount;
			subMesh.vertexOffset = sub.vertexOffset;

			subMesh.indexCount = sub.indexCount;
			subMesh.indexOffset = sub.indexOffset;
		}

		MeshHandle handle = m_MeshMap.size() + 1;
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


	ShaderHandle D3D11ResourceProvider::LoadShader(const ShaderCPU& rawShader) {
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

		if (rawShader.inputLayout != InputLayout_None) 
			hr = device->GetDevice()->CreateInputLayout(inputLayouts[rawShader.inputLayout].data(), inputLayouts[rawShader.inputLayout].size(), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &shader->inputLayout);

		if (vsBlob) vsBlob->Release();
		if (psBlob) psBlob->Release();

		ShaderHandle handle = m_ShaderMap.size() + 1;
		m_ShaderMap[handle] = shader;
		return handle;
	}

	TextureHandle D3D11ResourceProvider::LoadTexture(const TextureCPU& rawTexture) {
		ID3D11Texture2D* texture = nullptr;

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

		if (FAILED(device->GetDevice()->CreateTexture2D(&desc, &initData, &texture))) {
			printf("Failed to create texture2d.");
			return InvalidHandle;
		}

		return CreateTextureResource(texture, static_cast<TextureUsageFlags>(rawTexture.usageFlags));
	}

	// Order of Textures : 0 = +X, 1 = -X, 2 = +Y, 3 = -Y, 4 = +Z, 5 = -Z
	TextureHandle D3D11ResourceProvider::LoadTextureCubeMap(const TextureCPU rawTextures[6]) {
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

		TextureHandle handle = m_TextureMap.size() + 1;
		m_TextureMap[handle] = texture;
		return handle;
	}

	BufferHandle D3D11ResourceProvider::CreateBuffer(zRender::Buffer_Usage usage, int accessFlags, UINT byteWidth, void* initData) {
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

		BufferHandle h = m_BufferMap.size() + 1;
		m_BufferMap[h] = constantBuffer;
		return h;
	}

	ID3D11Buffer* D3D11ResourceProvider::GetBuffer(BufferHandle h) {
		if (!m_BufferMap.contains(h)) return nullptr;
		return m_BufferMap[h];
	}

	ID3D11RasterizerState* D3D11ResourceProvider::GetRasterizerState(RasterizerHandle h) {
		return m_RasterizerStateMap[h];
	}

	ID3D11DepthStencilView* D3D11ResourceProvider::CreateDepthStencilView() {
		return nullptr;
	}

	D3D11_DEPTH_WRITE_MASK MyDepthMaskToDx(DepthWriteMask write) {
		switch (write) {
		case DepthWriteMask_All:
			return D3D11_DEPTH_WRITE_MASK_ALL;
		case DepthWriteMask_Zero:
			return D3D11_DEPTH_WRITE_MASK_ZERO;
		}

		return D3D11_DEPTH_WRITE_MASK_ZERO;
	}
	D3D11_COMPARISON_FUNC MyDepthFuncToDx(DepthFunc func) {
		switch (func) {
		case DepthFunc_Always:
			return D3D11_COMPARISON_ALWAYS;
		case DepthFunc_Never:
			return D3D11_COMPARISON_NEVER;
		case DepthFunc_LessEqual:
			return D3D11_COMPARISON_LESS_EQUAL;
		}

		return D3D11_COMPARISON_LESS_EQUAL;
	}

	DepthStateHandle D3D11ResourceProvider::CreateDepthStencilState(DepthWriteMask write, DepthFunc func) {
		ID3D11DepthStencilState* state;

		D3D11_DEPTH_STENCIL_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.DepthEnable = TRUE;
		desc.DepthWriteMask = MyDepthMaskToDx(write);
		desc.DepthFunc = MyDepthFuncToDx(func);
		desc.StencilEnable = FALSE;

		device->GetDevice()->CreateDepthStencilState(&desc, &state);

		DepthStateHandle h = m_DepthStencilStateMap.size() + 1;
		m_DepthStencilStateMap[h] = state;
		return h;
	}

	DepthStateHandle D3D11ResourceProvider::GetDepthStateHandle(DepthWriteMask write, DepthFunc function) {
		D3D11_DEPTH_WRITE_MASK mask = MyDepthMaskToDx(write);
		D3D11_COMPARISON_FUNC func = MyDepthFuncToDx(function);

		for (const auto& [key, val] : m_DepthStencilStateMap) {

			D3D11_DEPTH_STENCIL_DESC desc;
			val->GetDesc(&desc);

			if (desc.DepthFunc == func && desc.DepthWriteMask == mask) {
				return key;
			}
		}

		return InvalidHandle;
	}

	ID3D11DepthStencilState* D3D11ResourceProvider::GetDepthStencilState(DepthStateHandle h) {
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
		case zRender::TextureFormat::TextureFormat_RGB10A2_UNorm:
			return DXGI_FORMAT_R10G10B10A2_UNORM;
		case zRender::TextureFormat::TextureFormat_RGBA16F:
			return DXGI_FORMAT_R16G16B16A16_FLOAT;
		case zRender::TextureFormat::TextureFormat_R32_Typeless:
			return DXGI_FORMAT_R32_TYPELESS;
		}

		return DXGI_FORMAT_R8G8B8A8_UNORM;
	}
	///*
	void CheckHResult(HRESULT hr, const std::string& message) {
		if (FAILED(hr)) {
			_com_error err(hr);
			// err.ErrorMessage() returns a wide character string (LPCTSTR)
			// You might need a conversion if your project uses multi-byte characters
			// For Unicode projects (default in modern VS), this works fine
			MessageBox(
				nullptr,
				err.ErrorMessage(),
				L"DirectX Error",
				MB_OK | MB_ICONERROR
			);
			std::cerr << "Error: " << message << std::endl;
			// Optionally exit or throw an exception
			// exit(-1); 
		}
	}
	//*/
	TextureHandle D3D11ResourceProvider::CreateTexture(int width, int height, TextureFormat format, TextureUsageFlags usageFlags) {
		ID3D11Texture2D* texture = nullptr;

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = static_cast<UINT>(width);
		desc.Height = static_cast<UINT>(height);
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = MyTextureFormatToDX11(format);
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		UINT bindFlags = 0;
		if (HasFlag(usageFlags, TextureUsageFlags::TextureUsageFlag_ShaderResource)) {
			bindFlags |= D3D11_BIND_SHADER_RESOURCE;
		}
		if (HasFlag(usageFlags, TextureUsageFlags::TextureUsageFlag_RenderTarget)) {
			bindFlags |= D3D11_BIND_RENDER_TARGET;
		}
		if (HasFlag(usageFlags, TextureUsageFlags::TextureUsageFlag_DepthStencil)) {
			bindFlags |= D3D11_BIND_DEPTH_STENCIL;
		}
		desc.BindFlags = bindFlags;

		HRESULT hr = device->GetDevice()->CreateTexture2D(&desc, nullptr, &texture);

		if (FAILED(hr)) {
			printf("Failed to create texture2d.");
			return InvalidHandle;
		}

		return CreateTextureResource(texture, usageFlags);
	}

	TextureHandle D3D11ResourceProvider::CreateTextureResource(ID3D11Texture2D* texture, TextureUsageFlags usageFlags) {
		std::shared_ptr<D3D11Texture> tex = std::make_shared<D3D11Texture>();
		tex->texture = texture;

		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

		if (FAILED(device->GetDevice()->CreateSamplerState(&sampDesc, &tex->samplerState))) {
			printf("Failed to create sampler.");
			return InvalidHandle;
		}

		bool createRTV = HasFlag(usageFlags, TextureUsageFlags::TextureUsageFlag_RenderTarget);
		bool createSRV = HasFlag(usageFlags, TextureUsageFlags::TextureUsageFlag_ShaderResource);
		bool createDSV = HasFlag(usageFlags, TextureUsageFlags::TextureUsageFlag_DepthStencil);
		bool depthTexture = createSRV && createDSV;

		if (createDSV && createRTV) {
			printf("Can't create a texture with RenderTargetView and DepthStencilView");
			return InvalidHandle;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		HRESULT hr;

		if (createSRV) {
			if (FAILED(hr = device->GetDevice()->CreateShaderResourceView(texture, depthTexture ? &srvDesc : nullptr, &tex->shaderResourceView))) {
				printf("Failed to create srv.");
				return InvalidHandle;
			}
		}
		if (createRTV) {
			if (FAILED(device->GetDevice()->CreateRenderTargetView(texture, nullptr, &tex->renderTargetView))) {
				printf("Failed to create rtv.");
				return InvalidHandle;
			}
		}
		if (createDSV) {
			if (FAILED(device->GetDevice()->CreateDepthStencilView(texture, depthTexture ? &dsvDesc : nullptr, &tex->depthStencilView))) {
				printf("Failed to create dsv.");
				return InvalidHandle;
			}
		}

		TextureHandle h = m_TextureMap.size() + 1;
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

	RasterizerHandle D3D11ResourceProvider::CreateRasterizer(zRender::RasterizerCullMode cullMode, zRender::RasterizerFillMode fillMode) {
		ID3D11RasterizerState* state = nullptr;

		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		D3D11_CULL_MODE cull = MyCullToD3D11(cullMode);
		D3D11_FILL_MODE fill = MyFillToD3D11(fillMode);
		desc.CullMode = cull;
		desc.FillMode = fill;
		desc.FrontCounterClockwise = FALSE;
		desc.DepthClipEnable = TRUE;

		device->GetDevice()->CreateRasterizerState(&desc, &state);

		RasterizerHandle h = m_RasterizerStateMap.size() + 1;
		m_RasterizerStateMap[h] = state;
		return h;
	}

	RasterizerHandle D3D11ResourceProvider::GetRasteriserHandle(zRender::RasterizerCullMode cullMode, zRender::RasterizerFillMode fillMode) {
		D3D11_CULL_MODE dxCull = MyCullToD3D11(cullMode);
		D3D11_FILL_MODE dxFill = MyFillToD3D11(fillMode);

		for (const auto& [key, val] : m_RasterizerStateMap) {
			D3D11_RASTERIZER_DESC desc;
			val->GetDesc(&desc);

			if (desc.CullMode == dxCull && desc.FillMode == dxFill) {
				return key;
			}
		}

		return InvalidHandle;
	}
}