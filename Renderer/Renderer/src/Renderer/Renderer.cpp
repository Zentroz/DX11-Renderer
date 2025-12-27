#include"Renderer.h"

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

		staticBufferHandle = resourceProvider->CreateBuffer(Buffer_Uasge_Immutable, 0, sizeof(StaticData), &sData);
		frameBufferHandle = resourceProvider->CreateBuffer(Buffer_Uasge_Default, 0, sizeof(FrameData), &fData);
		objectBufferHandle = resourceProvider->CreateBuffer(Buffer_Uasge_Dynamic, Buffer_CPU_Write, sizeof(ObjectData), &oData);
		materialBufferHandle = resourceProvider->CreateBuffer(Buffer_Uasge_Dynamic, Buffer_CPU_Write, sizeof(MaterialData), &mData);
		skyboxBufferHandle = resourceProvider->CreateBuffer(Buffer_Uasge_Dynamic, Buffer_CPU_Write, sizeof(SkyboxData), &sbData);

		PipelineStateContainer pbrOpaque;
		pbrOpaque.rasterizerHandle = resourceProvider->GetRasteriserHandle(RasterizerFunc_CullMode_None, RasterizerFunc_FillMode_Solid);
		pbrOpaque.topology = PrimitiveTopology_Triangelist;
		pbrOpaque.depthStencilHandle = 1;
		m_PipelineStates[PipelineStateType_PBR_Opaque] = pbrOpaque;

		PipelineStateContainer skybox;
		skybox.rasterizerHandle = resourceProvider->GetRasteriserHandle(RasterizerFunc_CullMode_Front, RasterizerFunc_FillMode_Solid);
		skybox.topology = PrimitiveTopology_Triangelist;
		skybox.depthStencilHandle = 1;
		m_PipelineStates[PipelineStateType_Skybox] = skybox;
	}

	void Renderer::SetCamera(Camera& cam) {
		renderCamera = cam;
	}
	void Renderer::SetSkybox(Handle textureHandle) {
		skyboxTextureHandle = textureHandle;
	}
	void Renderer::SetPipelineShader(PipelineStateType pipelineStateType, Handle shaderHandle) {
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
		RenderOpaque();
		RenderSkybox();
	}
	void Renderer::EndRender() {
		m_RenderQueue.clear();
	}

	void Renderer::RenderOpaque() {
		m_RenderContext->BindBufferVS(0, staticBufferHandle);
		m_RenderContext->BindBufferPS(0, staticBufferHandle);

		m_RenderContext->BindBufferVS(1, frameBufferHandle);
		m_RenderContext->BindBufferPS(1, frameBufferHandle);

		m_RenderContext->BindPipeline(GetPipelineState(PipelineStateType_PBR_Opaque));
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
	}
	void Renderer::RenderSkybox() {
		SkyboxData sbData;
		sbData.view = renderCamera.ViewMatrix();
		sbData.proj = renderCamera.ProjMatrix();
		m_RenderContext->UpdateBuffer(skyboxBufferHandle, sizeof(SkyboxData), &sbData);

		m_RenderContext->BindPipeline(GetPipelineState(PipelineStateType_Skybox));
		m_RenderContext->BindBufferVS(0, skyboxBufferHandle);
		m_RenderContext->BindTexturePS(0, skyboxTextureHandle);
		m_RenderContext->DrawGeometryIndexed(2);
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