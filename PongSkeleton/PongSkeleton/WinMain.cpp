#include <Windows.h> // Include the Windows API (Win32 Application Programming Interface)
#include <tchar.h> // Include UNICODE support
#include <sstream> // Include ostringstream support
using std::wostringstream;

#include <commdlg.h> // Include the Common Dialogs.
#include <process.h> // Include Multi-Threading functions.
#include "resource.h" // Include the resource script IDs.
#include "Game.h" // Include the Game Class.

// Foward Declarations:
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void OnPaint(HWND hWnd);
void OnLButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
void ToggleFullscreenMode(HWND hWnd, BOOL bFullScreen);

// Entry point function for the game:
unsigned int WINAPI GameMain(void* lpvArgList);
int main(void)
{
	_tWinMain(GetModuleHandle(NULL), NULL, GetCommandLine(), SW_SHOWDEFAULT);
}

#define CLIENT_WIDTH 800
#define CLIENT_HEIGHT 600

// The Entry Point function
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int iCmdShow)
{
	Game pong;

	// Initialize the COM Library.
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	// Step 1: Initialize the Window Class.
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(wcex);
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hInstance = hInstance;
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WindowProc; // Function to send Windows Message to for processing.
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_REALTIME);
	wcex.lpszClassName = _T("REALTIMEWND"); // Name to identify this Window Class.
	RegisterClassEx(&wcex);

	// Shoot for desired client size
	RECT rDesiredClient;
	rDesiredClient.left = 0;
	rDesiredClient.top = 0;
	rDesiredClient.right = CLIENT_WIDTH;
	rDesiredClient.bottom = CLIENT_HEIGHT;

	AdjustWindowRect(&rDesiredClient, WS_OVERLAPPEDWINDOW, TRUE);

	int iWindowWidth = rDesiredClient.right - rDesiredClient.left;
	int iWindowHeight = rDesiredClient.bottom - rDesiredClient.top;

	// Step 2: Create the Main Window.
	HWND hWnd = CreateWindow(wcex.lpszClassName,
		_T("Real-Time Skeleton"),
		WS_OVERLAPPEDWINDOW,
		(GetSystemMetrics(SM_CXSCREEN) / 2) - (iWindowWidth / 2),
		(GetSystemMetrics(SM_CYSCREEN) / 2) - (iWindowHeight / 2),
		iWindowWidth, iWindowHeight,
		HWND_DESKTOP,
		NULL,
		hInstance,
		NULL);
	if (!hWnd)
	{
		//DWORD err = GetLastError();
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to create main window!"),
			_T("Program Name"), MB_OK | MB_ICONERROR);
		return -1;
	}

	// Attach game to the window
	SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG)&pong);

	pong.worker = std::thread(GameMain, hWnd);

	// Fullscreen?
	if (IDYES == MessageBox(NULL, L"Do you want to go fullscreen?", L"Fullscreen...", MB_YESNO))
	{
		ToggleFullscreenMode(hWnd, TRUE);
	}

	// Step 3: Show the Main Window.
	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);

	// Step 4: Enter the Main Message Loop.
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0)) // Getting WM_QUIT makes GetMessage return FALSE.
	{
		TranslateMessage(&msg); // Generate WM_CHAR messages (case sensitive input).
		DispatchMessage(&msg); // Invoke the WindowProc for message processing.
	}

	// Uninitialize the COM Library.
	CoUninitialize();

	return msg.wParam;
}

