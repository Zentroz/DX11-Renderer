#include"Engine.h"
#include"Engine/Components.h"
#include"Physics/Intersection.h"

// RenderPasses
#include"RenderPasses/GBufferPass.h"
#include"RenderPasses/ShadowPass.h"
#include"RenderPasses/LightingPass.h"
#include"RenderPasses/GradiantPass.h"
#include"RenderPasses/PresentPass.h"

using namespace zRender;

void Engine::Init(const EngineInitData& initData) {
	graphicsDevice = std::make_unique<D3D11Device>();
	graphicsDevice->Initialize(initData.hWnd);

	resourceProvider = std::make_unique<D3D11ResourceProvider>(graphicsDevice.get());
	renderContext = std::make_unique<D3D11RenderContext>(graphicsDevice->GetDeviceContext(), graphicsDevice->GetSwapChain(), resourceProvider.get());

	renderer.Setup(renderContext.get(), resourceProvider.get());

	Camera cam{};
	cam.up = vec3(0, 1, 0);
	cam.forward = normalize(vec3(0, -2, -1));
	cam.position = vec3(0, 0, 0) - cam.forward * 50;
	cam.width = 4096;
	cam.height = 4096;
	cam.renderMode = Camera::Orthographic;

	DirectX::XMMATRIX lightVP = cam.ViewProjMatrix();

	renderer.AddLight(
		{
			.type = { 0, 0, 0, 0 },
			.position = vec4(cam.position.x, cam.position.y, cam.position.z, 1),
			.direction = vec4(cam.forward.x, cam.forward.y, cam.forward.z, 1),
			.lightColor = vec4(0.77, 0.92, 0.85, 1),
			.lightProp = vec4(1, 0, 0, 0),
			.VPMatrix = lightVP,
			.invVPMatrix = DirectX::XMMatrixInverse(nullptr, lightVP)
		}
	);

	CreatePipelines();
	CreateRenderPasses(initData.width, initData.height);
	
	UI::Setup(initData.hWnd, graphicsDevice->GetDevice(), graphicsDevice->GetDeviceContext());

	freeCamera.GetCamera().width = initData.width;
	freeCamera.GetCamera().height = initData.height;

	renderContext->SetScreenSize(initData.width, initData.height);
	renderer.SetScreenSize(initData.width, initData.height);

	OpenModelFile("Assets/Mesh/sponza/scene.gltf");
}

void Engine::CleanUp() {
	renderer.Shutdown();

	UI::CleanUp();

	renderContext.release();
	resourceProvider.release();

	graphicsDevice->Release();
}

void Engine::QueueResize(int newWidth, int newHeight, bool fullscreen) {
	this->newWidth = newWidth;
	this->newHeight = newHeight;
	this->fullscreen = fullscreen;
	resize = true;
}

void Engine::Resize(int newWidth, int newHeight, bool fullscreen) {
	resourceProvider->ReleaseScreenTexture();
	graphicsDevice->Resize(newWidth, newHeight, fullscreen);

	resourceProvider->RecreateScreenTextureHandle();
	renderContext->SetScreenSize(newWidth, newHeight);

	freeCamera.GetCamera().width = newWidth;
	freeCamera.GetCamera().height = newHeight;

	renderer.SetScreenSize(newWidth, newHeight);

	resize = false;
}

void Engine::Run() {
	if (resize) Resize(newWidth, newHeight, fullscreen);

	PresentPass* presentPass = static_cast<PresentPass*>(renderer.GetRenderPass("PresentPass"));

	UI::NewFrame();

	freeCamera.Update();

	if (ImGui::GetIO().MouseDown[1]) {
		Intersect(intersectedEntity);
	}

	objectPanel.Draw(intersectedEntity, scene.GetRegistry());
	presentPass->SetOutputTextureIndex(objectPanel.GetSelectedRenderOutput());

	for (auto& item : scene.GenerateDrawCalls()) {
		renderer.Queue(item);
	}

	renderer.SetCamera(freeCamera.GetCamera());

	renderer.InitRender();
	renderer.Render();
	renderer.EndRender();

	UI::Render();
	renderContext->EndFrame();
}

