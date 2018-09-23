////////////////////////////////////////////////////////////////////////////////////////
//
// Plane Fold Operator
//
////////////////////////////////////////////////////////////////////////////////////////

#include "FracPlaneFold.h"
#include "GraphicMacros.h"

FracPlaneFold::FracPlaneFold(ID3D11Device* dxDev, float3 p) : FracOperator(), lastBar(NULL) {
	buf.fop_plane = p;

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

    bdsc.ByteWidth = ((sizeof( FracPlaneFoldBuffer ) + 15)/16)*16; //must be multiple of 16
	InitData.pSysMem = &buf;

	hr = dxDev->CreateBuffer(&bdsc,&InitData, &cb);
}

FracPlaneFold::~FracPlaneFold(){
	 SAFE_RELEASE( cb );
}

void FracPlaneFold::addControlsToBar(TwBar* bar) {
	if(lastBar!=NULL) removeControlsFromBar();

	string ctr = "planeFold_" + fromNumber(pos);
	string ctr_def = "opened=true axisz=z showval=true label='Direction' group='Plane Fold (" + fromNumber(pos) + ")'";
	TwAddVarRW(bar, ctr.c_str(), TW_TYPE_DIR3F, &(buf.fop_plane), ctr_def.c_str());

	string btn = "planeFold_remove_" + fromNumber(pos);
	string btn_def = "label='Remove' group='Plane Fold (" + fromNumber(pos) + ")'";
	TwAddButton(bar, btn.c_str(), FracOpRemoveControlBtn , this, btn_def.c_str());

	lastBar = bar;
}

void FracPlaneFold::removeControlsFromBar() {
	if(lastBar!=NULL) {
		string ctr = "planeFold_" + fromNumber(pos);
		string btn = "planeFold_remove_" + fromNumber(pos);
		TwRemoveVar(lastBar,ctr.c_str());
		TwRemoveVar(lastBar,btn.c_str());
	}
}

string FracPlaneFold::getDeCode() {
	string b = "\
	fPlaneFold(p,dr,fop<N>_plane);\n\
	";

	replaceAll(b,"<N>",fromNumber(pos));

	return b;
}

string FracPlaneFold::getCbCode() {
	string b = "\n\
			   \n\
	cbuffer fop<N>_PlaneFold : register( b<BN> ) \n\
	{\n\
		float3 fop<N>_plane;\n\
	};\n";

	replaceAll(b,"<N>",fromNumber(pos));
	replaceAll(b,"<BN>",fromNumber(pos + FracOperator::minBufferNum));

	return b;
}

void FracPlaneFold::setPlane(float3& p) {
	buf.fop_plane = p;
}

void FracPlaneFold::normalizePlane() {
	buf.fop_plane = normalize(buf.fop_plane);
}

void FracPlaneFold::updateBuffer(ID3D11DeviceContext* dxCon) {
	D3D11_MAPPED_SUBRESOURCE mapSR;
	dxCon->Map(cb,0,D3D11_MAP_WRITE_DISCARD,0,&mapSR);
	*((FracPlaneFoldBuffer*)mapSR.pData) = buf;
	dxCon->Unmap(cb,0);
}