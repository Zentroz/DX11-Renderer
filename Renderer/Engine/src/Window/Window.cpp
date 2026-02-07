#include"Window/Window.h"

#include<string>
#include<stdexcept>

#include<imgui/backends/imgui_impl_win32.h>

std::wstring widen(const char* str) {
    return std::wstring(str, str + strlen(str));
}

Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept : hInstance(GetModuleHandle(nullptr)) {
    WNDCLASSEX wc = { 0 };

    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = HandleMsgSetup;
    wc.hInstance = GetInstance();
    wc.lpszClassName = GetNameWSTR();

    RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass() {
    UnregisterClass(GetNameWSTR(), GetInstance());
}

LPCWSTR Window::WindowClass::GetNameWSTR() noexcept
{
    return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
    return wndClass.hInstance;
}

void Window::GetClientSize(int& width, int& height) const {
    RECT windowRect;

    if (!GetClientRect(m_hWnd, &windowRect))
    {
        assert("Couldn't get window rect.");
    }

    width = windowRect.right - windowRect.left;
    height = windowRect.bottom - windowRect.top;
}

void Window::Init(const WindowInitData& initData) {
    int clientWidth = initData.width;
    int clintHeight = initData.height;

    RECT wr;
    wr.left = 100;
    wr.right = clientWidth + wr.left;
    wr.top = 100;
    wr.bottom = clintHeight + wr.top;

    AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

    int width = wr.right - wr.left;
    int height = wr.bottom - wr.top;

    m_hWnd = CreateWindowEx(
        0,
        WindowClass::GetNameWSTR(),
        L"DX11-Renderer",
        WS_OVERLAPPEDWINDOW | WS_MAXIMIZE,    // Window style

        // Size and position
        0, 0, width, height,

        NULL,
        NULL,
        WindowClass::GetInstance(),
        this
    );

    if (m_hWnd == NULL)
    {
        throw std::runtime_error("Failed to create window.");
        return;
    }

    ShowWindow(m_hWnd, SW_SHOW);
    shouldClose = false;
}

void Window::Process() {
    MSG msg = { };

    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            break;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void Window::Close() {
    shouldClose = true;
}

void Window::CleanUp() {
    DestroyWindow(m_hWnd);
    m_hWnd = NULL;
}

bool IsFullscreen(HWND hwnd) {
    RECT windowRect;
    // Get the coordinates of the window's bounding rectangle
    if (!GetWindowRect(hwnd, &windowRect)) {
        return false; // Error getting window rect
    }

    // Get the dimensions of the primary screen
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Calculate window width and height from its rect
    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;

    // Check if window dimensions match screen dimensions
    // Note: A small tolerance might be needed in some complex scenarios, 
    // but typically they will match exactly if fullscreened by standard methods.
    if (windowWidth == screenWidth && windowHeight == screenHeight) {
        return true;
    }
    else {
        return false;
    }
}


LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    if (msg == WM_NCCREATE)
    {
        const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
        Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
        SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
        return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}


LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

//extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#include<Engine.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    break;
    case WM_SIZE:
        RECT windowRect;

        if (!GetClientRect(hWnd, &windowRect))
        {
            assert("Couldn't get window rect.");
        }

        if (engine == nullptr) break;

        static_cast<Engine*>(engine)->QueueResize(windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, IsFullscreen(m_hWnd));
        break;
    case WM_CLOSE:
        Close();
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

