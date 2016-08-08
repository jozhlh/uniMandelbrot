//3D Framework main

// Includes
#include <windows.h>
#include <stdio.h>
#include "Input.h"
#include "Scene3D.h"
#include "Timer.h"

// Globals
HWND hwnd;
Scene3D scene;
Timer timer;
Input input;
bool userContinue;

bool TestKeys();
bool UserContinue();

//Prototypes
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void RegisterMyWindow(HINSTANCE);
BOOL InitialiseMyWindow(HINSTANCE, int);
int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, int);

// Defines the window
void RegisterMyWindow(HINSTANCE hInstance)
{
	WNDCLASSEX  wcex;

	wcex.cbSize = sizeof (wcex);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "FirstWindowClass";
	wcex.hIconSm = 0;

	RegisterClassEx(&wcex);
}

// Attempts to create the window and display it
BOOL InitialiseMyWindow(HINSTANCE hInstance, int nCmdShow)
{
	hwnd = CreateWindow("FirstWindowClass",
		"Graphics Programming - Rolling Project",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL);
	if (!hwnd)
	{
		return FALSE;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	return TRUE;
}

// Entry point. The program start here
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow)
{
	MSG         msg;
	RegisterMyWindow(hInstance);

	if (!InitialiseMyWindow(hInstance, nCmdShow))
		return FALSE;

	scene.Init(&hwnd, &input);
	timer.Initialize();
	bool running = true;
	userContinue = true;

	while (running)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else
		{
			// update delta time
			timer.Frame();

			// update and render scene
			scene.Update(timer.GetTime());
			userContinue = TestKeys();
		}
		if (!userContinue)
		{
			running = UserContinue();
		}
	}
	// free up memory
	scene.CleanUp();
	return msg.wParam;
}

bool TestKeys()
{
	if (input.isKeyDown(VK_ESCAPE)) // if ESC is pressed
	{
		input.SetKeyUp(VK_ESCAPE); // force unpressing of ESC
		return false;
	}
	else {
		return true;
	}
}

bool UserContinue()
{
	int resp = MessageBox(NULL, "Are you sure you want to quit?", "Esc Pressed", MB_YESNO);
	if (resp == IDYES)
	{
		return false;
	}
	else {
		userContinue = true;
		return true;
	}
}

// handles window messages				
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_CREATE:
			break;
		case WM_SIZE:
			scene.Resize();
			break;
		case WM_KEYDOWN:
			input.SetKeyDown(wParam);
			break;
		case WM_KEYUP:
			input.SetKeyUp(wParam);
			break;
		case WM_MOUSEMOVE:
			input.setMouseX(LOWORD(lParam));
			input.setMouseY(HIWORD(lParam));
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}