////////////////////////////////////////////////////////////////////////////////////////
//
// Implementation of the globals used for rendering
//
////////////////////////////////////////////////////////////////////////////////////////

#include "DxPipeline.h"

UINT w = 0;
UINT h = 0;

ID3D11Device* dxDev = NULL;
ID3D11DeviceContext* dxCon = NULL;
IDXGISwapChain* dxSC = NULL;
ID3D11RenderTargetView* dxBackBuf = NULL;
D3D11_VIEWPORT* dxBVP = NULL;
ID3D11Buffer* dxVBuf = NULL;
ID3D11VertexShader* dxFS_VS = NULL;
ID3D11PixelShader* dxFS_PS = NULL;  
ID3D11InputLayout* dxFS_Layout = NULL;

ID3D11Texture2D* dxTPos = NULL;
ID3D11ShaderResourceView* dxTPos_SRV = NULL;
ID3D11UnorderedAccessView* dxTPos_UAV = NULL;
ID3D11Texture2D* dxTNormal = NULL;
ID3D11ShaderResourceView* dxTNormal_SRV = NULL;
ID3D11UnorderedAccessView* dxTNormal_UAV = NULL;
ID3D11Texture2D* dxTColor = NULL;
ID3D11ShaderResourceView* dxTColor_SRV = NULL;
ID3D11UnorderedAccessView* dxTColor_UAV = NULL;

ID3D11ComputeShader* dxCS = NULL;

 camera cam = camera();
 ID3D11Buffer* cbCam = NULL;

fracBuffer fracBuf = fracBuffer();
ID3D11Buffer* cbFracBuf = NULL;

lightBuffer lightBuf = lightBuffer();
ID3D11Buffer* cbLightBuf = NULL;

fracColorBuffer fracColorBuf = fracColorBuffer();
ID3D11Buffer* cbFracColorBuf = NULL;

TwBar *tbLight = NULL;
TwBar *tbFracGen = NULL;
TwBar *tbFracFor = NULL;
TwBar *tbFracData = NULL;
TwBar *tbFracColor = NULL;

float fpsSum = 0.0f;
float fpsAvg = 0.0f;
float fpsCount = 0.0f;
float fpsUpdateRate = 0.2f;
bool saveScreen = false;