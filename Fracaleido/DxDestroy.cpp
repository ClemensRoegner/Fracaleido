////////////////////////////////////////////////////////////////////////////////////////
//
// Destroying all resources needed for rendering
//
////////////////////////////////////////////////////////////////////////////////////////

#include "DxPipeline.h"
#include "CoreMacros.h"
#include "GraphicMacros.h"

void DxDestroy(HWND hwnd, void* data) { //release everything
	for(UINT i=0; i < fracOps.size(); i++) {
		if(fracOps.at(i)) {
			fracOps.at(i)->removeControlsFromBar();
			SAFE_DELETE(fracOps.at(i));
		}
	}
	fracOps.clear();

	TwTerminate();

	SAFE_RELEASE(cbCam);
	SAFE_RELEASE(cbFracBuf);
	SAFE_RELEASE(cbLightBuf);
	SAFE_RELEASE(cbFracColorBuf);

	SAFE_RELEASE(dxTPos_UAV);
	SAFE_RELEASE(dxTPos_SRV);
	SAFE_RELEASE(dxTPos);
	SAFE_RELEASE(dxTNormal_UAV);
	SAFE_RELEASE(dxTNormal_SRV);
	SAFE_RELEASE(dxTNormal);
	SAFE_RELEASE(dxTColor_UAV);
	SAFE_RELEASE(dxTColor_SRV);
	SAFE_RELEASE(dxTColor);

	SAFE_RELEASE(dxVBuf);
	SAFE_RELEASE(dxFS_VS);
	SAFE_RELEASE(dxFS_PS);  
	SAFE_RELEASE(dxFS_Layout);
	SAFE_RELEASE(dxCS);

	SAFE_DELETE(dxBVP);
	SAFE_RELEASE(dxSC);
    SAFE_RELEASE(dxDev);
    SAFE_RELEASE(dxCon);
}