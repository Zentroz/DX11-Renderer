#include"Window/Window.h"
#include"Utility/FreelookCamera.h"
#include"Utility/AssetLoader.h"
#include"Editor/UI.h"
#include"Editor/ObjectPanel.h"

#include<Renderer/D3D11/GraphicsDevice.h>
#include<Renderer/D3D11/RenderContext.h>
#include<Renderer/D3D11/ResourceProvider.h>
#include<Renderer/Renderer.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include<stb_image/stb_image.h>
#include<imgui/imgui.h>

#include<stdexcept>
#include <cstdio>


void CreateConsole()
{
	AllocConsole();

	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);
	freopen_s(&fp, "CONIN$", "r", stdin);

	SetConsoleTitle(L"DX11 Debug Console");
}

using namespace zRender;

void PopulateCubeMesh(MeshCPU& mesh);
MeshCPU GenerateSphere(MeshCPU& mesh, float radius, uint32_t slices, uint32_t stacks);

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow) {
	CreateConsole();

	Window window{};
	D3D11Device graphicsDevice{};
	Renderer renderer{};
	FreelookCamera freeCamera{};
	ObjectPanel objectPanel{};

	window.Init();
	graphicsDevice.Initialize(window.GetHWND());

	D3D11ResourceProvider resourceProvider(&graphicsDevice);
	D3D11RenderContext renderContext(graphicsDevice.GetDeviceContext(), graphicsDevice.GetSwapChain(), &resourceProvider);
	renderContext.SetRenderTargetView(graphicsDevice.CreateRenderTarget());

	TextureLoader textureLoader;
	ShaderLoader shaderLoader;

	MeshCPU cubeMesh;
	MeshCPU sphereMesh;
	PopulateCubeMesh(cubeMesh);
	GenerateSphere(sphereMesh, 0.5f, 32, 32);

	ShaderCPU shader;
	shader.inputLayout = InputLayout_PNTT;
	ShaderCPU skyboxShader;
	skyboxShader.inputLayout = InputLayout_PNTT;
	shaderLoader.Load(shader, "Assets/Shaders/pbr.hlsl");
	shaderLoader.Load(skyboxShader, "Assets/Shaders/skybox.hlsl");
	ShaderHandle shaderH = resourceProvider.LoadShader(shader);
	ShaderHandle skyboxShaderH = resourceProvider.LoadShader(skyboxShader);

	textureLoader.FlipImage();
	TextureCPU texture;
	TextureCPU normalMap;
	textureLoader.Load(texture, "Assets/sand.png");
	textureLoader.Load(normalMap, "Assets/sand_normal.png");

	TextureCPU skyboxTextures[6]{};
	textureLoader.Load(skyboxTextures[0], "Assets/Skybox/Sides/px.jpg");
	textureLoader.Load(skyboxTextures[1], "Assets/Skybox/Sides/nx.jpg");
	textureLoader.Load(skyboxTextures[2], "Assets/Skybox/Sides/py.jpg");
	textureLoader.Load(skyboxTextures[3], "Assets/Skybox/Sides/ny.jpg");
	textureLoader.Load(skyboxTextures[4], "Assets/Skybox/Sides/pz.jpg");
	textureLoader.Load(skyboxTextures[5], "Assets/Skybox/Sides/nz.jpg");
	TextureHandle skyboxHandle = resourceProvider.LoadTextureCubeMap(skyboxTextures);

	renderer.Setup(&renderContext, &resourceProvider);
	renderer.SetSkybox(skyboxHandle);
	renderer.SetPipelineShader(zRender::PipelineStateType_PBR_Opaque, shaderH);
	renderer.SetPipelineShader(zRender::PipelineStateType_Skybox, skyboxShaderH);

	UI::Setup(window.GetHWND(), graphicsDevice.GetDevice(), graphicsDevice.GetDeviceContext());

	RenderItem sphere{};
	sphere.meshHandle = resourceProvider.LoadMesh(sphereMesh);
	sphere.shaderHandle = shaderH;
	sphere.modelMatrix = DirectX::XMMatrixTranslation(0, 0, 0);
	sphere.flags = RenderFlag_CastShadows | RenderFlag_ReceiveShadows;
	sphere.material.diffuseColor = { 1, 0, 0, 1 };
	sphere.material.metallic = 0;
	sphere.material.roughness = 0.1;

	RenderItem plane{};
	plane.meshHandle = resourceProvider.LoadMesh(cubeMesh);
	plane.shaderHandle = shaderH;
	plane.modelMatrix = DirectX::XMMatrixScaling(5, 0.1f, 5) * DirectX::XMMatrixTranslation(0, -0.6f, 0);
	plane.flags = RenderFlag_CastShadows | RenderFlag_ReceiveShadows;
	plane.material.diffuseColor = { 1, 1, 1, 1 };
	plane.material.metallic = 0;
	plane.material.roughness = 0;
	plane.material.diffuseTexHandle = resourceProvider.LoadTexture(texture);
	plane.material.normalTexHandle = resourceProvider.LoadTexture(normalMap);

	while (!window.ShouldClose()) {
		window.Process();

		UI::NewFrame();

		sphere.material.diffuseColor = objectPanel.GetColor();
		sphere.material.roughness = objectPanel.GetRoughness();
		sphere.material.metallic = objectPanel.GetMetallic();

		renderer.Queue(sphere);
		renderer.Queue(plane);

		freeCamera.Update();
		renderer.SetCamera(freeCamera.GetCamera());

		renderer.InitRender();
		renderer.Render();
		renderer.EndRender();

		objectPanel.Draw();

		UI::Render();

		renderContext.EndFrame();
	}

	renderer.Shutdown();
	UI::CleanUp();
	window.CleanUp();

	return 0;
}

