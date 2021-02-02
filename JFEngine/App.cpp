#include "App.h"
#include <sstream>
#include <iomanip>

App::App()
	:
	wnd(800, 600, "The Donkey Fart Box")
{}

int App::Go()
{
	MSG msg;
	BOOL gResult;
	while(true)
	{
		// TranslateMessage will post auxilliary WM_CHAR messages from key msgs
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		// process all messages pending, but to not block for new messages
		if (const auto ecode = Window::ProcessMessages())
		{
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}

		DoFrame();
	}

	// check if GetMessage call itself borked
	if (gResult == -1)
	{
		throw CHWND_LAST_EXCEPT();
	}

	// wParam here is the value passed to PostQuitMessage
	return (int)msg.wParam;
}

void App::DoFrame()
{
	wnd.Gfx().EndFrame();
}