////////////////////////////////////////////////////////////////////////////////////////
//
// Cpasules for Window handling
//
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <conio.h>
#include <string>
#include <windows.h>
#include "CoreEvents.h"

using namespace std;

class winClassCreationEx: public exception
{
  virtual const char* what() const throw()
  {
    return "Window Class construction failed";
  }
};

struct DefWindow { //struct to generate the window - i like it that way
	void (*funcInit)(HWND,void*);
	void (*funcLoop)(HWND,void*);
	void (*funcResize)(HWND,void*);
	void (*funcPaint)(HWND,void*);
	void (*funcOnKey)(HWND,KeyEvent& e, void* userData);
	void (*funcOnMouse)(HWND,MouseEvent& e, void* userData);
	void (*funcOnWinEvent)(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void (*funcDestroy)(HWND,void*);
	
	HINSTANCE hinst;
	HWND parent;
	string title;
	UINT width; 
	UINT height; 
	UINT x;
	UINT y;
	void* userData; //set to this (Window that is), if it is null

	DefWindow();
};

class Window {
	public:
		Window(DefWindow& p);
		~Window();

		void showWindow();
		void closeWindow();

		//getters
		HWND getHWND();
		UINT getWidth();
		UINT getHeight();
		UINT getX();
		UINT getY();
		void* getUserData();

		//statics
		static int closeAllWindows();
		static void EnterLoop();

	private:
		void (*funcInit)(HWND,void*);
		void (*funcLoop)(HWND,void*);
		void (*funcResize)(HWND,void*);
		void (*funcPaint)(HWND,void*);
		void (*funcOnKey)(HWND,KeyEvent& e, void* userData);
		void (*funcOnMouse)(HWND,MouseEvent& e, void* userData);
		void (*funcOnWinEvent)(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void (*funcDestroy)(HWND,void*);

		HWND hMe;
		
		string title;
		UINT width; 
		UINT height; 
		UINT x;
		UINT y;
		void* userData;

		HINSTANCE hinst;
		HWND parent;

		bool isShown;

		static LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam );
};

