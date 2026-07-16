#include <PIMM/Window/Window.h>
#include <Windows.h>
#include <stdexcept>

//IMGUI//
#include <PIMM/ImGui/imgui_impl_dx11.h>
#include <PIMM/ImGui/imgui_impl_win32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
		return true;

	switch (msg)
	{
		//Close Window
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			break;
		}
		//Default, pass all parameters
		default: return DefWindowProc(hwnd, msg, wparam, lparam);
	}

	return 0;
}

pimm::Window::Window(const WindowDescriptor& descriptor) : 
	Base(descriptor.base), 
	m_size(descriptor.size)
{
	//Create lambda function to register the window
	//Defined inline, convenient way for small short-time functions
	auto registerWindowClassFunction = []()
	{
		// Brace initialize wc to its default values
		WNDCLASSEX wc{};
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.lpszClassName = L"PIMMWindow";
		wc.lpfnWndProc = &WindowProcedure;				//Define window procedure
		return RegisterClassEx(&wc);

	};

	//Static const avoids multiple ids being generated
	static const auto windowClassID = std::invoke(registerWindowClassFunction);

	//Window class failure checker
	if (!windowClassID) PIMMLogThrowError("[ERROR] RegisterClassEx() failed.");

	RECT rc{ 0, 0, m_size.width, m_size.height };
	AdjustWindowRect(&rc, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, false);

	m_handle = CreateWindowEx
	(
		NULL,									//Extended Window Style
		MAKEINTATOM(windowClassID),				//Window Class
		L"PIMM | DirectX C++ Game Engine",		//Set Title of Window
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, //Window Style, create standard window which includes title bar and system menu
		CW_USEDEFAULT,							//Window position set to default
		CW_USEDEFAULT,							//Window position set to default
		rc.right - rc.left,						//Window width
		rc.bottom - rc.top,						//Window height
		NULL,
		NULL,
		NULL,
		NULL
	);

	//Handle failure checker
	if (!m_handle) PIMMLogThrowError("[ERROR] CreateWindowEx() failed.");

	//Show the window if no errors occured
	ShowWindow(static_cast<HWND>(m_handle), SW_SHOW);
}

pimm::Rect pimm::Window::GetClientAreaInScreenSpace()
{
	auto hWnd = static_cast<HWND>(m_handle);

	RECT client{};
	GetClientRect(hWnd, &client);

	POINT topLeft{ client.left, client.top };
	POINT bottomRight{ client.right, client.bottom };
	ClientToScreen(hWnd, &topLeft);
	ClientToScreen(hWnd, &bottomRight);

	return {
		topLeft.x,
		topLeft.y,
		bottomRight.x - topLeft.x,
		bottomRight.y - topLeft.y
	};
}

pimm::Window::~Window()
{
	DestroyWindow(static_cast<HWND>(m_handle));
}
