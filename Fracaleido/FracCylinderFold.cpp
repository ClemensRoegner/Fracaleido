////////////////////////////////////////////////////////////////////////////////////////
//
// Cylinder Fold Operator
//
////////////////////////////////////////////////////////////////////////////////////////

#include "FracCylinderFold.h"
#include "GraphicMacros.h"

FracCylinderFold::FracCylinderFold(ID3D11Device* dxDev, float minRadius2, float fixedRadius2, float foldingLimit) : FracOperator(),lastBar(NULL) {
	buf.fop_minRadius2 = minRadius2;
	buf.fop_fixedRadius2 = fixedRadius2;
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

    bdsc.ByteWidth = ((sizeof( FracCylinderFoldBuffer ) + 15)/16)*16; //must be multiple of 16
	InitData.pSysMem = &buf;

	hr = dxDev->CreateBuffer(&bdsc,&InitData, &cb);
}

FracCylinderFold::~FracCylinderFold(){
	 SAFE_RELEASE( cb );
}

void FracCylinderFold::addControlsToBar(TwBar* bar) {
	if(lastBar!=NULL) removeControlsFromBar();

	string ctr0 = "cylinderFold_minR_" + fromNumber(pos);
	string ctr0_def = "min=0 step=0.05 label='Minimal Radius Squared' group='Cylinder Fold (" + fromNumber(pos) + ")'";
	TwAddVarRW(bar, ctr0.c_str(), TW_TYPE_FLOAT, &(buf.fop_minRadius2), ctr0_def.c_str());

	string ctr1 = "cylinderFold_fixR_" + fromNumber(pos);
	string ctr1_def = "min=0 step=0.05 label='Fixed Radius Squared' group='Cylinder Fold (" + fromNumber(pos) + ")'";
	TwAddVarRW(bar, ctr1.c_str(), TW_TYPE_FLOAT, &(buf.fop_fixedRadius2), ctr1_def.c_str());

	string ctr2 = "cylinderFold_lim_" + fromNumber(pos);
	string ctr2_def = "min=0 step=0.05 label='Folding Limit' group='Cylinder Fold (" + fromNumber(pos) + ")'";
	TwAddVarRW(bar, ctr2.c_str(), TW_TYPE_FLOAT, &(buf.fop_foldingLimit), ctr2_def.c_str());

	string btn = "cylinderFold_remove_" + fromNumber(pos);
	string btn_def = "label='Remove' group='Cylinder Fold (" + fromNumber(pos) + ")'";
	TwAddButton(bar, btn.c_str(), FracOpRemoveControlBtn , this, btn_def.c_str());

	lastBar = bar;
}

void FracCylinderFold::removeControlsFromBar() {
	if(lastBar!=NULL) {
		string ctr0 = "cylinderFold_minR_" + fromNumber(pos);
		string ctr1 = "cylinderFold_fixR_" + fromNumber(pos);
		string ctr2 = "cylinderFold_lim_" + fromNumber(pos);
		string btn = "cylinderFold_remove_" + fromNumber(pos);
		TwRemoveVar(lastBar,ctr0.c_str());
		TwRemoveVar(lastBar,ctr1.c_str());
		TwRemoveVar(lastBar,ctr2.c_str());
		TwRemoveVar(lastBar,btn.c_str());
	}
}

string FracCylinderFold::getDeCode() {
	string b = "\
		fCylinderFold(p,dr,fop<N>_minRadius2,fop<N>_fixedRadius2,fop<N>_foldingLimit);\n\
	";

	replaceAll(b,"<N>",fromNumber(pos));

	return b;
}

string FracCylinderFold::getCbCode() {
	string b = "\n\
			   \n\
	cbuffer fop<N>_CylinderFold : register( b<BN> ) \n\
	{\n\
		float fop<N>_minRadius2;\n\
		float fop<N>_fixedRadius2;\n\
		float fop<N>_foldingLimit;\n\
	};\n";

	replaceAll(b,"<N>",fromNumber(pos));
	replaceAll(b,"<BN>",fromNumber(pos + FracOperator::minBufferNum));

	return b;
}

void FracCylinderFold::setMinRadius2(float& minRadius2) {
	buf.fop_minRadius2 = minRadius2;
}

void FracCylinderFold::setFixedRadius2(float& fixedRadius2) {
	buf.fop_fixedRadius2 = fixedRadius2;
}

void FracCylinderFold::setFoldingLimit(float& foldingLimit) {
	buf.fop_foldingLimit = foldingLimit;
}

void FracCylinderFold::updateBuffer(ID3D11DeviceContext* dxCon) {
	D3D11_MAPPED_SUBRESOURCE mapSR;
	dxCon->Map(cb,0,D3D11_MAP_WRITE_DISCARD,0,&mapSR);
	*((FracCylinderFoldBuffer*)mapSR.pData) = buf;
	dxCon->Unmap(cb,0);
}