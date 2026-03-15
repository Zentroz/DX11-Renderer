#pragma once

#include<wrl.h>
#include<d3d11.h>
#include<vector>

struct D3D11Pipeline {
	// Topology
	D3D11_PRIMITIVE_TOPOLOGY topology;

	// Input Layout
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	// Shaders
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;

	// States
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
	std::vector<Microsoft::WRL::ComPtr<ID3D11SamplerState>> samplerStates;
};

struct D3D11Texture {
	// Texture Desc
	D3D11_TEXTURE2D_DESC desc;
	
	// TextureObject
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;

	// Views
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv;
};

struct D3D11Mesh {
	UINT offset;
	UINT strides;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
};