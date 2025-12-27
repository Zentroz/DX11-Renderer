#pragma once
#include"Window/ChiliWin.h"

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

	void Init();
	void Process();

	HWND GetHWND() const { return m_hWnd; }
	bool ShouldClose() const { return shouldClose; }

private:
	HWND m_hWnd;
	bool shouldClose;

private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
};