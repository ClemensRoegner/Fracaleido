////////////////////////////////////////////////////////////////////////////////////////
//
// How to handle resizing the window shoudl be handled
//
////////////////////////////////////////////////////////////////////////////////////////

#include "DxPipeline.h"
#include "CoreMacros.h"
#include "GraphicMacros.h"

void DxResize(HWND hwnd, void* data) {

	if(dxSC) { //we actually have something to resize
		HRESULT hr;

		dxCon->ClearState();
		dxCon->OMSetRenderTargets(0,0,0);
		SAFE_RELEASE(dxBackBuf);
		dxSC->ResizeBuffers(0,0,0,DXGI_FORMAT_R8G8B8A8_UNORM,0); //auto fit client rect
	
		//rendertarget
		ID3D11Texture2D *backBuf;
		dxSC->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuf);

		dxDev->CreateRenderTargetView(backBuf, NULL, &dxBackBuf);
		D3D11_TEXTURE2D_DESC td; //get the width and height
		backBuf->GetDesc(&td);
		w = td.Width;
		h = td.Height;

		SAFE_RELEASE(backBuf);

		dxBVP->Width = w;
		dxBVP->Height = h;

		//---------------------------------------------------
		// Fraktal texture & compute shaders
		SAFE_RELEASE(dxTPos_UAV);
		SAFE_RELEASE(dxTPos_SRV);
		SAFE_RELEASE(dxTPos);
		SAFE_RELEASE(dxTNormal_UAV);
		SAFE_RELEASE(dxTNormal_SRV);
		SAFE_RELEASE(dxTNormal);
		SAFE_RELEASE(dxTColor_UAV);
		SAFE_RELEASE(dxTColor_SRV);
		SAFE_RELEASE(dxTColor);

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

		//---------------------------------------------------
		// GUI - tweak bar
		TwWindowSize(w, h);

		int FracGenBarSize[2] = {200, 220};
		TwSetParam(tbFracGen, NULL, "size", TW_PARAM_INT32, 2, FracGenBarSize);
		int FracGenBarPos[2] = {20, 20};
		TwSetParam(tbFracGen, NULL, "position", TW_PARAM_INT32, 2, FracGenBarPos);

		int FracForBarSize[2] = {200, h - 280};
		TwSetParam(tbFracFor, NULL, "size", TW_PARAM_INT32, 2, FracForBarSize);
		int FracForBarPos[2] = {20, 260};
		TwSetParam(tbFracFor, NULL, "position", TW_PARAM_INT32, 2, FracForBarPos);

		int lightBarSize[2] = {200, 220};
		TwSetParam(tbLight, NULL, "size", TW_PARAM_INT32, 2, lightBarSize);
		int lightBarPos[2] = {w - 220, 20};
		TwSetParam(tbLight, NULL, "position", TW_PARAM_INT32, 2, lightBarPos);

		int colorBarSize[2] = {200,  h - 280};
		TwSetParam(tbFracColor, NULL, "size", TW_PARAM_INT32, 2, colorBarSize);
		int colorBarPos[2] = {w - 220, 260};
		TwSetParam(tbFracColor, NULL, "position", TW_PARAM_INT32, 2, colorBarPos);

		//---------------------------------------------------
		// Buffer
		cam.w = float(w);
		cam.h = float(h);
		cam.calcRatio();
		cam.angle = radians(30.0f);
		cam.calcSkew();
	}
}