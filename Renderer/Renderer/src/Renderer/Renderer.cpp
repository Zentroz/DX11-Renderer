#include"Renderer.h"

#include"RenderPasses/GBufferPass.h"
#include"RenderPasses/GBufferDebugPass.h"
#include"RenderPasses/PresentPass.h"
#include"RenderPasses/LightingPass.h"
#include"RenderPasses/ShadowPass.h"

namespace zRender {

	DirectX::XMMATRIX GetVPMatrix(vec3 cameraPosition) {
		DirectX::XMVECTOR eyePosition = DirectX::XMVectorSet(cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0f);
		DirectX::XMVECTOR focusPosition = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		DirectX::XMVECTOR upDirection = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		float aspect = 1536.0f / 793.0f;
		constexpr float fov = DirectX::XMConvertToRadians(60.0f);

		DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);
		DirectX::XMMATRIX proj = DirectX::XMMatrixPerspectiveFovLH(fov, aspect, 0.1f, 100.0f);

		return view * proj;
	}

	void Renderer::Setup(IRenderContext* renderContext, IRenderResourceProvider* resourceProvider) {
		this->m_RenderContext = renderContext;
		this->resourceProvider = resourceProvider;

		StaticData sData;
		sData.mainLightDirection = { 0.25, 0.5f, -0.25f, 1 };
		sData.mainLightColor = { 1, 1, 1, 1 };

		vec3 cPos = { 0, 1, -5 };

		FrameData fData;
		fData.vpMatrix = DirectX::XMMatrixTranspose(GetVPMatrix(cPos));
		fData.cameraPosition = { cPos.x, cPos.y, cPos.z, 1 };

		ObjectData oData;
		oData.modelMatrix = DirectX::XMMatrixIdentity();

		MaterialData mData;
		mData.diffuseColor = { 0, 0, 0 };
		mData.metallic = 0;
		mData.roughness = 0;

		SkyboxData sbData;
		sbData.view = DirectX::XMMatrixIdentity();
		sbData.proj = DirectX::XMMatrixIdentity();

		float width = 1536;
		float height = 793;

		staticBufferHandle = resourceProvider->CreateBuffer(Buffer_Uasge_Immutable, 0, sizeof(StaticData), &sData);
		frameBufferHandle = resourceProvider->CreateBuffer(Buffer_Uasge_Default, 0, sizeof(FrameData), &fData);
		objectBufferHandle = resourceProvider->CreateBuffer(Buffer_Uasge_Dynamic, Buffer_CPU_Write, sizeof(ObjectData), &oData);
		materialBufferHandle = resourceProvider->CreateBuffer(Buffer_Uasge_Dynamic, Buffer_CPU_Write, sizeof(MaterialData), &mData);
		skyboxBufferHandle = resourceProvider->CreateBuffer(Buffer_Uasge_Dynamic, Buffer_CPU_Write, sizeof(SkyboxData), &sbData);
		
		GBuffer gData = {};
		gData.albedoRT = resourceProvider->CreateTexture(width, height, TextureFormat_RGBA8_UNorm, TextureUsageFlags::TextureUsageFlag_RenderTarget | TextureUsageFlags::TextureUsageFlag_ShaderResource);
		gData.normalRT = resourceProvider->CreateTexture(width, height, TextureFormat_RGB10A2_UNorm, TextureUsageFlags::TextureUsageFlag_RenderTarget | TextureUsageFlags::TextureUsageFlag_ShaderResource);
		gData.materialRT = resourceProvider->CreateTexture(width, height, TextureFormat_RGBA8_UNorm, TextureUsageFlags::TextureUsageFlag_RenderTarget | TextureUsageFlags::TextureUsageFlag_ShaderResource);
		gData.depthRT = resourceProvider->CreateTexture(width, height, TextureFormat_R32_Typeless, TextureUsageFlags::TextureUsageFlag_DepthStencil | TextureUsageFlags::TextureUsageFlag_ShaderResource);
		gData.objectBufferHandle = objectBufferHandle;
		gData.materialBufferHandle = materialBufferHandle;
		gData.pipelineStateHandles = resourceProvider->GetPipelineStateContainer("GBufferPass");
		GBufferPass* gPass = new GBufferPass(gData);
		renderGraph.AddPass(gPass);
		
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

		LightingPass::MatricesBufferData lpmData;
		lpmData.invViewProj = DirectX::XMMatrixIdentity();

		LightingPass::InitData lightPassData;
		lightPassData.albedoRT = gData.albedoRT;
		lightPassData.normalRT = gData.normalRT;
		lightPassData.materialRT = gData.materialRT;
		lightPassData.depthRT = gData.depthRT;
		//lightPassData.shadowDSV = shadowInit.depthSV;
		lightPassData.shadowDSV = InvalidHandle;
		lightPassData.outputRT = resourceProvider->CreateTexture(width, height, TextureFormat_RGBA8_UNorm, TextureUsageFlags::TextureUsageFlag_RenderTarget | TextureUsageFlags::TextureUsageFlag_ShaderResource);
		lightPassData.matricesBufferHandle = resourceProvider->CreateBuffer(Buffer_Uasge_Default, 0, sizeof(LightingPass::MatricesBufferData), &lpmData);
		lightPassData.pipeline = resourceProvider->GetPipelineStateContainer("LightPass");
		LightingPass* lightPass = new LightingPass(lightPassData);
		renderGraph.AddPass(lightPass);

		PresentPass* presentPass = new PresentPass(lightPassData.outputRT, resourceProvider->GetScreenTextureHandle(), resourceProvider->GetPipelineStateContainer("PresentPass"));
		renderGraph.AddPass(presentPass);
	}

	void Renderer::SetCamera(Camera& cam) {
		renderCamera = cam;
	}
	void Renderer::SetSkybox(TextureHandle textureHandle) {
		skyboxTextureHandle = textureHandle;
	}
	void Renderer::SetPipelineShader(PipelineStateType pipelineStateType, TextureHandle shaderHandle) {
		m_PipelineStates[pipelineStateType].shaderHandle = shaderHandle;
	}

	void Renderer::Shutdown() {}

	void Renderer::InitRender() {
		m_RenderContext->BeginFrame();

		vec3 pos = renderCamera.position;
		FrameData fData;
		fData.vpMatrix = DirectX::XMMatrixTranspose(renderCamera.ViewProjMatrix());
		fData.cameraPosition = { pos.x, pos.y, pos.z, 1 };
		m_RenderContext->UpdateBuffer(frameBufferHandle, 0, &fData);
	}

	void Renderer::Render() {
		m_RenderContext->BindBufferVS(0, staticBufferHandle);
		m_RenderContext->BindBufferPS(0, staticBufferHandle);
		m_RenderContext->BindBufferVS(1, frameBufferHandle);
		m_RenderContext->BindBufferPS(1, frameBufferHandle);
		RenderPassContext ctx(m_RenderContext, renderCamera, m_RenderQueue);
		renderGraph.Execute(ctx);
	}
	void Renderer::EndRender() {
		m_RenderQueue.clear();
	}

	void Renderer::RenderOpaque() {
		/*
		Handle h[1] = {resourceProvider->GetScreenTextureHandle()};
		m_RenderContext->BindMultiViews(1, h, InvalidHandle);

		m_RenderContext->BindBufferVS(0, staticBufferHandle);
		m_RenderContext->BindBufferPS(0, staticBufferHandle);

		m_RenderContext->BindBufferVS(1, frameBufferHandle);
		m_RenderContext->BindBufferPS(1, frameBufferHandle);

		PipelineStateContainer pipeline = resourceProvider->GetPipelineStateContainer("PBROpaque");
		m_RenderContext->BindPipeline(pipeline);
		for (const auto& item : m_RenderQueue) {
			ObjectData objectData;
			objectData.modelMatrix = DirectX::XMMatrixTranspose(item.modelMatrix);

			MaterialData matData;
			matData.diffuseColor = item.material.diffuseColor;
			matData.metallic = item.material.metallic;
			matData.roughness = item.material.roughness;

			m_RenderContext->UpdateBuffer(objectBufferHandle, sizeof(ObjectData), &objectData);
			m_RenderContext->UpdateBuffer(materialBufferHandle, sizeof(MaterialData), &matData);

			m_RenderContext->BindBufferVS(2, objectBufferHandle);
			m_RenderContext->BindBufferPS(2, materialBufferHandle);
			m_RenderContext->BindTexturePS(0, item.material.diffuseTexHandle);
			m_RenderContext->BindTexturePS(1, item.material.normalTexHandle);
			m_RenderContext->DrawGeometryIndexed(item.meshHandle);
		}
		*/
	}
	void Renderer::RenderSkybox() {
		SkyboxData sbData;
		sbData.view = DirectX::XMMatrixTranspose(renderCamera.ViewMatrix());
		sbData.proj = DirectX::XMMatrixTranspose(renderCamera.ProjMatrix());
		m_RenderContext->UpdateBuffer(skyboxBufferHandle, sizeof(SkyboxData), &sbData);

		m_RenderContext->BindPipeline(GetPipelineState(PipelineStateType_Skybox));
		m_RenderContext->BindBufferVS(0, skyboxBufferHandle);
		m_RenderContext->BindTexturePS(0, skyboxTextureHandle);
		MeshHandle cube = 2;
		m_RenderContext->DrawGeometryIndexed(cube, 0);
	}

	void Renderer::Queue(RenderItem item) {
		m_RenderQueue.push_back(item);
	}

	PipelineStateContainer& Renderer::GetPipelineState(PipelineStateType state) {
		return m_PipelineStates[state];
	}

	void Renderer::AddLight(Light light) {
		m_Lights[lightCount++] = light;
	}

}

/*
PipelineStateContainer pbrOpaque;
pbrOpaque.rasterizerHandle = resourceProvider->GetRasteriserHandle(RasterizerFunc_CullMode_Back, RasterizerFunc_FillMode_Solid);
pbrOpaque.depthStencilHandle = resourceProvider->GetDepthStateHandle(DepthWriteMask_All, DepthFunc_LessEqual);
pbrOpaque.shaderHandle = resourceProvider->GetDefaultGeometryShaderHandle();
pbrOpaque.topology = PrimitiveTopology_Triangelist;
m_PipelineStates[PipelineStateType_PBR_Opaque] = pbrOpaque;

PipelineStateContainer skybox;
skybox.rasterizerHandle = resourceProvider->GetRasteriserHandle(RasterizerFunc_CullMode_None, RasterizerFunc_FillMode_Solid);
skybox.depthStencilHandle = resourceProvider->GetDepthStateHandle(DepthWriteMask_Zero, DepthFunc_LessEqual);
skybox.topology = PrimitiveTopology_Triangelist;
m_PipelineStates[PipelineStateType_Skybox] = skybox;
*/