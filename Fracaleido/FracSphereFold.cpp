////////////////////////////////////////////////////////////////////////////////////////
//
// Sphere Fold Operator
//
////////////////////////////////////////////////////////////////////////////////////////

#include "FracSphereFold.h"
#include "GraphicMacros.h"

FracSphereFold::FracSphereFold(ID3D11Device* dxDev, float minRadius2, float fixedRadius2) : FracOperator(), lastBar(NULL) {
	buf.fop_minRadius2 = minRadius2;
	buf.fop_fixedRadius2 = fixedRadius2;

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

    bdsc.ByteWidth = ((sizeof( FracSphereFoldBuffer ) + 15)/16)*16; //must be multiple of 16
	InitData.pSysMem = &buf;

	hr = dxDev->CreateBuffer(&bdsc,&InitData, &cb);
}

FracSphereFold::~FracSphereFold(){
	 SAFE_RELEASE( cb );
}

void FracSphereFold::addControlsToBar(TwBar* bar) {
	if(lastBar!=NULL) removeControlsFromBar();

	string ctr0 = "sphereFold_minR_" + fromNumber(pos);
	string ctr0_def = "min=0 step=0.05 label='Minimal Radius Squared' group='Sphere Fold (" + fromNumber(pos) + ")'";
	TwAddVarRW(bar, ctr0.c_str(), TW_TYPE_FLOAT, &(buf.fop_minRadius2), ctr0_def.c_str());

	string ctr1 = "sphereFold_fixR_" + fromNumber(pos);
	string ctr1_def = "min=0 step=0.05 label='Fixed Radius Squared' group='Sphere Fold (" + fromNumber(pos) + ")'";
	TwAddVarRW(bar, ctr1.c_str(), TW_TYPE_FLOAT, &(buf.fop_fixedRadius2), ctr1_def.c_str());

	string btn = "sphereFold_remove_" + fromNumber(pos);
	string btn_def = "label='Remove' group='Sphere Fold (" + fromNumber(pos) + ")'";
	TwAddButton(bar, btn.c_str(), FracOpRemoveControlBtn , this, btn_def.c_str());

	lastBar = bar;
}

void FracSphereFold::removeControlsFromBar() {
	if(lastBar!=NULL) {
		string ctr0 = "sphereFold_minR_" + fromNumber(pos);
		string ctr1 = "sphereFold_fixR_" + fromNumber(pos);
		string btn = "sphereFold_remove_" + fromNumber(pos);
		TwRemoveVar(lastBar,ctr0.c_str());
		TwRemoveVar(lastBar,ctr1.c_str());
		TwRemoveVar(lastBar,btn.c_str());
	}
}

string FracSphereFold::getDeCode() {
	string b = "\
	fSphereFold(p,dr,fop<N>_minRadius2,fop<N>_fixedRadius2);\n\
	";

	replaceAll(b,"<N>",fromNumber(pos));

	return b;
}

string FracSphereFold::getCbCode() {
	string b = "\n\
			   \n\
	cbuffer fop<N>_SphereFold : register( b<BN> ) \n\
	{\n\
		float fop<N>_minRadius2;\n\
		float fop<N>_fixedRadius2;\n\
	};\n";

	replaceAll(b,"<N>",fromNumber(pos));
	replaceAll(b,"<BN>",fromNumber(pos + FracOperator::minBufferNum));

	return b;
}

void FracSphereFold::setMinRadius2(float& minRadius2) {
	buf.fop_minRadius2 = minRadius2;
}

void FracSphereFold::setFixedRadius2(float& fixedRadius2) {
	buf.fop_fixedRadius2 = fixedRadius2;
}

void FracSphereFold::updateBuffer(ID3D11DeviceContext* dxCon) {
	D3D11_MAPPED_SUBRESOURCE mapSR;
	dxCon->Map(cb,0,D3D11_MAP_WRITE_DISCARD,0,&mapSR);
	*((FracSphereFoldBuffer*)mapSR.pData) = buf;
	dxCon->Unmap(cb,0);
}