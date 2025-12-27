#pragma once

#include<imgui/backends/imgui_impl_dx11.h>
#include<imgui/backends/imgui_impl_win32.h>

class UI {
public:
	static void Setup(void* hWnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	static void NewFrame();
	static void Render();
	static void CleanUp();
private:
	UI();
};