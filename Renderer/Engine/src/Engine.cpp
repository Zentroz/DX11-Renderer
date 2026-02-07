#include"Engine.h"

// RenderPasses
#include"RenderPasses/GBufferPass.h"
#include"RenderPasses/LightingPass.h"
#include"RenderPasses/GradiantPass.h"
#include"RenderPasses/PresentPass.h"

using namespace zRender;

struct MatData {
	vec4 baseColor;
	float roughness;
	float metallic;
	vec2 padding;
};

void Engine::Init(const EngineInitData& initData) {
	graphicsDevice.Initialize(initData.hWnd);

	resourceProvider = std::make_unique<D3D11ResourceProvider>(&graphicsDevice);
	renderContext = std::make_unique<D3D11RenderContext>(graphicsDevice.GetDeviceContext(), graphicsDevice.GetSwapChain(), resourceProvider.get());

	renderer.Setup(renderContext.get(), resourceProvider.get());
	renderer.AddLight(
		{
			.type = { 0, 0, 0, 0 },
			.position = vec4(0, 4, 0, 1),
			.direction = normalizeVec(vec4(-1, -1, 0)),
			.lightColor = vec4(1, 1, 1, 1),
			.lightProp = vec4(2, 0, 0, 0)
		}
	);

	CreatePipelines();
	CreateRenderPasses(initData.width, initData.height);
	
	UI::Setup(initData.hWnd, graphicsDevice.GetDevice(), graphicsDevice.GetDeviceContext());

	freeCamera.GetCamera().width = initData.width;
	freeCamera.GetCamera().height = initData.height;

	renderContext->SetScreenSize(initData.width, initData.height);

	OpenModelFile("Assets/Mesh/sponza/scene.gltf");
}

void Engine::CleanUp() {
	renderer.Shutdown();

	UI::CleanUp();

	renderContext.release();
	resourceProvider.release();

	graphicsDevice.Release();
}

void Engine::QueueResize(int newWidth, int newHeight, bool fullscreen) {
	this->newWidth = newWidth;
	this->newHeight = newHeight;
	this->fullscreen = fullscreen;
	resize = true;
}

void Engine::Resize(int newWidth, int newHeight, bool fullscreen) {
	resourceProvider->ReleaseScreenTexture();
	graphicsDevice.Resize(newWidth, newHeight, fullscreen);

	resourceProvider->RecreateScreenTextureHandle();
	//CreateRenderPasses(newWidth, newHeight);
	renderContext->SetScreenSize(newWidth, newHeight);

	freeCamera.GetCamera().width = newWidth;
	freeCamera.GetCamera().height = newHeight;

	resize = false;
}

void Engine::Run() {
	if (resize) Resize(newWidth, newHeight, fullscreen);

	PresentPass* presentPass = static_cast<PresentPass*>(renderer.GetRenderPass("PresentPass"));

	UI::NewFrame();

	objectPanel.Draw(scene.GetAssetManager().GetAllMaterials());
	presentPass->SetOutputTextureIndex(objectPanel.GetSelectedRenderOutput());

	for (auto& item : scene.GenerateDrawCalls()) {
		renderer.Queue(item);
	}

	freeCamera.Update();
	renderer.SetCamera(freeCamera.GetCamera());

	renderer.InitRender();
	renderer.Render();
	renderer.EndRender();

	UI::Render();

	renderContext->EndFrame();
}

