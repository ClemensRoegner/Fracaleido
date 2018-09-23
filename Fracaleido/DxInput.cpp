////////////////////////////////////////////////////////////////////////////////////////
//
// Peripheral input
//
////////////////////////////////////////////////////////////////////////////////////////

#include "DxPipeline.h"
#include "CoreMsg.h"

#define MOVE_SPEED 0.1f
#define LOOK_SPEED 1.0f

float speedMove = MOVE_SPEED;

void DxOnKey(HWND hwnd, KeyEvent& e, void* data) {
	if(e.keyDown) {
		if(e.key == 87) { //W
			cam.pos += cam.dir * speedMove; 
		}
		if(e.key == 83) { //S
			cam.pos -= cam.dir * speedMove; 
		}
		if(e.key == 65) { //A
			cam.pos -= cam.side * speedMove; 
		}
		if(e.key == 68) { //D
			cam.pos += cam.side * speedMove; 
		}
		if(e.key == 32) { //SPACE
			cam.pos += cam.up * speedMove; 
		}
		if(e.key == 17) { //CTRL (left)
			cam.pos -= cam.up * speedMove; 
		}
		if(e.key == VK_RETURN) { //ENTER
			saveScreen = true;
		}
	}
}

bool mouseMove = false;

void DxOnMouse(HWND hwnd, MouseEvent& e, void* data) {

	if(e.right == MouseButtonChange::DOWN) {
		mouseMove = true;
	}
	if(e.right == MouseButtonChange::NOCHANGE) {
		if(mouseMove && !e.posAbsolute){

			float3 up = float3(cam.up.x,cam.up.y,cam.up.z);
			float3 side = float3(cam.side.x,cam.side.y,cam.side.z);

			mat4 rL = glm::rotate(glm::mat4(),-LOOK_SPEED,up);
			mat4 rR = glm::rotate(glm::mat4(), LOOK_SPEED,up);
			mat4 rD = glm::rotate(glm::mat4(),-LOOK_SPEED,side);
			mat4 rU = glm::rotate(glm::mat4(), LOOK_SPEED,side);
			
			if(e.p.x == -1) {
				cam.dir = rL * cam.dir;
				cam.side = rL * cam.side;
			}
			if(e.p.x ==  1) {
				cam.dir = rR * cam.dir;
				cam.side = rR * cam.side;
			}
			
			if(e.p.y == -1) {
				cam.dir = rD * cam.dir;
				cam.up = rD * cam.up;
			}
			if(e.p.y ==  1) {
				cam.dir = rU * cam.dir;
				cam.up = rU * cam.up;
			}
		}
	}
	if(e.right == MouseButtonChange::UP) {
		mouseMove=false;
	}

	if(e.mouseWheelDelta!=0) {
		if(e.mouseWheelDelta<0)
			speedMove /= 1.1f;
		else
			speedMove *= 1.1f;
	}
}

void DxOnWinEvent(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	TwEventWin(wnd, msg, wParam, lParam);
}