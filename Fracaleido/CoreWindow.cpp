////////////////////////////////////////////////////////////////////////////////////////
//
// Cpasules for Window handling
//
////////////////////////////////////////////////////////////////////////////////////////

#include "CoreWindow.h"
#include "CoreMsg.h"
#include "CoreMacros.h"

#include <vector>
#include <thread>

#define INIT_WIN_NUM 10
#define WINDOW_JUMP 20

UINT winClassCnt = 0;
UINT stdX = 100;
UINT stdY = 100;

vector<Window*> allWindows;

//------------------------CLASS MEMBERS-------------------------

DefWindow::DefWindow() : x(stdX), y(stdY), width(100), height(100), funcInit(NULL), funcLoop(NULL), funcResize(NULL), funcPaint(NULL), funcOnKey(NULL), funcOnMouse(NULL), funcOnWinEvent(NULL), funcDestroy(NULL), title("") {
	stdX += WINDOW_JUMP;
	stdY += WINDOW_JUMP;
}

Window::Window(DefWindow& p) : x(p.x), y(p.y), width(p.width), height(p.height), funcInit(p.funcInit), funcLoop(p.funcLoop), funcResize(p.funcResize), funcPaint(p.funcPaint), funcOnKey(p.funcOnKey), funcOnMouse(p.funcOnMouse), funcOnWinEvent(p.funcOnWinEvent), funcDestroy(p.funcDestroy), title(p.title), hinst(p.hinst), parent(p.parent), isShown(false), userData(p.userData) {
	wchar_t winClassCntStr[8];
	swprintf_s(winClassCntStr,8,L"%05.5d",winClassCnt);

	std::wstring titleW = std::wstring(title.begin(), title.end());

	WNDCLASS wc = {0};
    wc.hbrBackground =(HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.hCursor = LoadCursor( hinst, IDC_ARROW );
    wc.hIcon = LoadIcon( hinst, IDI_APPLICATION );
    wc.hInstance = hinst;
	wc.lpfnWndProc = WndProc;
    wc.lpszClassName = winClassCntStr;
    wc.style = CS_HREDRAW | CS_VREDRAW;

    if (! RegisterClass( &wc ) ) {
		throw winClassCreationEx();
    }
    
	hMe = CreateWindow(
		winClassCntStr, 
		titleW.c_str(),
        WS_OVERLAPPEDWINDOW,
        x, y, width, height,
        parent,
        NULL,
        hinst, NULL 
	);

	winClassCnt++;

	SetWindowLong(hMe, GWL_USERDATA, (LONG)this);
	allWindows.push_back(this);
	if(!userData) userData = this;

};

Window::~Window() {
	closeWindow();
};

void Window::showWindow() {
	if(isShown) return;
	ShowWindow( hMe, SW_SHOWNORMAL );
	UpdateWindow( hMe );
	if(funcInit) funcInit(hMe,userData);
	isShown = true;
}

void Window::closeWindow() {
	if(IsWindow(hMe)) {
		DestroyWindow(hMe);
	}
}

//------------------------STATIC MEMBERS------------------------

int Window::closeAllWindows() {
	for(Window* w : allWindows) {
		w->closeWindow();
	}
	allWindows.clear();
	return 0;
}

void Window::EnterLoop() {

	//device input stuff
	UINT numDev = 2;
	RAWINPUTDEVICE* Rid = new RAWINPUTDEVICE[numDev];
    
	Rid[0].usUsagePage = 0x01; 
	Rid[0].usUsage = 0x02; 
	Rid[0].dwFlags = 0;
	Rid[0].hwndTarget = 0;

	Rid[1].usUsagePage = 0x01; 
	Rid[1].usUsage = 0x06; 
	Rid[1].dwFlags = 0;
	Rid[1].hwndTarget = 0;

	if (RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])) == FALSE) {
		delete Rid;
		throw winClassCreationEx();
	}

	delete Rid;

	//input buffers
	ULONG rawBuffer[128];
	UINT rawBufferSize = sizeof(rawBuffer);

	//window processing
	UINT numWin = allWindows.size();

	//mesage handling
	MSG msg = {0};
	bool running = true;
	bool firstMouse = true;
	while(running) {

		while ( PeekMessage( &msg, 0, 0, WM_INPUT_DEVICE_CHANGE - 1, PM_REMOVE) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
			if(msg.message == WM_QUIT) {
				running = false;
				break;
			}
		}

		while ( PeekMessage( &msg, 0, WM_INPUT + 1, 0xffffffff, PM_REMOVE) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg ); 
			if(msg.message == WM_QUIT) {
				running = false;
				break;
			}
		}

		if(running) {
			
			UINT activeWin = 0;
			
			HWND focusWindow = GetFocus();
			Window* focus = NULL;
			for(Window* w : allWindows) {
				if(w->hMe == focusWindow) {
					focus = w;		
				}
				if(IsWindow(w->hMe)) {
					activeWin++;
				}
			}
			if(!activeWin) { //Nothing open anymore
				PostQuitMessage( 0 );
				running = false;
				continue;
			}

			//Input
			UINT bytes = rawBufferSize;
			UINT count = GetRawInputBuffer((PRAWINPUT)rawBuffer, &bytes, sizeof(RAWINPUTHEADER));
			if(count > 0) {
				RAWINPUT* raw = (RAWINPUT*) rawBuffer;
				for (UINT i = 0; i < count; i++) { 
					MYRAWINPUT* r = (MYRAWINPUT*) raw;

					if (r->header.dwType == RIM_TYPEKEYBOARD) {
						KeyEvent e;
						e.key = r->data.keyboard.VKey;
						e.keyDown = GETBYOFFSET(raw->data.keyboard,BYTE,10) == RI_KEY_MAKE;
						e.keyUp = GETBYOFFSET(raw->data.keyboard,BYTE,10) == RI_KEY_BREAK;

						if(focus && focus->funcOnKey) {
							focus->funcOnKey(focus->getHWND(),e,focus->userData);
						}
					}
					if (raw->header.dwType == RIM_TYPEMOUSE) {
						MouseEvent e;
						e.posAbsolute = raw->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE;
						e.p.x = r->data.mouse.lLastX;
						e.p.y = r->data.mouse.lLastY;

						e.left = MouseButtonChange::NOCHANGE;
						if(GETBYOFFSET(raw->data.mouse,BYTE,12) & RI_MOUSE_LEFT_BUTTON_DOWN) e.left = MouseButtonChange::DOWN; 
						if(GETBYOFFSET(raw->data.mouse,BYTE,12) & RI_MOUSE_LEFT_BUTTON_UP) e.left = MouseButtonChange::UP;
						e.middle = MouseButtonChange::NOCHANGE;
						if(GETBYOFFSET(raw->data.mouse,BYTE,12) & RI_MOUSE_MIDDLE_BUTTON_DOWN) e.middle = MouseButtonChange::DOWN;
						if(GETBYOFFSET(raw->data.mouse,BYTE,12) & RI_MOUSE_MIDDLE_BUTTON_UP) e.middle = MouseButtonChange::UP;
						e.right = MouseButtonChange::NOCHANGE;
						if(GETBYOFFSET(raw->data.mouse,BYTE,12) & RI_MOUSE_RIGHT_BUTTON_DOWN) e.right = MouseButtonChange::DOWN;
						if(GETBYOFFSET(raw->data.mouse,BYTE,12) & RI_MOUSE_RIGHT_BUTTON_UP) e.right = MouseButtonChange::UP;

						e.mouseWheelDelta = 0;
						if(GETBYOFFSET(raw->data.mouse,SHORT,12) & RI_MOUSE_WHEEL) {
							e.mouseWheelDelta = GETBYOFFSET(raw->data.mouse,SHORT,14)<0?-1:1;
						}

						if(focus && focus->funcOnMouse) {
							focus->funcOnMouse(focus->getHWND(),e,focus->userData);
						}
					}

					raw = NEXTRAWINPUTBLOCK(raw); 
				} 
			}

			for(Window* w : allWindows) {
				
				if(w->funcLoop != NULL) { //maybe multi thread this - watch mem leaks
					w->funcLoop(w->getHWND(),w->userData);
				}
			}
			
		}
	}

	closeAllWindows();
}