void Engine::OpenModelFile(const std::string& path) {
	ModelLoader modelLoader;
	ModelAsset loadedModel;

	if (modelLoader.Load(loadedModel, path)) {
		Model model;
		model.meshHandle = resourceProvider->LoadMesh(*loadedModel.mesh);

		for (auto m : loadedModel.materials) {
			for (uint32_t subMeshIndex : m.subMeshIndices) {
				Material mat;
				mat.name = loadedModel.mesh->subMeshes[subMeshIndex].name;
				mat.baseColor = m.baseColor;
				mat.roughnessFactor = m.roughnessFactor;
				mat.metallicFactor = m.metallicFactor;
				mat.aplhaCutoff = m.aplhaCutoff;
				mat.renderMode = (Material::RenderMode)m.renderMode;

				if (loadedModel.textures.contains(m.albedoTextureName)) {
					TextureCPU* tex = loadedModel.textures[m.albedoTextureName];
					tex->usageFlags = (uint32_t)TextureUsageFlags::TextureUsageFlag_ShaderResource;
					mat.albedo = resourceProvider->LoadTexture(*tex);
				}
				if (loadedModel.textures.contains(m.normalTextureName)) {
					TextureCPU* tex = loadedModel.textures[m.normalTextureName];
					tex->usageFlags = (uint32_t)TextureUsageFlags::TextureUsageFlag_ShaderResource;
					mat.normal = resourceProvider->LoadTexture(*tex);
				}
				if (loadedModel.textures.contains(m.rmTextureName)) {
					TextureCPU* tex = loadedModel.textures[m.rmTextureName];
					tex->usageFlags = (uint32_t)TextureUsageFlags::TextureUsageFlag_ShaderResource;
					mat.orm = resourceProvider->LoadTexture(*tex);
				}

				Model::SubMesh sub;
				sub.submeshIndex = subMeshIndex;
				sub.localModelMatrix = loadedModel.mesh->subMeshes[subMeshIndex].localModel;
				sub.materialHandle = scene.GetAssetManager().AddMaterial(mat);

				model.subMeshes.push_back(sub);
			}
		}

		Entity loadedMesh;
		loadedMesh.model = scene.GetAssetManager().AddModel(model);
		loadedMesh.modelMatrix = loadedModel.modelMatrix;
		loadedMesh.baseColor = vec4(1, 1, 1, 1);
		loadedMesh.roughness = 1;
		loadedMesh.metallic = 0;

		scene.AddEntity(loadedMesh);
	}
	else {
		printf("Failed to load model at path: %s \n", path.c_str());
	}

	loadedModel.Dispose();
}

void Engine::CreatePipelines() {
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

	ShaderCPU gradiantShader;
	gradiantShader.inputLayout = InputLayout_None;
	shaderLoader.Load(gradiantShader, "Assets/Shaders/GradiantShader.hlsl");

	ShaderCPU presentShader;
	presentShader.inputLayout = InputLayout_None;
	shaderLoader.Load(presentShader, "Assets/Shaders/PresentShader.hlsl");

	resourceProvider->AddPipelineStateContainer({
		.name = "PBROpaque",
		.shaderHandle = resourceProvider->LoadShader(pbrShader),
		.rasterizerHandle = resourceProvider->GetRasteriserHandle(RasterizerFunc_CullMode_None, RasterizerFunc_FillMode_Solid),
		.depthStencilHandle = resourceProvider->GetDepthStateHandle(DepthWriteMask_Zero, DepthFunc_Never),
		.topology = PrimitiveTopology_Triangelist
	});

	resourceProvider->AddPipelineStateContainer({
		.name = "GBufferDebug",
		.shaderHandle = resourceProvider->LoadShader(gDebugShader),
		.rasterizerHandle = resourceProvider->GetRasteriserHandle(RasterizerFunc_CullMode_None, RasterizerFunc_FillMode_Solid),
		.depthStencilHandle = resourceProvider->GetDepthStateHandle(DepthWriteMask_Zero, DepthFunc_Never),
		.topology = PrimitiveTopology_Triangelist
	});

	resourceProvider->AddPipelineStateContainer({
		.name = "GBufferPass",
		.shaderHandle = resourceProvider->LoadShader(gShader),
		.rasterizerHandle = resourceProvider->GetRasteriserHandle(RasterizerFunc_CullMode_None, RasterizerFunc_FillMode_Solid),
		.depthStencilHandle = resourceProvider->GetDepthStateHandle(DepthWriteMask_Zero, DepthFunc_Never),
		.topology = PrimitiveTopology_Triangelist
	});


	resourceProvider->AddPipelineStateContainer({
		.name = "ShadowPass",
		.shaderHandle = resourceProvider->LoadShader(sShader),
		.rasterizerHandle = resourceProvider->GetRasteriserHandle(RasterizerFunc_CullMode_None, RasterizerFunc_FillMode_Solid),
		.depthStencilHandle = resourceProvider->GetDepthStateHandle(DepthWriteMask_Zero, DepthFunc_Never),
		.topology = PrimitiveTopology_Triangelist
	});

	resourceProvider->AddPipelineStateContainer({
		.name = "PresentPass",
		.shaderHandle = resourceProvider->LoadShader(presentShader),
		.rasterizerHandle = resourceProvider->GetRasteriserHandle(RasterizerFunc_CullMode_None, RasterizerFunc_FillMode_Solid),
		.depthStencilHandle = resourceProvider->GetDepthStateHandle(DepthWriteMask_Zero, DepthFunc_Never),
		.topology = PrimitiveTopology_Triangelist
	});

	resourceProvider->AddPipelineStateContainer({
		.name = "LightPass",
		.shaderHandle = resourceProvider->LoadShader(lightShader),
		.rasterizerHandle = resourceProvider->GetRasteriserHandle(RasterizerFunc_CullMode_None, RasterizerFunc_FillMode_Solid),
		.depthStencilHandle = resourceProvider->GetDepthStateHandle(DepthWriteMask_Zero, DepthFunc_Never),
		.topology = PrimitiveTopology_Triangelist,
	});

	resourceProvider->AddPipelineStateContainer({
		.name = "GradiantPass",
		.shaderHandle = resourceProvider->LoadShader(gradiantShader),
		.rasterizerHandle = resourceProvider->GetRasteriserHandle(RasterizerFunc_CullMode_None, RasterizerFunc_FillMode_Solid),
		.depthStencilHandle = resourceProvider->GetDepthStateHandle(DepthWriteMask_Zero, DepthFunc_LessEqual),
		.topology = PrimitiveTopology_Triangelist
	});
}