void Engine::OpenModelFile(const std::string& path) {
	entt::registry& registry = scene.GetRegistry();

	ModelLoader modelLoader;
	ModelAsset loadedModel;

	if (modelLoader.Load(loadedModel, path)) {
		Model model;
		model.meshHandle = resourceProvider->LoadMesh(*loadedModel.mesh);

		for (auto m : loadedModel.materials) {
			for (uint32_t subMeshIndex : m.subMeshIndices) {
				auto e = registry.create();

				DirectX::XMVECTOR scale;
				DirectX::XMVECTOR rotation;
				DirectX::XMVECTOR position;
				DirectX::XMMatrixDecompose(&scale, &rotation, &position, loadedModel.mesh->subMeshes[subMeshIndex].localModel);

				registry.emplace<TransformComponent>(e,
					vec3(DirectX::XMVectorGetX(position), DirectX::XMVectorGetY(position), DirectX::XMVectorGetZ(position)),
					vec4(DirectX::XMVectorGetX(rotation), DirectX::XMVectorGetY(rotation), DirectX::XMVectorGetZ(rotation), DirectX::XMVectorGetW(rotation)),
					vec3(DirectX::XMVectorGetX(scale), DirectX::XMVectorGetY(scale), DirectX::XMVectorGetZ(scale))
				);

				registry.emplace<MeshFilter>(e, model.meshHandle, subMeshIndex);

				MaterialComponent material{
					.baseColor = m.baseColor,
					.roughness = m.roughnessFactor,
					.metallic = m.metallicFactor,
					.toughness = 0.5,
					.alphaCutoff = m.aplhaCutoff,
					.surfaceType = m.renderMode == ModelAsset::Material::Transparent ? Transparent : Opaque,
					.alphaClipping = m.aplhaCutoff < 0.99999f
				};

				Material mat;
				mat.name = loadedModel.mesh->subMeshes[subMeshIndex].name;
				mat.baseColor = m.baseColor;
				mat.roughnessFactor = m.roughnessFactor;
				mat.metallicFactor = m.metallicFactor;
				mat.aplhaCutoff = m.aplhaCutoff;
				mat.renderMode = (Material::RenderMode)m.renderMode;

				if (loadedModel.textures.contains(m.albedoTextureName)) {
					Texture* tex = loadedModel.textures[m.albedoTextureName];
					tex->usageFlags = (uint32_t)TextureUsageFlags::TextureUsageFlag_ShaderResource;
					mat.albedo = resourceProvider->LoadTexture(*tex);
					material.albedoTexture = mat.albedo;
				}
				if (loadedModel.textures.contains(m.normalTextureName)) {
					Texture* tex = loadedModel.textures[m.normalTextureName];
					tex->usageFlags = (uint32_t)TextureUsageFlags::TextureUsageFlag_ShaderResource;
					mat.normal = resourceProvider->LoadTexture(*tex);
					material.normalTexture = mat.normal;
				}
				if (loadedModel.textures.contains(m.rmTextureName)) {
					Texture* tex = loadedModel.textures[m.rmTextureName];
					tex->usageFlags = (uint32_t)TextureUsageFlags::TextureUsageFlag_ShaderResource;
					mat.orm = resourceProvider->LoadTexture(*tex);
					material.ormTexture = mat.orm;
				}
				registry.emplace<MaterialComponent>(e, material);

				Model::SubMesh sub;
				sub.submeshIndex = subMeshIndex;
				sub.localModelMatrix = loadedModel.mesh->subMeshes[subMeshIndex].localModel;
				sub.materialHandle = scene.GetAssetManager().AddMaterial(mat);

				model.subMeshes.push_back(sub);
			}
		}
	}
	else {
		printf("Failed to load model at path: %s \n", path.c_str());
	}

	loadedModel.Dispose();
}

bool Engine::Intersect(entt::entity& outEntity) {
	vec3 rayO = freeCamera.GetCamera().position;
	vec3 target = freeCamera.GetCamera().position + freeCamera.GetCamera().forward;
	vec3 rayD = normalize(target - rayO);

	auto& reg = scene.GetRegistry();
	auto view = reg.view<const TransformComponent, const MeshFilter>();

	entt::entity closest;
	float nearestDist = D3D11_FLOAT32_MAX;

	const auto multiplyMatrixVector = [](DirectX::XMMATRIX matrix, vec3 vector) -> vec3 {
		DirectX::XMVECTOR v = DirectX::XMVectorSet(vector.x, vector.y, vector.z, 1);
		auto res = DirectX::XMVector3Transform(v, matrix);

		return vec3(DirectX::XMVectorGetX(res), DirectX::XMVectorGetY(res), DirectX::XMVectorGetZ(res));
	};

	view.each(
		[&](const auto e, const TransformComponent& tc, const MeshFilter& m) {
			Mesh* mesh = resourceProvider->GetResource<Mesh>(m.mesh);
			SubMesh& sub = mesh->subMeshes[m.subMeshIndex];

			for (size_t i = 0; i < sub.indexCount / 3; i++) {
				size_t baseIndex = sub.indexOffset + i * 3;

				const Vertex& v0 = mesh->vertices[mesh->indices[baseIndex]];
				const Vertex& v1 = mesh->vertices[mesh->indices[baseIndex + 1]];
				const Vertex& v2 = mesh->vertices[mesh->indices[baseIndex + 2]];

				float t, u, v;

				DirectX::XMMATRIX model = tc.Model();

				if (IsRayIntersecting(rayO, rayD, multiplyMatrixVector(model, v0.position), multiplyMatrixVector(model, v1.position), multiplyMatrixVector(model, v2.position), t, u, v)) {
					vec3 hitPoint = rayO + rayD * t;

					if (t < nearestDist) {
						closest = e;
						nearestDist = t;
					}

					printf("Intersection at : (x: %f, y: %f, z: %f), Distance: %f, Barycentric Coords: (u: %f, v: %f)\n", hitPoint.x, hitPoint.y, hitPoint.z, t, u, v);
				}
			}
		}
	);

	if (!reg.valid(closest)) {
		return false;
	}

	outEntity = closest;
	return true;
}

