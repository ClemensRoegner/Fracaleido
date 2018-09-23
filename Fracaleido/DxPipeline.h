////////////////////////////////////////////////////////////////////////////////////////
//
// Key structures and global resources used for rendering
//
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include "glm.h"
#include <AntTweakBar.h>
#include <vector>
#include "FracOperator.h"

// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

#include "CoreEvents.h"

struct vertex {
	float4 p;
	float2 t;
};

struct camera {
	float4 pos;
	float4 dir;
	float4 up;
	float4 side; //must be left side
	float angle;
	float w;
	float h;
	float ratio;
	float skew;
		
	void calcSideVector() {
		side = float4(normalize(cross(float3(up.x,up.y,up.z), float3(dir.x,dir.y,dir.z))),0.0);
		up = float4(normalize(cross(float3(dir.x,dir.y,dir.z), float3(side.x,side.y,side.z))),0.0);
	};
	void calcRatio() {
		ratio = w/h;
	};
	void calcSkew() {
		skew = tan(angle);
	};
};

struct fracBuffer {
	float fracScale;
	float3 fracOffset;
	INT fracOffsetFromPos;
};

struct lightBuffer {
	float4 lightDir;
	float4 lightColor;
	float4 cameraPos;
};

struct fracColorBuffer {
	float4 phong;
	float4 simpleColor;
	float4 trapColor1;
	float4 trapColor2;
	float4 trapColor3;
};

extern UINT w;
extern UINT h;

extern ID3D11Device* dxDev;
extern ID3D11DeviceContext* dxCon;
extern IDXGISwapChain* dxSC;
extern ID3D11RenderTargetView* dxBackBuf;
extern D3D11_VIEWPORT* dxBVP;
extern ID3D11Buffer* dxVBuf; 
extern ID3D11VertexShader* dxFS_VS;
extern ID3D11PixelShader* dxFS_PS;  
extern ID3D11InputLayout* dxFS_Layout;
extern ID3D11ComputeShader* dxCS;

extern ID3D11Texture2D* dxTPos;
extern ID3D11ShaderResourceView* dxTPos_SRV;
extern ID3D11UnorderedAccessView* dxTPos_UAV;
extern ID3D11Texture2D* dxTNormal;
extern ID3D11ShaderResourceView* dxTNormal_SRV;
extern ID3D11UnorderedAccessView* dxTNormal_UAV;
extern ID3D11Texture2D* dxTColor;
extern ID3D11ShaderResourceView* dxTColor_SRV;
extern ID3D11UnorderedAccessView* dxTColor_UAV;

extern camera cam;
extern ID3D11Buffer* cbCam;

extern fracBuffer fracBuf;
extern ID3D11Buffer* cbFracBuf;

extern lightBuffer lightBuf;
extern ID3D11Buffer* cbLightBuf;

extern fracColorBuffer fracColorBuf;
extern ID3D11Buffer* cbFracColorBuf;

extern TwBar *tbLight;
extern TwBar *tbFracGen;
extern TwBar *tbFracFor;
extern TwBar *tbFracData;
extern TwBar *tbFracColor;

extern float fpsSum;
extern float fpsAvg;
extern float fpsCount;
extern float fpsUpdateRate;
extern bool saveScreen;

void DxInit(HWND hwnd, void* data);
void DxLoop(HWND hwnd, void* data);
void DxResize(HWND hwnd, void* data);
void DxOnKey(HWND hwnd, KeyEvent& e, void* data);
void DxOnMouse(HWND hwnd, MouseEvent& e, void* data);
void DxOnWinEvent(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
void DxDestroy(HWND hwnd, void* data);