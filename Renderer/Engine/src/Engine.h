#pragma once

#include"Engine/Scene.h"
#include"Utility/FreelookCamera.h"
#include"Editor/ObjectPanel.h"
#include"Editor/UI.h"

#include<Renderer/D3D11/GraphicsDevice.h>
#include<Renderer/Render/ResourceProviderInterface.h>
#include<Renderer/Renderer.h>

#include<entt/entt.hpp>

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
	entt::entity intersectedEntity;

	zRender::Renderer renderer{};
	std::unique_ptr<zRender::D3D11Device> graphicsDevice;
	zRender::IRenderResourceProvider* resourceProvider;
	ICommandContext* commandContext;

private:
	void CreateRenderPasses(int width, int height);
	void RebuildRenderPass(int width, int height);
	void Resize(int newWidth, int newHeight, bool fullscreen);

	bool Intersect(entt::entity& outEntity);
};