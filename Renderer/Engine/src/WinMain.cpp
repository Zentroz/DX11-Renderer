#include"Window/Window.h"
#include"Utility/FreelookCamera.h"
#include"Utility/AssetLoader.h"
#include"Editor/UI.h"
#include"Editor/ObjectPanel.h"
#include"Engine/Scene.h"
#include"Engine/AssetManager.h"

#include<Renderer/D3D11/GraphicsDevice.h>
#include<Renderer/D3D11/RenderContext.h>
#include<Renderer/D3D11/ResourceProvider.h>
#include<Renderer/Renderer.h>

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
void GenerateSphere(MeshCPU& mesh, float radius, uint32_t slices, uint32_t stacks);

void CreatePipelines(std::vector<PipelineStateContainer>& pipelineStateContainers, D3D11ResourceProvider& p) {
	ShaderLoader shaderLoader;

	ShaderCPU gDebugShader;
	gDebugShader.inputLayout = InputLayout_None;
	shaderLoader.Load(gDebugShader, "Assets/Shaders/GeometryDebugShader.hlsl");

	ShaderCPU gShader;
	gShader.inputLayout = InputLayout_PNTT;
	shaderLoader.Load(gShader, "Assets/Shaders/GeometryShader.hlsl");

	ShaderCPU pbrShader;
	pbrShader.inputLayout = InputLayout_PNTT;
	shaderLoader.Load(pbrShader, "Assets/Shaders/pbr.hlsl");

	ShaderCPU sShader;
	sShader.inputLayout = InputLayout_PNTT;
	shaderLoader.Load(sShader, "Assets/Shaders/ShadowShader.hlsl");

	ShaderCPU lightShader;
	lightShader.inputLayout = InputLayout_None;
	shaderLoader.Load(lightShader, "Assets/Shaders/LightingShader.hlsl");

	ShaderCPU presentShader;
	presentShader.inputLayout = InputLayout_None;
	shaderLoader.Load(presentShader, "Assets/Shaders/PresentShader.hlsl");

	PipelineStateContainer gBufferDebugPass;
	gBufferDebugPass.name = "GBufferDebug";
	gBufferDebugPass.depthStencilHandle = p.GetDepthStateHandle(DepthWriteMask_Zero, DepthFunc_Never);
	gBufferDebugPass.rasterizerHandle = p.GetRasteriserHandle(RasterizerFunc_CullMode_None, RasterizerFunc_FillMode_Solid);
	gBufferDebugPass.topology = PrimitiveTopology_Triangelist;
	gBufferDebugPass.shaderHandle = p.LoadShader(gDebugShader);
	pipelineStateContainers.push_back(gBufferDebugPass);

	PipelineStateContainer gBufferPass;
	gBufferPass.name = "GBufferPass";
	gBufferPass.depthStencilHandle = p.GetDepthStateHandle(DepthWriteMask_Zero, DepthFunc_Never);
	gBufferPass.rasterizerHandle = p.GetRasteriserHandle(RasterizerFunc_CullMode_None, RasterizerFunc_FillMode_Solid);
	gBufferPass.topology = PrimitiveTopology_Triangelist;
	gBufferPass.shaderHandle = p.LoadShader(gShader);
	pipelineStateContainers.push_back(gBufferPass);

	PipelineStateContainer pbrOpaque;
	pbrOpaque.name = "PBROpaque";
	pbrOpaque.depthStencilHandle = p.GetDepthStateHandle(DepthWriteMask_Zero, DepthFunc_Never);
	pbrOpaque.rasterizerHandle = p.GetRasteriserHandle(RasterizerFunc_CullMode_None, RasterizerFunc_FillMode_Solid);
	pbrOpaque.topology = PrimitiveTopology_Triangelist;
	pbrOpaque.shaderHandle = p.LoadShader(pbrShader);
	pipelineStateContainers.push_back(pbrOpaque);

	PipelineStateContainer shadowPass;
	shadowPass.name = "ShadowPass";
	shadowPass.depthStencilHandle = p.GetDepthStateHandle(DepthWriteMask_Zero, DepthFunc_Never);
	shadowPass.rasterizerHandle = p.GetRasteriserHandle(RasterizerFunc_CullMode_None, RasterizerFunc_FillMode_Solid);
	shadowPass.topology = PrimitiveTopology_Triangelist;
	shadowPass.shaderHandle = p.LoadShader(sShader);
	pipelineStateContainers.push_back(shadowPass);

	PipelineStateContainer presentPass;
	presentPass.name = "PresentPass";
	presentPass.depthStencilHandle = p.GetDepthStateHandle(DepthWriteMask_Zero, DepthFunc_Never);
	presentPass.rasterizerHandle = p.GetRasteriserHandle(RasterizerFunc_CullMode_None, RasterizerFunc_FillMode_Solid);
	presentPass.topology = PrimitiveTopology_Triangelist;
	presentPass.shaderHandle = p.LoadShader(presentShader);
	pipelineStateContainers.push_back(presentPass);

	PipelineStateContainer lightPass;
	lightPass.name = "LightPass";
	lightPass.depthStencilHandle = p.GetDepthStateHandle(DepthWriteMask_Zero, DepthFunc_Never);
	lightPass.rasterizerHandle = p.GetRasteriserHandle(RasterizerFunc_CullMode_None, RasterizerFunc_FillMode_Solid);
	lightPass.topology = PrimitiveTopology_Triangelist;
	lightPass.shaderHandle = p.LoadShader(lightShader);
	pipelineStateContainers.push_back(lightPass);
}

