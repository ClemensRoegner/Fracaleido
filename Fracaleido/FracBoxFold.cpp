////////////////////////////////////////////////////////////////////////////////////////
//
// Box Fold Operator
//
////////////////////////////////////////////////////////////////////////////////////////

#include "FracBoxFold.h"
#include "GraphicMacros.h"

FracBoxFold::FracBoxFold(ID3D11Device* dxDev, float foldingLimit) : FracOperator(), lastBar(NULL) {
	buf.fop_foldingLimit = foldingLimit;

	HRESULT hr;

	D3D11_BUFFER_DESC bdsc;
    bdsc.Usage = D3D11_USAGE_DYNAMIC;
    bdsc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bdsc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bdsc.MiscFlags = 0;
	bdsc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

    bdsc.ByteWidth = ((sizeof( FracBoxFoldBuffer ) + 15)/16)*16; //must be multiple of 16
	InitData.pSysMem = &buf;

	hr = dxDev->CreateBuffer(&bdsc,&InitData, &cb);
}

FracBoxFold::~FracBoxFold(){
	 SAFE_RELEASE( cb );
}

void FracBoxFold::addControlsToBar(TwBar* bar) {
	if(lastBar!=NULL) removeControlsFromBar();

	string ctr = "boxFold_" + fromNumber(pos);
	string ctr_def = "min=0 step=0.05 label='Folding Limit' group='Box Fold (" + fromNumber(pos) + ")'";
	TwAddVarRW(bar, ctr.c_str(), TW_TYPE_FLOAT, &(buf.fop_foldingLimit), ctr_def.c_str());

	string btn = "boxFold_remove_" + fromNumber(pos);
	string btn_def = "label='Remove' group='Box Fold (" + fromNumber(pos) + ")'";
	TwAddButton(bar, btn.c_str(), FracOpRemoveControlBtn , this, btn_def.c_str());

	lastBar = bar;
}

void FracBoxFold::removeControlsFromBar() {
	if(lastBar!=NULL) {
		string ctr = "boxFold_" + fromNumber(pos);
		string btn = "boxFold_remove_" + fromNumber(pos);
		TwRemoveVar(lastBar,ctr.c_str());
		TwRemoveVar(lastBar,btn.c_str());
	}
}

string FracBoxFold::getDeCode() {
	string b = "\
	fBoxFold(p,dr,fop<N>_foldingLimit);\n\
	";

	replaceAll(b,"<N>",fromNumber(pos));

	return b;
}

string FracBoxFold::getCbCode() {
	string b = "\n\
			   \n\
	cbuffer fop<N>_BoxFold : register( b<BN> ) \n\
	{\n\
		float fop<N>_foldingLimit;\n\
	};\n";

	replaceAll(b,"<N>",fromNumber(pos));
	replaceAll(b,"<BN>",fromNumber(pos + FracOperator::minBufferNum));

	return b;
}

void FracBoxFold::setFoldingLimit(float& foldingLimit) {
	buf.fop_foldingLimit = foldingLimit;
}

void FracBoxFold::updateBuffer(ID3D11DeviceContext* dxCon) {
	D3D11_MAPPED_SUBRESOURCE mapSR;
	dxCon->Map(cb,0,D3D11_MAP_WRITE_DISCARD,0,&mapSR);
	*((FracBoxFoldBuffer*)mapSR.pData) = buf;
	dxCon->Unmap(cb,0);
}