void PopulateCubeMesh(MeshCPU& mesh) {
	mesh.vertices.reserve(36);
	mesh.indices.reserve(36);

	// Front face (+Z)
	{
		vec3 n = { 0, 0, 1 };
		vec3 t = { 1, 0, 0 };

		mesh.vertices.push_back({ {-0.5f,-0.5f, 0.5f}, n, t, {0,0} });
		mesh.vertices.push_back({ { 0.5f,-0.5f, 0.5f}, n, t, {1,0} });
		mesh.vertices.push_back({ { 0.5f, 0.5f, 0.5f}, n, t, {1,1} });
		mesh.vertices.push_back({ {-0.5f,-0.5f, 0.5f}, n, t, {0,0} });
		mesh.vertices.push_back({ { 0.5f, 0.5f, 0.5f}, n, t, {1,1} });
		mesh.vertices.push_back({ {-0.5f, 0.5f, 0.5f}, n, t, {0,1} });
	}

	// Back face (-Z)
	{
		vec3 n = { 0, 0, -1 };
		vec3 t = { -1, 0, 0 };

		mesh.vertices.push_back({ { 0.5f,-0.5f,-0.5f}, n, t, {0,0} });
		mesh.vertices.push_back({ {-0.5f,-0.5f,-0.5f}, n, t, {1,0} });
		mesh.vertices.push_back({ {-0.5f, 0.5f,-0.5f}, n, t, {1,1} });
		mesh.vertices.push_back({ { 0.5f,-0.5f,-0.5f}, n, t, {0,0} });
		mesh.vertices.push_back({ {-0.5f, 0.5f,-0.5f}, n, t, {1,1} });
		mesh.vertices.push_back({ { 0.5f, 0.5f,-0.5f}, n, t, {0,1} });
	}

	// Right face (+X)
	{
		vec3 n = { 1, 0, 0 };
		vec3 t = { 0, 0, -1 };

		mesh.vertices.push_back({ { 0.5f,-0.5f, 0.5f}, n, t, {0,0} });
		mesh.vertices.push_back({ { 0.5f,-0.5f,-0.5f}, n, t, {1,0} });
		mesh.vertices.push_back({ { 0.5f, 0.5f,-0.5f}, n, t, {1,1} });
		mesh.vertices.push_back({ { 0.5f,-0.5f, 0.5f}, n, t, {0,0} });
		mesh.vertices.push_back({ { 0.5f, 0.5f,-0.5f}, n, t, {1,1} });
		mesh.vertices.push_back({ { 0.5f, 0.5f, 0.5f}, n, t, {0,1} });
	}

	// Left face (-X)
	{
		vec3 n = { -1, 0, 0 };
		vec3 t = { 0, 0, 1 };

		mesh.vertices.push_back({ {-0.5f,-0.5f,-0.5f}, n, t, {0,0} });
		mesh.vertices.push_back({ {-0.5f,-0.5f, 0.5f}, n, t, {1,0} });
		mesh.vertices.push_back({ {-0.5f, 0.5f, 0.5f}, n, t, {1,1} });
		mesh.vertices.push_back({ {-0.5f,-0.5f,-0.5f}, n, t, {0,0} });
		mesh.vertices.push_back({ {-0.5f, 0.5f, 0.5f}, n, t, {1,1} });
		mesh.vertices.push_back({ {-0.5f, 0.5f,-0.5f}, n, t, {0,1} });
	}

	// Top face (+Y)
	{
		vec3 n = { 0, 1, 0 };
		vec3 t = { 1, 0, 0 };

		mesh.vertices.push_back({ {-0.5f, 0.5f, 0.5f}, n, t, {0,0} });
		mesh.vertices.push_back({ { 0.5f, 0.5f, 0.5f}, n, t, {1,0} });
		mesh.vertices.push_back({ { 0.5f, 0.5f,-0.5f}, n, t, {1,1} });
		mesh.vertices.push_back({ {-0.5f, 0.5f, 0.5f}, n, t, {0,0} });
		mesh.vertices.push_back({ { 0.5f, 0.5f,-0.5f}, n, t, {1,1} });
		mesh.vertices.push_back({ {-0.5f, 0.5f,-0.5f}, n, t, {0,1} });
	}

	// Bottom face (-Y)
	{
		vec3 n = { 0, -1, 0 };
		vec3 t = { 1, 0, 0 };

		mesh.vertices.push_back({ {-0.5f,-0.5f,-0.5f}, n, t, {0,0} });
		mesh.vertices.push_back({ { 0.5f,-0.5f,-0.5f}, n, t, {1,0} });
		mesh.vertices.push_back({ { 0.5f,-0.5f, 0.5f}, n, t, {1,1} });
		mesh.vertices.push_back({ {-0.5f,-0.5f,-0.5f}, n, t, {0,0} });
		mesh.vertices.push_back({ { 0.5f,-0.5f, 0.5f}, n, t, {1,1} });
		mesh.vertices.push_back({ {-0.5f,-0.5f, 0.5f}, n, t, {0,1} });
	}

	// Indices (unchanged)
	for (uint32_t i = 0; i < 36; ++i)
		mesh.indices.push_back(i);

	mesh.indexCount = 36;
}