struct MatData {
	vec4 baseColor;
	float roughness;
	float metallic;
	vec2 padding;
};

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow) {
	CreateConsole();

	//MeshCPU cubeTest;
	//ImportMesh("Assets/cube.fbx", cubeTest);

	Window window{};
	D3D11Device graphicsDevice{};
	Renderer renderer{};
	FreelookCamera freeCamera{};
	ObjectPanel objectPanel{};
	Scene scene{};

	window.Init();
	graphicsDevice.Initialize(window.GetHWND());

	D3D11ResourceProvider resourceProvider(&graphicsDevice);
	D3D11RenderContext renderContext(graphicsDevice.GetDeviceContext(), graphicsDevice.GetSwapChain(), &resourceProvider);

	std::vector<PipelineStateContainer> pipelineStateContainers;
	CreatePipelines(pipelineStateContainers, resourceProvider);
	for (auto& state : pipelineStateContainers) {
		resourceProvider.AddPipelineStateContainer(state);
	}

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
	texture.usageFlags = static_cast<uint32_t>(TextureUsageFlags::TextureUsageFlag_ShaderResource);
	TextureCPU normalMap;
	normalMap.usageFlags = static_cast<uint32_t>(TextureUsageFlags::TextureUsageFlag_ShaderResource);
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

	ModelLoader modelLoader;
	ModelAsset casaModel;
	modelLoader.Load(casaModel, "Assets/Mesh/casa.fbx");

	Model model;
	model.meshHandle = resourceProvider.LoadMesh(*casaModel.mesh);

	for (auto m : casaModel.materials) {
		for (uint32_t subMeshIndex : m.subMeshIndices) {
			Material mat;
			mat.name = casaModel.mesh->subMeshes[subMeshIndex].name;
			mat.baseColor = m.baseColor;
			mat.roughness = m.roughness;
			mat.metallic = m.metallic;

			Model::SubMesh sub;
			sub.submeshIndex = subMeshIndex;
			sub.localModelMatrix = casaModel.mesh->subMeshes[subMeshIndex].localModel;
			sub.materialHandle = scene.GetAssetManager().AddMaterial(mat);

			model.subMeshes.push_back(sub);
		}
	}

	Entity casa;
	casa.model = scene.GetAssetManager().AddModel(model);
	casa.modelMatrix = casaModel.modelMatrix;
	casa.baseColor = vec4(1, 1, 1, 1);
	casa.roughness = 1;
	casa.metallic = 0;

	scene.AddEntity(casa);

	while (!window.ShouldClose()) {
		window.Process();

		UI::NewFrame();

		objectPanel.Draw(scene.GetAssetManager().GetAllMaterials());
		
		for (auto& item : scene.GenerateDrawCalls()) {
			renderer.Queue(item);
		}

		freeCamera.Update();
		renderer.SetCamera(freeCamera.GetCamera());

		renderer.InitRender();
		renderer.Render();
		renderer.EndRender();

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

	SubMesh sub;
	sub.vertexOffset = 0;
	sub.vertexCount = static_cast<uint64_t>(mesh.vertices.size());
	sub.indexOffset = 0;
	sub.indexCount = static_cast<uint64_t>(mesh.indices.size());

	mesh.subMeshes.push_back(sub);
}

constexpr float PI = 3.14159265359f;

void GenerateSphere(MeshCPU& mesh, float radius, uint32_t slices, uint32_t stacks)
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

	SubMesh sub;
	sub.vertexOffset = 0;
	sub.vertexCount = static_cast<uint64_t>(mesh.vertices.size());
	sub.indexOffset = 0;
	sub.indexCount = static_cast<uint64_t>(mesh.indices.size());

	mesh.subMeshes.push_back(sub);
}