void Engine::CreatePipelines() {
	ShaderLoader shaderLoader;

	Shader gShader;
	gShader.inputLayoutFlag = InputLayout_PNTT;
	shaderLoader.Load(gShader, "Assets/Shaders/GeometryShader.hlsl");

	Shader sShader;
	sShader.inputLayoutFlag = InputLayout_PNTT;
	shaderLoader.Load(sShader, "Assets/Shaders/ShadowShader.hlsl");

	Shader lightShader;
	lightShader.inputLayoutFlag = InputLayout_None;
	shaderLoader.Load(lightShader, "Assets/Shaders/LightingShader.hlsl");

	Shader gradiantShader;
	gradiantShader.inputLayoutFlag = InputLayout_None;
	shaderLoader.Load(gradiantShader, "Assets/Shaders/GradiantShader.hlsl");

	Shader presentShader;
	presentShader.inputLayoutFlag = InputLayout_None;
	shaderLoader.Load(presentShader, "Assets/Shaders/PresentShader.hlsl");

	resourceProvider->AddPipelineStateContainer({
		.name = "GBufferPass",
		.shaderHandle = resourceProvider->LoadShader(gShader),
		.rasterizerHandle = resourceProvider->GetRasteriserHandle(RasterizerFunc_CullMode_Back, RasterizerFunc_FillMode_Solid),
		.depthStencilHandle = resourceProvider->GetDepthStateHandle(DepthWriteMask_All, DepthFunc_Less),
		.topology = PrimitiveTopology_Triangelist
	});


	resourceProvider->AddPipelineStateContainer({
		.name = "ShadowPass",
		.shaderHandle = resourceProvider->LoadShader(sShader),
		.rasterizerHandle = resourceProvider->GetRasteriserHandle(RasterizerFunc_CullMode_Back, RasterizerFunc_FillMode_Solid),
		.depthStencilHandle = resourceProvider->GetDepthStateHandle(DepthWriteMask_All, DepthFunc_Less),
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

	resourceProvider->AddPipelineStateContainer({
		.name = "PresentPass",
		.shaderHandle = resourceProvider->LoadShader(presentShader),
		.rasterizerHandle = resourceProvider->GetRasteriserHandle(RasterizerFunc_CullMode_None, RasterizerFunc_FillMode_Solid),
		.depthStencilHandle = resourceProvider->GetDepthStateHandle(DepthWriteMask_Zero, DepthFunc_Never),
		.topology = PrimitiveTopology_Triangelist
	});
}

void Engine::CreateRenderPasses(int width, int height) {
	StaticData sData{
		.mainLightDirection = { 0.25, 0.5f, -0.25f, 1 },
		.mainLightColor = { 1, 1, 1, 1 }
	};

	vec3 cPos(0, 1, -5);

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
	ShadowPass::LightBuffer shadowLightData{};
	ShadowPass::InitData shadowInit{
		.depthSV = resourceProvider->CreateTexture(4096, 4096, TextureFormat_R32_Typeless, TextureUsageFlags::TextureUsageFlag_DepthStencil | TextureUsageFlags::TextureUsageFlag_ShaderResource, TextureFilter::Point),
		.debugRT = resourceProvider->CreateTexture(4096, 4096, TextureFormat_RGBA8_UNorm, TextureUsageFlags::TextureUsageFlag_RenderTarget | TextureUsageFlags::TextureUsageFlag_ShaderResource, TextureFilter::Linear),
		.lightBufferHandle = resourceProvider->CreateBuffer(Buffer_Uasge_Default, 0, sizeof(ShadowPass::LightBuffer), &shadowLightData),
		.objectBufferHandle = objectBufferHandle,
		.pipeline = resourceProvider->GetPipelineStateContainer("ShadowPass")
	};
	renderer.AddRenderPass(new ShadowPass(shadowInit));

	LightingPass::MatricesBufferData lpmData;
	lpmData.invViewProj = DirectX::XMMatrixIdentity();

	LightingPass::LightPassShaderData lightPassShaderData{};

	LightingPass::InitData lightPassData{
		.albedoRT = gData.albedoRT,
		.normalRT = gData.normalRT,
		.materialRT = gData.materialRT,
		.depthRT = gData.depthRT,
		.shadowDSV = shadowInit.depthSV,
		.outputRT = resourceProvider->CreateTexture(width, height, TextureFormat_RGBA16F, TextureUsageFlags::TextureUsageFlag_RenderTarget | TextureUsageFlags::TextureUsageFlag_ShaderResource, TextureFilter::Linear),
		.lightBufferHandle = resourceProvider->CreateBuffer(Buffer_Uasge_Default, 0, sizeof(LightingPass::LightPassShaderData), &lightPassShaderData),
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