constexpr float PI = 3.14159265359f;

MeshCPU GenerateSphere(MeshCPU& mesh, float radius, uint32_t slices, uint32_t stacks)
{
	// Vertices
	for (uint32_t stack = 0; stack <= stacks; ++stack)
	{
		float v = (float)stack / stacks;
		float phi = v * PI; // 0 -> PI

		float y = std::cos(phi);
		float r = std::sin(phi);

		for (uint32_t slice = 0; slice <= slices; ++slice)
		{
			float u = (float)slice / slices;
			float theta = u * 2.0f * PI; // 0 -> 2PI

			float x = r * std::cos(theta);
			float z = r * std::sin(theta);

			Vertex vertex;

			// Position
			vertex.position = vec3{
				x * radius,
				y * radius,
				z * radius
			};

			// Normal
			vertex.normal = vec3{ x, y, z };

			// Tangent (direction of increasing U / theta)
			vertex.tangent = vec3{
				-std::sin(theta),
				0.0f,
				std::cos(theta)
			};

			// UV
			vertex.uv = vec2{ u, 1.0f - v };

			mesh.vertices.push_back(vertex);
		}
	}

	// Indices
	uint32_t vertsPerRow = slices + 1;

	for (uint32_t stack = 0; stack < stacks; ++stack)
	{
		for (uint32_t slice = 0; slice < slices; ++slice)
		{
			uint32_t i0 = stack * vertsPerRow + slice;
			uint32_t i1 = i0 + 1;
			uint32_t i2 = i0 + vertsPerRow;
			uint32_t i3 = i2 + 1;

			// Triangle 1
			mesh.indices.push_back(i0);
			mesh.indices.push_back(i1);
			mesh.indices.push_back(i2);

			// Triangle 2
			mesh.indices.push_back(i1);
			mesh.indices.push_back(i3);
			mesh.indices.push_back(i2);
		}
	}

	mesh.indexCount = static_cast<uint64_t>(mesh.indices.size());
	return mesh;
}