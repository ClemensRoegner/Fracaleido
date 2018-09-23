////////////////////////////////////////////////////////////////////////////////////////
//
// Inits DX rendering and GUI (latter based on Tweakbar API)
//
////////////////////////////////////////////////////////////////////////////////////////

#include "DxPipeline.h"
#include "CoreMacros.h"
#include "GraphicMacros.h"
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include "FracPlaneFold.h"
#include "FracRotation.h"
#include "FracBoxFold.h"
#include "FracSphereFold.h"
#include "FracCylinderFold.h"
#include "DxCompileFractal.h"

using namespace std;

//TweakBar function calls = GUI
void TW_CALL SetFracOffPos(const void *value, void * /*clientData*/) {
	fracBuf.fracOffsetFromPos = (*static_cast<const bool *>(value))?1:-1;
}

void TW_CALL GetFracOffPos(void *value, void * /*clientData*/) {
	*static_cast<bool *>(value) = (fracBuf.fracOffsetFromPos>=0)?true:false;
}

void TW_CALL SetFracSimpleColor(const void *value, void * /*clientData*/) {
	fracColorBuf.simpleColor.w = (*static_cast<const bool *>(value))?1.0f:-1.0f;
}

void TW_CALL GetFracSimpleColor(void *value, void * /*clientData*/) {
	*static_cast<bool *>(value) = (fracColorBuf.simpleColor.w>=0.0f)?true:false;
}

void TW_CALL RecompileFractal(void * clientData) {
	if(!compileFractal()) {
		cout << "COMPILE ERROR" << endl;
		return;
	}
}

void TW_CALL AddPlaneFold(void * clientData) {
	FracPlaneFold* f = new FracPlaneFold(dxDev);
	f->setPos(getMaxPosOfOperators()+1);
	fracOps.push_back(f);

	f->addControlsToBar(tbFracFor);
}

void TW_CALL AddRotation(void * clientData) {
	FracRotation* f = new FracRotation(dxDev);
	f->setPos(getMaxPosOfOperators()+1);
	fracOps.push_back(f);

	f->addControlsToBar(tbFracFor);
}

void TW_CALL AddBoxFold(void * clientData) {
	FracBoxFold* f = new FracBoxFold(dxDev);
	f->setPos(getMaxPosOfOperators()+1);
	fracOps.push_back(f);

	f->addControlsToBar(tbFracFor);
}

void TW_CALL AddSphereFold(void * clientData) {
	FracSphereFold* f = new FracSphereFold(dxDev);
	f->setPos(getMaxPosOfOperators()+1);
	fracOps.push_back(f);

	f->addControlsToBar(tbFracFor);
}

void TW_CALL AddCylinderFold(void * clientData) {
	FracCylinderFold* f = new FracCylinderFold(dxDev);
	f->setPos(getMaxPosOfOperators()+1);
	fracOps.push_back(f);

	f->addControlsToBar(tbFracFor);
}

void TW_CALL SetTetra(void * clientData) {
	for(UINT i=0; i<fracOps.size();i++) {
		fracOps.at(i)->removeControlsFromBar();
		fracOps.at(i)->setToDelete();
	}

	FracPlaneFold* f = NULL;

	f = new FracPlaneFold(dxDev,float3( 1, 1, 0)); f->setPos(getMaxPosOfOperators()+1); f->normalizePlane(); fracOps.push_back(f); f->addControlsToBar(tbFracFor);
	f = new FracPlaneFold(dxDev,float3( 1, 0, 1)); f->setPos(getMaxPosOfOperators()+1); f->normalizePlane(); fracOps.push_back(f); f->addControlsToBar(tbFracFor);
	f = new FracPlaneFold(dxDev,float3( 0, 1, 1)); f->setPos(getMaxPosOfOperators()+1); f->normalizePlane(); fracOps.push_back(f); f->addControlsToBar(tbFracFor);

	if(!compileFractal()) {
		return;
	}

	fracBuf.fracOffsetFromPos = -1;
}

