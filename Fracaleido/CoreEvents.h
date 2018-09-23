////////////////////////////////////////////////////////////////////////////////////////
//
// Capsules for user input
//
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Windows.h>

struct KeyEvent {
	bool keyUp;
	bool keyDown;
	USHORT key;
};

enum class MouseButtonChange : byte { NOCHANGE = 0, DOWN = 1, UP = 2};

struct MouseEvent {
	bool posAbsolute;
	POINT p;
	MouseButtonChange left;
	MouseButtonChange middle;
	MouseButtonChange right;
	INT mouseWheelDelta;
};

struct MYRAWINPUT
{
    RAWINPUTHEADER header;
	UINT pad1, pad2;  // Gruesse an die Idioten von Microsoft
	union {
    RAWMOUSE mouse;
    RAWKEYBOARD keyboard;
    RAWHID hid;
	} data;
};
