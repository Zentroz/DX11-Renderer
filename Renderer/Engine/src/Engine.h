#pragma once

#include"Engine/Scene.h"
#include"Utility/FreelookCamera.h"
#include"Editor/ObjectPanel.h"
#include"Editor/UI.h"

#include<Renderer/D3D11/GraphicsDevice.h>
#include<Renderer/D3D11/RenderContext.h>
#include<Renderer/D3D11/ResourceProvider.h>
#include<Renderer/Renderer.h>

struct EngineInitData {
	int width;
	int height;
	void* hWnd;
};

class Engine {
public:
	Engine() = default;

	void Init(const EngineInitData& initData);
	void CleanUp();

	void QueueResize(int width, int height, bool fullscreen);

	void Run();

	void OpenModelFile(const std::string& path);

private:
	FreelookCamera freeCamera{};
	ObjectPanel objectPanel{};
	Scene scene{};
	bool resize = false;
	int newWidth;
	int newHeight;
	bool fullscreen;

	zRender::Renderer renderer{};
	zRender::D3D11Device graphicsDevice{};
	std::unique_ptr<zRender::D3D11ResourceProvider> resourceProvider;
	std::unique_ptr<zRender::D3D11RenderContext> renderContext;

private:
	void CreatePipelines();
	void CreateRenderPasses(int width, int height);
	void RebuildRenderPass(int width, int height);
	void Resize(int newWidth, int newHeight, bool fullscreen);
};