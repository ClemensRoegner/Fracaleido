////////////////////////////////////////////////////////////////////////////////////////
//
// Main rendering loop
//
////////////////////////////////////////////////////////////////////////////////////////

#include "DxPipeline.h"
#include <AntTweakBar.h>
#include "CoreMsg.h"
#include <time.h>

clock_t frameStart=clock(), frameEnd=0;

void DxLoop(HWND hwnd, void* data) {
	D3D11_MAPPED_SUBRESOURCE mapSR;
	dxCon->ClearState();

	//mapping new buffer data
	lightBuf.cameraPos = cam.pos;

	dxCon->Map(cbCam,0,D3D11_MAP_WRITE_DISCARD,0,&mapSR);
	*((camera*)mapSR.pData) = cam;
	dxCon->Unmap(cbCam,0);

	dxCon->Map(cbFracBuf,0,D3D11_MAP_WRITE_DISCARD,0,&mapSR);
	*((fracBuffer*)mapSR.pData) = fracBuf;
	dxCon->Unmap(cbFracBuf,0);

	dxCon->Map(cbLightBuf,0,D3D11_MAP_WRITE_DISCARD,0,&mapSR);
	*((lightBuffer*)mapSR.pData) = lightBuf;
	dxCon->Unmap(cbLightBuf,0);

	dxCon->Map(cbFracColorBuf,0,D3D11_MAP_WRITE_DISCARD,0,&mapSR);
	*((fracColorBuffer*)mapSR.pData) = fracColorBuf;
	dxCon->Unmap(cbFracColorBuf,0);

	for(UINT i=0;i<fracOps.size();i++) {
		if(fracOps.at(i) && !fracOps.at(i)->isToDelete()) {
			fracOps.at(i)->updateBuffer(dxCon);
		}
	}

	//rendering the fractal
	UINT pUAVInitialCounts = 0; //for us irrelevant
		
	dxCon->CSSetShader(dxCS,NULL,0);
	dxCon->CSSetConstantBuffers(0,1,&cbCam);
	dxCon->CSSetConstantBuffers(1,1,&cbFracBuf);
	dxCon->CSSetConstantBuffers(2,1,&cbFracColorBuf);
	for(UINT i=0;i<fracOps.size();i++) {
		if(fracOps.at(i)) {
			dxCon->CSSetConstantBuffers(FracOperator::minBufferNum+fracOps.at(i)->pos,1,&(fracOps.at(i)->cb));
			//cout << FracOperator::minBufferNum+fracOps.at(i)->pos << endl;
		}
	}
	dxCon->CSSetUnorderedAccessViews(0,1,&dxTPos_UAV,&pUAVInitialCounts);
	dxCon->CSSetUnorderedAccessViews(1,1,&dxTNormal_UAV,&pUAVInitialCounts);
	dxCon->CSSetUnorderedAccessViews(2,1,&dxTColor_UAV,&pUAVInitialCounts);
	dxCon->Dispatch((w+31)/32,(h+31)/32,1);

	dxCon->ClearState();

	//actual rendering
	float clearColor[4] = {0.0f,1.0f,0.0f,1.0f};

	dxCon->OMSetRenderTargets(1, &dxBackBuf, NULL);
	dxCon->RSSetViewports(1, dxBVP);

	dxCon->ClearRenderTargetView(dxBackBuf,clearColor);

	dxCon->VSSetShader(dxFS_VS, 0, 0);
	dxCon->PSSetShader(dxFS_PS, 0, 0);
	dxCon->PSSetConstantBuffers(0,1,&cbLightBuf);
	dxCon->PSSetShaderResources(0,1,&dxTPos_SRV);
	dxCon->PSSetShaderResources(1,1,&dxTNormal_SRV);
	dxCon->PSSetShaderResources(2,1,&dxTColor_SRV);
	dxCon->IASetInputLayout(dxFS_Layout);

	UINT stride = sizeof(vertex);
    UINT offset = 0;
	dxCon->IASetVertexBuffers(0, 1, &dxVBuf, &stride, &offset);
    dxCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    dxCon->Draw(6, 0);

	TwDraw();

	dxSC->Present(0, 0);

	frameEnd = clock() - frameStart;

	fpsSum += double(frameEnd)/double(CLOCKS_PER_SEC);
	fpsCount += 1.0f;
	if(fpsSum>fpsUpdateRate) {
		fpsAvg = 1.0f / (fpsSum / fpsCount);
		fpsCount = 0.0f;
		fpsSum = 0.0f;
	}

	frameStart = clock();
}