void Engine::CreateRenderPasses(int width, int height) {
	// Directional Light Shadow
	/*
	ShadowPass::InitData shadowInit;
	shadowInit.depthSV = resourceProvider->CreateTexture(width, height, TextureFormat_R32_Typeless, TextureUsageFlags::TextureUsageFlag_DepthStencil | TextureUsageFlags::TextureUsageFlag_ShaderResource);
	shadowInit.frameBufferHandle = frameBufferHandle;
	shadowInit.objectBufferHandle = objectBufferHandle;
	shadowInit.pipeline = resourceProvider->GetPipelineStateContainer("ShadowPass");

	ShadowPass* shadow = new ShadowPass(shadowInit);
	renderGraph.AddPass(shadow);
	*/

	StaticData sData{
		.mainLightDirection = { 0.25, 0.5f, -0.25f, 1 },
		.mainLightColor = { 1, 1, 1, 1 }
	};

	vec3 cPos = { 0, 1, -5 };

	FrameData fData{
		.vpMatrix = DirectX::XMMatrixIdentity(),
		.cameraPosition = { cPos.x, cPos.y, cPos.z, 1 }
	};

	ObjectData oData;
	oData.modelMatrix = DirectX::XMMatrixIdentity();

	MaterialData mData{
		.diffuseColor = { 0, 0, 0 },
		.roughness = 0,
		.metallic = 0
	};

	SkyboxData sbData{
		sbData.view = DirectX::XMMatrixIdentity(),
		sbData.proj = DirectX::XMMatrixIdentity()
	};

	BufferHandle staticBufferHandle = resourceProvider->CreateBuffer(Buffer_Uasge_Immutable, 0, sizeof(StaticData), &sData);
	BufferHandle frameBufferHandle = resourceProvider->CreateBuffer(Buffer_Uasge_Default, 0, sizeof(FrameData), &fData);
	BufferHandle objectBufferHandle = resourceProvider->CreateBuffer(Buffer_Uasge_Dynamic, Buffer_CPU_Write, sizeof(ObjectData), &oData);
	BufferHandle materialBufferHandle = resourceProvider->CreateBuffer(Buffer_Uasge_Dynamic, Buffer_CPU_Write, sizeof(MaterialData), &mData);
	BufferHandle lightBufferHandle = resourceProvider->CreateBuffer(Buffer_Uasge_Dynamic, Buffer_CPU_Write, sizeof(LightData), &mData);

	GBuffer gData = {
		.albedoRT = resourceProvider->CreateTexture(width, height, TextureFormat_RGBA8_UNorm, TextureUsageFlags::TextureUsageFlag_RenderTarget | TextureUsageFlags::TextureUsageFlag_ShaderResource, TextureFilter::Linear),
		.normalRT = resourceProvider->CreateTexture(width, height, TextureFormat_RGBA16F, TextureUsageFlags::TextureUsageFlag_RenderTarget | TextureUsageFlags::TextureUsageFlag_ShaderResource, TextureFilter::Point),
		.materialRT = resourceProvider->CreateTexture(width, height, TextureFormat_RGBA8_UNorm, TextureUsageFlags::TextureUsageFlag_RenderTarget | TextureUsageFlags::TextureUsageFlag_ShaderResource, TextureFilter::Point),
		.depthRT = resourceProvider->CreateTexture(width, height, TextureFormat_R32_Typeless, TextureUsageFlags::TextureUsageFlag_DepthStencil | TextureUsageFlags::TextureUsageFlag_ShaderResource, TextureFilter::Point),
		.whiteTextureHandle = resourceProvider->CreateTexture(128, 128, TextureUsageFlags::TextureUsageFlag_ShaderResource, TextureFilter::Linear, vec4(1, 1, 1, 1)),
		.staticBufferHandle = staticBufferHandle,
		.frameBufferHandle = frameBufferHandle,
		.objectBufferHandle = objectBufferHandle,
		.materialBufferHandle = materialBufferHandle,
		.pipelineStateHandles = resourceProvider->GetPipelineStateContainer("GBufferPass")
	};
	renderer.AddRenderPass(new GBufferPass(gData));

	LightingPass::MatricesBufferData lpmData;
	lpmData.invViewProj = DirectX::XMMatrixIdentity();

	LightingPass::InitData lightPassData{
		.albedoRT = gData.albedoRT,
		.normalRT = gData.normalRT,
		.materialRT = gData.materialRT,
		.depthRT = gData.depthRT,
		.shadowDSV = {},
		.outputRT = resourceProvider->CreateTexture(width, height, TextureFormat_RGBA8_UNorm, TextureUsageFlags::TextureUsageFlag_RenderTarget | TextureUsageFlags::TextureUsageFlag_ShaderResource, TextureFilter::Linear),
		.lightBufferHandle = lightBufferHandle,
		.matricesBufferHandle = resourceProvider->CreateBuffer(Buffer_Uasge_Default, 0, sizeof(LightingPass::MatricesBufferData), &lpmData),
		.pipeline = resourceProvider->GetPipelineStateContainer("LightPass")
	};
	renderer.AddRenderPass(new LightingPass(lightPassData));

	PresentPass::OuputConstantBuffer presentPassOuputBufferData{
		.outputTextureIndex = 0,
		.paddings = { 0, 0, 0 }
	};

	GradiantPassInput gradiantPassInput{
		.outputTextureHandle = resourceProvider->CreateTexture(width, height, TextureFormat_RGBA8_UNorm, TextureUsageFlags::TextureUsageFlag_RenderTarget | TextureUsageFlags::TextureUsageFlag_ShaderResource, TextureFilter::Linear),
		.pipeline = resourceProvider->GetPipelineStateContainer("GradiantPass")
	};
	renderer.AddRenderPass(new GradiantPass(gradiantPassInput));

	PresentPassInput presentPassInput{
		.presentTextureHandle = lightPassData.outputRT,
		.albedoTextureHandle = gData.albedoRT,
		.normalTextureHandle = gData.normalRT,
		.materialTextureHandle = gData.materialRT,
		.depthTextureHandle = gData.depthRT,
		.backgroundTextureHandle = gradiantPassInput.outputTextureHandle,
		.screenTextureHandle = resourceProvider->GetScreenTextureHandle(),
		.outputTextureBufferHandle = resourceProvider->CreateBuffer(Buffer_Uasge_Default, 0, sizeof(PresentPass::OuputConstantBuffer), &presentPassOuputBufferData),
		.pipeline = resourceProvider->GetPipelineStateContainer("PresentPass")
	};
	renderer.AddRenderPass(new PresentPass(presentPassInput));
}

void Engine::RebuildRenderPass(int width, int height) {
	GBufferPass* gPass = static_cast<GBufferPass*>(renderer.GetRenderPass("G-Buffer_Pass"));
	LightingPass* lightPass = static_cast<LightingPass*>(renderer.GetRenderPass("LightingPass"));
	GradiantPass* gradiantPass = static_cast<GradiantPass*>(renderer.GetRenderPass("GradiantPass"));
	PresentPass* presentPass = static_cast<PresentPass*>(renderer.GetRenderPass("PresentPass"));
}