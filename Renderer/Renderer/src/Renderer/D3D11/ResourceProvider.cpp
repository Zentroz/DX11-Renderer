#include"ResourceProvider.h"
#include"Renderer/Core/ErrorLogger.h"
#include<fstream>

#include <iostream>
#include <comdef.h>
#include <string>

using namespace Microsoft::WRL;

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
	D3D11ResourceProvider::D3D11ResourceProvider(ID3D11Device* pDevice, ID3D11Texture2D* backBufferTexture) : device(pDevice), backBufferTexture(backBufferTexture) {
		/*
		CreateRasterizer(RasterizerFunc_CullMode_None, RasterizerFunc_FillMode_Solid);
		CreateRasterizer(RasterizerFunc_CullMode_Back, RasterizerFunc_FillMode_Solid);
		CreateRasterizer(RasterizerFunc_CullMode_Front, RasterizerFunc_FillMode_Solid);
		CreateRasterizer(RasterizerFunc_CullMode_None, RasterizerFunc_FillMode_Wireframe);
		CreateDepthStencilState(DepthWriteMask_All, DepthFunc_LessEqual);
		CreateDepthStencilState(DepthWriteMask_Zero, DepthFunc_LessEqual);
		CreateDepthStencilState(DepthWriteMask_All, DepthFunc_Less);
		CreateDepthStencilState(DepthWriteMask_Zero, DepthFunc_Less);
		CreateDepthStencilState(DepthWriteMask_Zero, DepthFunc_Never);
		*/
		screenTextureHandle = CreateTextureResource(backBufferTexture, TextureFormat_RGBA8_UNorm, TextureUsageFlags::TextureUsageFlag_RenderTarget, TextureFilter::Linear);
	}

	void D3D11ResourceProvider::ReleaseScreenTexture() {
		if (screenTextureHandle.isNull()) return;

		storage.RemoveTexture(screenTextureHandle);
	}
	

	void D3D11ResourceProvider::RecreateScreenTextureHandle() {
		if (screenTextureHandle.isNull()) return;

		uuid newId = CreateTextureResource(backBufferTexture, TextureFormat_RGBA8_UNorm, TextureUsageFlags::TextureUsageFlag_RenderTarget, TextureFilter::Linear);
		screenTextureHandle = newId;
	}
	

	MeshHandle D3D11ResourceProvider::LoadMesh(const Mesh& rawMesh) {
		D3D11Mesh mesh{};

		D3D11_BUFFER_DESC vDesc;
		ZeroMemory(&vDesc, sizeof(vDesc));
		vDesc.Usage = D3D11_USAGE_DEFAULT;
		vDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * rawMesh.vertices.size());
		vDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vDesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = rawMesh.vertices.data();

		HRESULT hr;

		hr = device->CreateBuffer(&vDesc, &initData, mesh.vertexBuffer.GetAddressOf());

		// Index buffer
		D3D11_BUFFER_DESC ibDesc = {};
		ibDesc.Usage = D3D11_USAGE_DEFAULT;
		ibDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * rawMesh.indices.size());
		ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibDesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA iinitData = {};
		iinitData.pSysMem = rawMesh.indices.data();

		hr = device->CreateBuffer(&ibDesc, &iinitData, mesh.indexBuffer.GetAddressOf());

		mesh.offset = 0;
		mesh.strides = sizeof(Vertex);

		/*
		for (const auto& sub : rawMesh.subMeshes) {
			mesh.subMeshesGPU.push_back({});

			SubMeshGPU& subMesh = mesh->subMeshesGPU.back();

			subMesh.vertexCount = sub.vertexCount;
			subMesh.vertexOffset = sub.vertexOffset;

			subMesh.indexCount = sub.indexCount;
			subMesh.indexOffset = sub.indexOffset;
		}
		*/

		return storage.AddMesh(mesh);
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


	ComPtr<ID3D11VertexShader> D3D11ResourceProvider::LoadVertexShader(const std::string& vertexShaderSrc, ComPtr<ID3DBlob>& blob) {
		ComPtr<ID3D11VertexShader> shader;

		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
		flags |= D3DCOMPILE_DEBUG;
#endif

		ID3DBlob* errBlob = nullptr;

		HRESULT hr;
		IncludeHandler handler;
		// VertexShader
		hr = D3DCompile(
			vertexShaderSrc.c_str(),
			vertexShaderSrc.length(),
			nullptr, nullptr, &handler,
			"VSMain", "vs_5_0",
			flags,
			0,
			&blob, &errBlob
		);

		if (FAILEDLOG(hr)) {
			LogShaderError(errBlob);
			return nullptr;
		}

		hr = device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, shader.GetAddressOf());

		if (FAILEDLOG(hr)) {
			LogShaderError(errBlob);
			return nullptr;
		}

		return shader;
	}

	ComPtr<ID3D11PixelShader> D3D11ResourceProvider::LoadPixelShader(const std::string& pixelShaderSrc, ComPtr<ID3DBlob>& blob) {
		ComPtr<ID3D11PixelShader> shader;

		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
		flags |= D3DCOMPILE_DEBUG;
#endif

		ID3DBlob* errBlob = nullptr;

		HRESULT hr;
		IncludeHandler handler;

		// PixelShader
		hr = D3DCompile(
			pixelShaderSrc.c_str(),
			pixelShaderSrc.length(),
			nullptr, nullptr, &handler,
			"PSMain", "ps_5_0",
			flags,
			0,
			&blob, &errBlob
		);

		if (FAILEDLOG(hr)) {
			LogShaderError(errBlob);
			return nullptr;
		}

		hr = device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, shader.GetAddressOf());

		if (FAILEDLOG(hr)) {
			LogShaderError(errBlob);
			return nullptr;
		}

		return shader;
	}

	ComPtr<ID3D11InputLayout> D3D11ResourceProvider::CreateInputLayout(ComPtr<ID3DBlob> vsBlob) {
		ComPtr<ID3D11InputLayout> inputLayout;

		D3D11_INPUT_ELEMENT_DESC elementDesc[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		HRESULT hr = device->CreateInputLayout(elementDesc, _countof(elementDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), inputLayout.GetAddressOf());

		if (FAILEDLOG(hr)) {
			return nullptr;
		}

		return inputLayout;
	}

	TextureHandle D3D11ResourceProvider::LoadTexture(const Texture& rawTexture) {
		ID3D11Texture2D* texture = nullptr;

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = rawTexture.width;
		desc.Height = rawTexture.height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//if (rawTexture.channels == 4 || rawTexture.channels == 3) {
		//	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//}
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = rawTexture.pixels;
		initData.SysMemPitch = rawTexture.width * 4;
		initData.SysMemSlicePitch = 0;

		if (FAILEDLOG(device->CreateTexture2D(&desc, &initData, &texture))) {
			return uuid();
		}

		return CreateTextureResource(texture, TextureFormat_RGBA8_UNorm, static_cast<TextureUsageFlags>(rawTexture.usageFlags), (TextureFilter)rawTexture.filterMode);
	}

	// Order of Textures : 0 = +X, 1 = -X, 2 = +Y, 3 = -Y, 4 = +Z, 5 = -Z
	TextureHandle D3D11ResourceProvider::LoadTextureCubeMap(const Texture textures[6]) {
		D3D11Texture texture{};

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = textures[0].width;
		desc.Height = textures[0].height;
		desc.MipLevels = 1;
		desc.ArraySize = 6;
		if (textures[0].channels == 4 || textures[0].channels == 3) {
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		}
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		std::vector<D3D11_SUBRESOURCE_DATA> subresources(6);

		for (int i = 0; i < 6; ++i)
		{
			subresources[i].pSysMem = textures[i].pixels;
			subresources[i].SysMemPitch = static_cast<UINT>(textures[i].width * 4);
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
		/*
		if (FAILEDLOG(hr = device->CreateSamplerState(&sampDesc, &texture->samplerState))) {
			printf("FAILEDLOG to create sampler.");
			return uuid();
		}
		*/
		if (FAILEDLOG(hr = device->CreateTexture2D(&desc, subresources.data(), texture.texture.GetAddressOf()))) {
			printf("FAILEDLOG to create texture2d.");
			return uuid();
		}
		if (FAILEDLOG(hr = device->CreateShaderResourceView(texture.texture.Get(), &srvDesc, texture.srv.GetAddressOf()))) {
			printf("FAILEDLOG to create srv.");
			return uuid();
		}

		return storage.AddTexture(texture);
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

		device->CreateBuffer(&desc, &initialData, &constantBuffer);

		return storage.AddBuffer(constantBuffer);
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
		case DepthFunc_Less:
			return D3D11_COMPARISON_LESS;
		case DepthFunc_Greater:
			return D3D11_COMPARISON_GREATER;
		case DepthFunc_GreaterEqual:
			return D3D11_COMPARISON_GREATER_EQUAL;
		}

		return D3D11_COMPARISON_LESS_EQUAL;
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
		case zRender::TextureFormat::TextureFormat_R32Uint:
			return DXGI_FORMAT_R32_UINT;
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
		if (FAILEDLOG(hr)) {
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
	TextureHandle D3D11ResourceProvider::CreateTexture(int width, int height, TextureUsageFlags usageFlags, TextureFilter filter, vec4 initialColor) {
		int pixelCount = width * height;

		std::vector<unsigned char> pixels{};
		pixels.resize(pixelCount * 4);

		for (size_t i = 0; i < pixelCount; i++) {
			pixels[i * 4 + 0] = 255 * initialColor.x;
			pixels[i * 4 + 1] = 255 * initialColor.y;
			pixels[i * 4 + 2] = 255 * initialColor.z;
			pixels[i * 4 + 3] = 255 * initialColor.w;
		}

		ID3D11Texture2D* texture = nullptr;

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = pixels.data();
		initData.SysMemPitch = width * 4;
		initData.SysMemSlicePitch = 0;

		if (FAILEDLOG(device->CreateTexture2D(&desc, &initData, &texture))) {
			printf("FAILEDLOG to create texture2d.");
			return uuid();
		}

		return CreateTextureResource(texture, TextureFormat_RGBA8_UNorm, static_cast<TextureUsageFlags>(usageFlags), filter);
	}

	TextureHandle D3D11ResourceProvider::CreateTexture(int width, int height, TextureFormat format, TextureUsageFlags usageFlags, TextureFilter filter) {
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

		HRESULT hr = device->CreateTexture2D(&desc, nullptr, &texture);

		if (FAILEDLOG(hr)) {
			printf("FAILEDLOG to create texture2d.");
			return uuid();
		}

		return CreateTextureResource(texture, format, usageFlags, filter);
	}

	TextureHandle D3D11ResourceProvider::CreateTextureResource(ID3D11Texture2D* texture, TextureFormat format, TextureUsageFlags usageFlags, TextureFilter filter) {
		D3D11Texture tex{};
		tex.texture = texture;
		texture->GetDesc(&tex.desc);

		bool createRTV = HasFlag(usageFlags, TextureUsageFlags::TextureUsageFlag_RenderTarget);
		bool createSRV = HasFlag(usageFlags, TextureUsageFlags::TextureUsageFlag_ShaderResource);
		bool createDSV = HasFlag(usageFlags, TextureUsageFlags::TextureUsageFlag_DepthStencil);
		bool depthTexture = createSRV && createDSV;

		if (createDSV && createRTV) {
			printf("Can't create a texture with RenderTargetView and DepthStencilView");
			return uuid();
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = depthTexture ? DXGI_FORMAT_R32_FLOAT : MyTextureFormatToDX11(format);
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		HRESULT hr;

		if (createSRV) {
			if (FAILEDLOG(hr = device->CreateShaderResourceView(texture, &srvDesc, tex.srv.GetAddressOf()))) {
				printf("FAILEDLOG to create srv.");
				return uuid();
			}
		}
		if (createRTV) {
			if (FAILEDLOG(device->CreateRenderTargetView(texture, nullptr, tex.rtv.GetAddressOf()))) {
				printf("FAILEDLOG to create rtv.");
				return uuid();
			}
		}
		if (createDSV) {
			if (FAILEDLOG(device->CreateDepthStencilView(texture, depthTexture ? &dsvDesc : nullptr, tex.dsv.GetAddressOf()))) {
				printf("FAILEDLOG to create dsv.");
				return uuid();
			}
		}

		return storage.AddTexture(tex);
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

	ComPtr<ID3D11RasterizerState> D3D11ResourceProvider::CreateRasterizer(zRender::RasterizerCullMode cullMode, zRender::RasterizerFillMode fillMode) {
		ComPtr<ID3D11RasterizerState> state = nullptr;

		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		D3D11_CULL_MODE cull = MyCullToD3D11(cullMode);
		D3D11_FILL_MODE fill = MyFillToD3D11(fillMode);
		desc.CullMode = cull;
		desc.FillMode = fill;
		desc.FrontCounterClockwise = FALSE;
		desc.DepthClipEnable = TRUE;

		device->CreateRasterizerState(&desc, &state);

		return state;
	}

	ComPtr<ID3D11DepthStencilState> D3D11ResourceProvider::CreateDepthStencilState(DepthWriteMask write, DepthFunc func) {
		ComPtr<ID3D11DepthStencilState> state;

		D3D11_DEPTH_STENCIL_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.DepthEnable = TRUE;
		desc.DepthWriteMask = MyDepthMaskToDx(write);
		desc.DepthFunc = MyDepthFuncToDx(func);
		desc.StencilEnable = FALSE;

		device->CreateDepthStencilState(&desc, &state);

		return state;
	}

	ComPtr<ID3D11BlendState> D3D11ResourceProvider::CreateBlendState(bool blendEnable) {
		ComPtr<ID3D11BlendState> state = nullptr;

		D3D11_BLEND_DESC desc{};
		ZeroMemory(&desc, sizeof(D3D11_BLEND_DESC));
		if (blendEnable) {
			desc.RenderTarget[0].BlendEnable = true;
			desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
			desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		}
		else {
			desc.RenderTarget[0].BlendEnable = false;
			desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		}

		HRESULT hr = device->CreateBlendState(&desc, &state);
		FAILEDLOG(hr);
		assert(SUCCEEDED(hr));

		return state;
	}

	Microsoft::WRL::ComPtr<ID3D11SamplerState> D3D11ResourceProvider::CreateSamplerState(TextureFilter sampleMode) {
		Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

		D3D11_SAMPLER_DESC desc{};
		desc.Filter = (sampleMode == TextureFilter::Linear) ? D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR : D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

		device->CreateSamplerState(&desc, samplerState.GetAddressOf());

		return samplerState;
	}

	PipelineHandle D3D11ResourceProvider::CreatePipeline(const PipelineDesc& desc) {
		D3D11Pipeline pipeline;

		pipeline.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		ComPtr<ID3DBlob> vsBlob;
		ComPtr<ID3DBlob> psBlob;
		pipeline.vertexShader = LoadVertexShader(desc.vertexShaderSrc, vsBlob);
		pipeline.pixelShader = LoadPixelShader(desc.pixelShaderSrc, psBlob);

		pipeline.inputLayout = CreateInputLayout(vsBlob.Get());

		pipeline.rasterizerState = CreateRasterizer(desc.rasterizerCullMode, desc.rasterizerFillMode);
		pipeline.depthStencilState = CreateDepthStencilState(desc.depthWriteMask, desc.depthFunc);

		pipeline.samplerStates.resize(16);
		for (size_t i = 0; i < 16; i++) {
			if (i >= desc.samplerFilterModes.size()) {
				pipeline.samplerStates[i] = nullptr;
				continue;
			}

			pipeline.samplerStates[i] = CreateSamplerState(desc.samplerFilterModes[i]);
		}

		return storage.AddPipeline(pipeline);
	}
}