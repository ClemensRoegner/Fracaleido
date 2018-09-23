////////////////////////////////////////////////////////////////////////////////////////
//
// Main Function
//
////////////////////////////////////////////////////////////////////////////////////////

//for mem leak detection
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

//general includes
#include <stdio.h>
#include <iostream>
#include <conio.h>
#include <string>
#include <windows.h>

//my includes
#include "CoreMsg.h"
#include "CoreWindow.h"

#include "DxPipeline.h"

using namespace std; 

LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam );

int main(int argc, char* argv[]) {
	
	{
		//get the consoles window handles
		HWND hwndC = GetConsoleWindow() ; 
		HINSTANCE hInstC = GetModuleHandle( 0 ) ;

		printf("creating windows \n");

		//spawn directx window
		DefWindow winDef;
		winDef.hinst = hInstC;
		winDef.parent = hwndC;
		winDef.title = "main window";
		winDef.width = 1024;
		winDef.height = 768;
		winDef.x = 500;
		winDef.y = 200;

		winDef.funcInit = DxInit;
		winDef.funcResize = DxResize;
		winDef.funcLoop = DxLoop;
		winDef.funcDestroy = DxDestroy;
		winDef.funcOnKey = DxOnKey;
		winDef.funcOnMouse = DxOnMouse;
		winDef.funcOnWinEvent = DxOnWinEvent;

		Window mainWin( winDef );

		mainWin.showWindow();

		Window::EnterLoop();
	}

	printf("everything ended \n");
	_CrtDumpMemoryLeaks();

	Sleep(1000);
	return 0;
}

