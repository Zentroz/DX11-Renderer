#pragma once
#include"Window/ChiliWin.h"

struct WindowInitData {
	int width;
	int height;
};

class Window {
private:
	class WindowClass {
	public:
		static LPCWSTR GetNameWSTR() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		static constexpr LPCWSTR wndClassName = L"EZ-Editor Window Class";
		static WindowClass wndClass;
		HINSTANCE hInstance;
	};
public:
	Window() = default;
	~Window() = default;

	void Close();
	void CleanUp();

	void Init(const WindowInitData& initData);
	void Process();

	void SetEngine(void* engine) { this->engine = engine; }

	HWND GetHWND() const { return m_hWnd; }
	bool ShouldClose() const { return shouldClose; }

	void GetClientSize(int& width, int& height) const;

private:
	HWND m_hWnd;
	bool shouldClose;
	void* engine;

private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
};