void TW_CALL SetBox(void * clientData) {
	for(UINT i=0; i<fracOps.size();i++) {
		fracOps.at(i)->removeControlsFromBar();
		fracOps.at(i)->setToDelete();
	}

	FracBoxFold* bf = new FracBoxFold(dxDev); bf->setPos(getMaxPosOfOperators()+1); fracOps.push_back(bf); bf->addControlsToBar(tbFracFor);
	FracSphereFold* sf = new FracSphereFold(dxDev); sf->setPos(getMaxPosOfOperators()+1); fracOps.push_back(sf); sf->addControlsToBar(tbFracFor);

	if(!compileFractal()) {
		return; 
	}

	fracBuf.fracOffsetFromPos = 1;
}

//do the actual init
void DxInit(HWND hwnd, void* data) {
	HRESULT hr;

    DXGI_SWAP_CHAIN_DESC scd;    
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    scd.BufferCount = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hwnd;
    scd.SampleDesc.Count = 4;
    scd.Windowed = TRUE;

	hr = D3D11CreateDeviceAndSwapChain(NULL,D3D_DRIVER_TYPE_HARDWARE,NULL,NULL,NULL,NULL,D3D11_SDK_VERSION,&scd,&dxSC,&dxDev,NULL,&dxCon);
	CHECK_HR(hr);

	//rendertarget
	ID3D11Texture2D *backBuf;
    dxSC->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuf);

	hr = dxDev->CreateRenderTargetView(backBuf, NULL, &dxBackBuf);
	CHECK_HR(hr);

	D3D11_TEXTURE2D_DESC td; //get the width and height
	backBuf->GetDesc(&td);
	w = td.Width;
	h = td.Height;

	SAFE_RELEASE(backBuf);

	//viewport
	dxBVP = new D3D11_VIEWPORT();
	ZeroMemory(dxBVP, sizeof(D3D11_VIEWPORT));

    dxBVP->TopLeftX = 0;
    dxBVP->TopLeftY = 0;
	dxBVP->Width = w;
	dxBVP->Height = h;


	//Shader
	ID3DBlob* pErrorBlob = NULL;
	ID3D10Blob *VS, *PS;
	hr = D3DCompileFromFile(L"FullScreen.hlsl", 0, 0, "FS_VS", "vs_5_0", 0, 0, &VS, &pErrorBlob);
	if ( FAILED(hr) )
    {
        if ( pErrorBlob ) OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );

        SAFE_RELEASE( pErrorBlob );
        SAFE_RELEASE( VS );    

        return;
    }    
	hr = D3DCompileFromFile(L"FullScreen.hlsl", 0, 0, "FS_PS", "ps_5_0", 0, 0, &PS, &pErrorBlob);
	if ( FAILED(hr) )
    {
        if ( pErrorBlob ) OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );

        SAFE_RELEASE( pErrorBlob );
        SAFE_RELEASE( PS );    

        return;
    }   

	dxDev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &dxFS_VS);
	dxDev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &dxFS_PS);

	D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT	, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT		, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

	dxDev->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &dxFS_Layout);

	//Buffer

	vertex vFullScreen[] =
	{
		{ float4(-1.0f, 1.0f, 0.0f, 1.0f), float2(0.0f, 0.0f) },
		{ float4(1.0f, 1.0f, 0.0f, 1.0f), float2(1.0f, 0.0f) },
		{ float4(-1.0f,-1.0f, 0.0f, 1.0f), float2(0.0f, 1.0f) },
		{ float4(-1.0f,-1.0f, 0.0f, 1.0f), float2(0.0f, 1.0f) },
		{ float4(1.0f, 1.0f, 0.0f, 1.0f), float2(1.0f, 0.0f) },
		{ float4(1.0f,-1.0f, 0.0f, 1.0f), float2(1.0f, 1.0f) },
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(vertex) * 6;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	dxDev->CreateBuffer(&bd, NULL, &dxVBuf);

	D3D11_MAPPED_SUBRESOURCE ms;
	dxCon->Map(dxVBuf, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	memcpy(ms.pData, vFullScreen, sizeof(vFullScreen));
	dxCon->Unmap(dxVBuf, NULL); 

	//---------------------------------------------------
	// GUI - tweak bar

	TwInit(TW_DIRECT3D11, dxDev);
	TwWindowSize(w, h);

	//left side - fractal

	tbFracGen = TwNewBar("Fractal General");
    TwDefine(" GLOBAL help='This is the Tweakbar to adjust the general fractal settings.' ");
    int FracGenBarSize[2] = {200, 220};
    TwSetParam(tbFracGen, NULL, "size", TW_PARAM_INT32, 2, FracGenBarSize);
	int FracGenBarPos[2] = {20, 20};
    TwSetParam(tbFracGen, NULL, "position", TW_PARAM_INT32, 2, FracGenBarPos);

	TwAddVarRW(tbFracGen, "fracScale", TW_TYPE_FLOAT, &(fracBuf.fracScale), "step=0.05 label='Fractal Scale'");
	TwAddVarCB(tbFracGen, "fracOffsetFromPos", TW_TYPE_BOOL32, SetFracOffPos, GetFracOffPos, NULL, "key=o label='Offset via Position'");
	TwAddVarRW(tbFracGen, "fracOffsetX", TW_TYPE_FLOAT, &(fracBuf.fracOffset.x), "step=0.05 label='Fractal Offset - X'");
	TwAddVarRW(tbFracGen, "fracOffsetY", TW_TYPE_FLOAT, &(fracBuf.fracOffset.y), "step=0.05 label='Fractal Offset - Y'");
	TwAddVarRW(tbFracGen, "fracOffsetZ", TW_TYPE_FLOAT, &(fracBuf.fracOffset.z), "step=0.05 label='Fractal Offset - Z'");
	TwAddButton(tbFracGen, "presetTetra", SetTetra, NULL, " label='Preset: Sierpinski' ");
	TwAddButton(tbFracGen, "presetBox", SetBox, NULL, " label='Preset: MandelBox' ");

	tbFracFor = TwNewBar("Fractal Operators");
    TwDefine(" GLOBAL help='This is the Tweakbar to adjust the general fractal settings.' ");
    int FracForBarSize[2] = {200, h - 280};
    TwSetParam(tbFracFor, NULL, "size", TW_PARAM_INT32, 2, FracForBarSize);
	int FracForBarPos[2] = {20, 260};
    TwSetParam(tbFracFor, NULL, "position", TW_PARAM_INT32, 2, FracForBarPos);

	TwAddButton(tbFracFor, "Compile", RecompileFractal, NULL, " label='Compile' ");
	TwAddButton(tbFracFor, "AddPlaneFold", AddPlaneFold, NULL, " label='Add Plane Fold' ");
	TwAddButton(tbFracFor, "AddRotation", AddRotation, NULL, " label='Add Rotation' ");
	TwAddButton(tbFracFor, "AddBoxFold", AddBoxFold, NULL, " label='Add Box Fold' ");
	TwAddButton(tbFracFor, "AddSphereFold", AddSphereFold, NULL, " label='Add Sphere Fold' ");
	//TwAddButton(tbFracFor, "AddCylinderFold", AddCylinderFold, NULL, " label='Add Cylinder Fold' "); //cylinder fold not yet in

	//right side - color and such

	tbLight = TwNewBar("Light");
    TwDefine(" GLOBAL help='This is the Tweakbar to adjust the light settings.' ");
    int lightBarSize[2] = {200, 220};
    TwSetParam(tbLight, NULL, "size", TW_PARAM_INT32, 2, lightBarSize);
	int lightBarPos[2] = {w - 220, 20};
    TwSetParam(tbLight, NULL, "position", TW_PARAM_INT32, 2, lightBarPos);

	TwAddVarRW(tbLight, "Color", TW_TYPE_COLOR3F, &(lightBuf.lightColor), "colormode=rgb");
	TwAddVarRW(tbLight, "Direction", TW_TYPE_DIR3F, &(lightBuf.lightDir), "opened=true axisz=-z showval=true");

	tbFracColor = TwNewBar("Color");
    TwDefine(" GLOBAL help='This is the Tweakbar to adjust the color settings.' ");
    int colorBarSize[2] = {200,  h - 280};
    TwSetParam(tbFracColor, NULL, "size", TW_PARAM_INT32, 2, colorBarSize);
	int colorBarPos[2] = {w - 220, 260};
    TwSetParam(tbFracColor, NULL, "position", TW_PARAM_INT32, 2, colorBarPos);

	TwAddVarRW(tbFracColor, "PhongAmbient", TW_TYPE_FLOAT, &(fracColorBuf.phong.x), "step=0.05 min=0.0 max=1.0 label='Phong Ambient'");
	TwAddVarRW(tbFracColor, "PhongDiffuse", TW_TYPE_FLOAT, &(fracColorBuf.phong.y), "step=0.05 min=0.0 max=1.0 label='Phong Diffuse'");
	TwAddVarRW(tbFracColor, "PhongSpecular", TW_TYPE_FLOAT, &(fracColorBuf.phong.z), "step=0.05 min=0.0 max=1.0 label='Phong Specular'");
	TwAddVarRW(tbFracColor, "PhongPower", TW_TYPE_FLOAT, &(fracColorBuf.phong.w), "step=1.0 label='Phong Power'");
	TwAddVarCB(tbFracColor, "UseSimple", TW_TYPE_BOOL32, SetFracSimpleColor, GetFracSimpleColor, NULL, "key=o label='use Simple Color'");
	TwAddVarRW(tbFracColor, "Simple Color", TW_TYPE_COLOR3F, &(fracColorBuf.simpleColor), "colormode=rgb");
	TwAddVarRW(tbFracColor, "Trap Color #1", TW_TYPE_COLOR3F, &(fracColorBuf.trapColor1), "colormode=rgb");
	TwAddVarRW(tbFracColor, "Trap Color #2", TW_TYPE_COLOR3F, &(fracColorBuf.trapColor2), "colormode=rgb");
	TwAddVarRW(tbFracColor, "Trap Color #3", TW_TYPE_COLOR3F, &(fracColorBuf.trapColor3), "colormode=rgb");

	
	//Other Bar

	tbFracData = TwNewBar("Data");
	TwDefine(" GLOBAL help='This is the Tweakbar for showing rendering Data.' ");
    int FracDataBarSize[2] = {200, 180};
    TwSetParam(tbFracData, NULL, "size", TW_PARAM_INT32, 2, FracDataBarSize);
	int FracDataBarPos[2] = {240, 20};
    TwSetParam(tbFracData, NULL, "position", TW_PARAM_INT32, 2, FracDataBarPos);
	TwDefine(" Data iconified=true ");

	TwAddVarRO(tbFracData, "CameraPosX", TW_TYPE_FLOAT, &(cam.pos.x), " label='Camera Pos X'");
	TwAddVarRO(tbFracData, "CameraPosY", TW_TYPE_FLOAT, &(cam.pos.y), " label='Camera Pos Y'");
	TwAddVarRO(tbFracData, "CameraPosZ", TW_TYPE_FLOAT, &(cam.pos.z), " label='Camera Pos Z'");
	TwAddVarRO(tbFracData, "CameraView", TW_TYPE_DIR3F, &(cam.dir), " label='Camera View Dir'");
	TwAddVarRO(tbFracData, "CameraUp", TW_TYPE_DIR3F, &(cam.up), " label='Camera Up Dir'");
	TwAddVarRO(tbFracData, "CameraSide", TW_TYPE_DIR3F, &(cam.side), " label='Camera Side Dir'");
	TwAddVarRO(tbFracData, "FPS", TW_TYPE_FLOAT, &(fpsAvg), " label='Average Framerate'");

	//---------------------------------------------------
	// Fraktal texture & compute shaders

	D3D11_TEXTURE2D_DESC desc;
    desc.Width = w;
    desc.Height = h;
	desc.MipLevels = 1;
    desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = 0;
	desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
	
	hr = dxDev->CreateTexture2D( &desc, NULL, &dxTPos );
	hr = dxDev->CreateTexture2D( &desc, NULL, &dxTNormal );
	hr = dxDev->CreateTexture2D( &desc, NULL, &dxTColor );

	D3D11_SHADER_RESOURCE_VIEW_DESC srd;
	srd.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srd.Texture2D.MipLevels = 1;
	srd.Texture2D.MostDetailedMip = 0;

	hr = dxDev->CreateShaderResourceView(dxTPos,&srd,&dxTPos_SRV);
	hr = dxDev->CreateShaderResourceView(dxTNormal,&srd,&dxTNormal_SRV);
	hr = dxDev->CreateShaderResourceView(dxTColor,&srd,&dxTColor_SRV);

	D3D11_UNORDERED_ACCESS_VIEW_DESC descUAV;
    descUAV.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	descUAV.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	descUAV.Texture2D.MipSlice = 0;

	hr = dxDev->CreateUnorderedAccessView(dxTPos,&descUAV,&dxTPos_UAV);
	hr = dxDev->CreateUnorderedAccessView(dxTNormal,&descUAV,&dxTNormal_UAV);
	hr = dxDev->CreateUnorderedAccessView(dxTColor,&descUAV,&dxTColor_UAV);

	//temp frac ops

	FracPlaneFold* f = NULL;

	f = new FracPlaneFold(dxDev,float3( 1, 1, 0)); f->setPos(0); f->normalizePlane(); fracOps.push_back(f);
	f = new FracPlaneFold(dxDev,float3( 1, 0, 1)); f->setPos(1); f->normalizePlane(); fracOps.push_back(f);
	f = new FracPlaneFold(dxDev,float3( 0, 1, 1)); f->setPos(2); f->normalizePlane(); fracOps.push_back(f);

	//end


	if(!compileFractal()) {
		return;
	}

	for(UINT i=0; i<fracOps.size();i++) {
		fracOps.at(i)->addControlsToBar(tbFracFor);
	}

	//---------------------------------------------------
	// Fraktal parameters

	cam.pos = float4(0.0f,0.0f,-10.0f,1.0f);
	cam.dir = float4(0.0f,0.0f,1.0f,0.0f);
	cam.up = float4(0.0f,1.0f,0.0f,0.0f);
	cam.calcSideVector();
	cam.w = float(w);
	cam.h = float(h);
	cam.calcRatio();
	cam.angle = radians(30.0f);
	cam.calcSkew();

	fracBuf.fracOffsetFromPos = -1;
	fracBuf.fracOffset = float3(-1,-1,-1);
	fracBuf.fracScale = 2.0f;

	lightBuf.lightDir = normalize(float4(20,18,16,0)*-1.0f);
	lightBuf.lightColor = float4(0.8,0.8,1.0,1.0);
	lightBuf.cameraPos = cam.pos;

	fracColorBuf.phong = float4(0.2f,0.4f,0.4f,6.0f);
	fracColorBuf.simpleColor = float4(1,1,1,1);
	fracColorBuf.trapColor1 = float4(1,0,0,1);
	fracColorBuf.trapColor2 = float4(0,1,0,1);
	fracColorBuf.trapColor3 = float4(0,0,1,1);

	//---------------------------------------------------
	// constant buffers
	D3D11_BUFFER_DESC bdsc;
    bdsc.Usage = D3D11_USAGE_DYNAMIC;
    bdsc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bdsc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bdsc.MiscFlags = 0;
	bdsc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

    bdsc.ByteWidth = ((sizeof( camera ) + 15)/16)*16; //must be multiple of 16
	InitData.pSysMem = &cam;

	hr = dxDev->CreateBuffer(&bdsc,&InitData, &cbCam);

	bdsc.ByteWidth = ((sizeof( fracBuffer ) + 15)/16)*16; //must be multiple of 16
	InitData.pSysMem = &fracBuf;

	hr = dxDev->CreateBuffer(&bdsc,&InitData, &cbFracBuf);
	
	bdsc.ByteWidth = ((sizeof( lightBuffer ) + 15)/16)*16; //must be multiple of 16
	InitData.pSysMem = &lightBuf;

	hr = dxDev->CreateBuffer(&bdsc,&InitData, &cbLightBuf);
	
	bdsc.ByteWidth = ((sizeof( fracColorBuffer ) + 15)/16)*16; //must be multiple of 16
	InitData.pSysMem = &fracColorBuf;

	hr = dxDev->CreateBuffer(&bdsc,&InitData, &cbFracColorBuf);

	SAFE_RELEASE( pErrorBlob );
	SAFE_RELEASE( VS );
	SAFE_RELEASE( PS );  
}