// Entry point function for the game:
unsigned int WINAPI GameMain(void* lpvArgList)
{
	// Initialize the COM Library.
	CoInitializeEx(NULL, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE);

	// Increase the accuracy/precision of the Windows timer:
	TIMECAPS tc;
	timeGetDevCaps(&tc, sizeof(TIMECAPS));
	timeBeginPeriod(tc.wPeriodMin);


	// Get the HWND value out of the ArgList parameter.
	HWND hWnd = (HWND)lpvArgList;

	// Get the window data (Game).
	Game* pong = (Game*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	pong->SetHwnd(hWnd);
	pong->Startup();

	while (pong->bRunning)
	{
		pong->Input();
		pong->Simulate();
		pong->Render();
	}

	pong->Shutdown();

	// Restore the accuracy/precision of the Windows timer:
	timeEndPeriod(tc.wPeriodMin);

	// Uninitialize the COM Library.
	CoUninitialize();

	return 0;
}

// Step 5: Write the Window Procedure
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		// Handle Window Creation:
	case WM_CREATE:
		break;

		// Handle Close Button [X] Pressed:
	case WM_CLOSE:
	{
					 // Get the window data (Game).
					 Game* pong = (Game*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

					 // Begin GameMain shutdown.
					 pong->bRunning = FALSE;

					 // Wait until Game Thread terminates.
					 //WaitForSingleObject(pong->hGameThread, INFINITE);

					 if (pong->worker.joinable()) pong->worker.join();

					 // Begin WinMain shutdown.
					 DestroyWindow(hWnd);
	}
		break;

		// Handle Window Destruction:
	case WM_DESTROY:
		PostQuitMessage(0); // Put the WM_QUIT message in the message queue.
		break;
	case WM_ERASEBKGND:
		// Do Nothing
		break;
		// Handle Window Rendering:
	case WM_PAINT: //(output)
		OnPaint(hWnd);
		break;
		// Handle Left Mouse Button Clicked:
	case WM_LBUTTONDOWN: //(input)
		OnLButtonDown(hWnd, wParam, lParam);
		break;
		// Handle keyboard input:
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	{
						// Get the window data (Game).
						Game* pong = (Game*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

						GetKeyboardState(pong->keys);
						DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
		break;

		// Handle window activation:
	case WM_ACTIVATE:
	{
						// Get the window data (Game).
						Game* pong = (Game*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

						if (LOWORD(wParam) != WA_INACTIVE)
							GetKeyboardState(pong->keys);
						else
							ZeroMemory(pong->keys, 256);
	}
		break;


		//case WM_SIZE:
		//	{
		//		// Resize the Render Target based upon the new client.
		//		D2D1_SIZE_U size = D2D1::SizeU(LOWORD(lParam), HIWORD(lParam));
		//		pRT->Resize(size);
		//	}
		//	break;

		// Handle Menu Items:
	case WM_COMMAND:
		OnCommand(hWnd, wParam, lParam);
		break;

	default:
		// Allow Windows to perform default processing.
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

// PAINT FUNCTION //
void OnPaint(HWND hWnd)
{
	ValidateRect(hWnd, NULL); // VERY IMPORTANT - Clear the Invalid flag
}

// CONTROLS THE LEFT MOUSE CLICK //
void OnLButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rc;
	GetClientRect(hWnd, &rc);

	POINT ptMouse;
	ptMouse.x = LOWORD(lParam); // Bits 0-15 contain the X Axis position
	ptMouse.y = HIWORD(lParam); // Bits 16-31 contain the Y Axis position

	InvalidateRect(hWnd, NULL, TRUE);
}

// CONTROLS THE WINDOW COMMANDS //
void OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	int menuid = LOWORD(wParam);

	switch (menuid)
	{
	case ID_ABOUT:
		MessageBox(hWnd, L"Contact info:\nJose Rivera\nstudentID: 2025044\nEmail: jose102@fullsail.edu", L"Contact Info", MB_OK);
		break;
	case IDM_FILE_NEW:
		break;
	case IDM_FILE_EXIT:
		PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;
	}
}

// Function that toggles a window between fullscreen and windowed mode:
void ToggleFullscreenMode(HWND hWnd, BOOL bFullScreen)
{
	static WINDOWPLACEMENT wp = { 0 };
	static HMENU hMenu = NULL;

	if (bFullScreen)
	{
		// Remember the window position.
		wp.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(hWnd, &wp);

		// Remove the window's title bar.
		SetWindowLongPtr(hWnd, GWL_STYLE, WS_POPUP);

		// Put the changes to the window into effect.
		SetWindowPos(hWnd, 0, 0, 0, 0, 0,
			SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);

		// Remember the menu, then remove it.
		hMenu = GetMenu(hWnd);
		SetMenu(hWnd, NULL);

		// Position the window to cover the entire screen.
		SetWindowPos(hWnd, HWND_TOPMOST,
			0, 0,
			GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
			SWP_SHOWWINDOW);

		// Remove the cursor.
		ShowCursor(FALSE);
	}
	else
	{
		// Restore the window's title bar.
		SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

		// Put the changes to the window into effect.
		SetWindowPos(hWnd, 0, 0, 0, 0, 0,
			SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);

		// Restore the window's menu.
		SetMenu(hWnd, hMenu);

		// Restore the window's original position.
		SetWindowPlacement(hWnd, &wp);

		// Restore the cursor.
		ShowCursor(TRUE);
	}
}