LRESULT CALLBACK Window::WndProc( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam )
{
	Window* w = (Window*) GetWindowLong(hwnd, GWL_USERDATA);
	
    switch( message )
    {
        case WM_CREATE:
			//window created
            return 0;
            break;

		case WM_MOVE:   
			//printf("move\n");
			w->x = LOWORD(lparam);
			w->y = HIWORD(lparam);
            return 0;
            break;

		case WM_SIZE:
			//printf("size\n");
			w->width = LOWORD(lparam);
			w->height = HIWORD(lparam);
			if(w->funcResize) w->funcResize(hwnd,w->userData);
            return 0;
            break;

        case WM_PAINT:
			//printf("paint\n");
			if(w->funcPaint) {
				w->funcPaint(hwnd,w->userData);
			}
			else {
				PAINTSTRUCT ps;
				BeginPaint( hwnd, &ps );
				EndPaint( hwnd, &ps );
			}
            return 0;
            break;

        case WM_DESTROY:
			if(w->funcDestroy) w->funcDestroy(hwnd,w->userData);
            return 0;
            break;

    }
    
	if(w && w->funcOnWinEvent) w->funcOnWinEvent(hwnd, message, wparam, lparam);
    return DefWindowProc( hwnd, message, wparam, lparam );
}

//------------------------GETTER--------------------------------

HWND Window::getHWND() {
	return hMe;
}

UINT Window::getWidth() {
	return width;
}

UINT Window::getHeight() {
	return height;
}

UINT Window::getX() {
	return x;
}

UINT Window::getY() {
	return y;
}

void* Window::getUserData() {
	return userData;
}
