////////////////////////////////////////////////////////////////////////////////////////
//
// Rotation Operator
//
////////////////////////////////////////////////////////////////////////////////////////

#include "FracRotation.h"
#include "GraphicMacros.h"

FracRotation::FracRotation(ID3D11Device* dxDev,  float3 axis, float angle) : FracOperator(), lastBar(NULL), rot_axis(axis), rot_angle(angle) {

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

    bdsc.ByteWidth = ((sizeof( FracRotationBuffer ) + 15)/16)*16; //must be multiple of 16
	InitData.pSysMem = &buf;

	hr = dxDev->CreateBuffer(&bdsc,&InitData, &cb);
}

FracRotation::~FracRotation(){
	 SAFE_RELEASE( cb );
}

void FracRotation::addControlsToBar(TwBar* bar) {
	if(lastBar!=NULL) removeControlsFromBar();

	string ctr0 = "rotation_axis_" + fromNumber(pos);
	string ctr0_def = "opened=true axisz=z showval=true label='Axis' group='Rotation (" + fromNumber(pos) + ")'";
	TwAddVarRW(bar, ctr0.c_str(), TW_TYPE_DIR3F, &(rot_axis), ctr0_def.c_str());

	string ctr1 = "rotation_angle_" + fromNumber(pos);
	string ctr1_def = "min=0 max=360 step=1.0 label='Angle' group='Rotation (" + fromNumber(pos) + ")'";
	TwAddVarRW(bar, ctr1.c_str(), TW_TYPE_FLOAT, &(rot_angle), ctr1_def.c_str());

	string btn = "rotation_remove_" + fromNumber(pos);
	string btn_def = "label='Remove' group='Rotation (" + fromNumber(pos) + ")'";
	TwAddButton(bar, btn.c_str(), FracOpRemoveControlBtn , this, btn_def.c_str());

	lastBar = bar;
}

void FracRotation::removeControlsFromBar() {
	if(lastBar!=NULL) {
		string ctr0 = "rotation_axis_" + fromNumber(pos);
		string ctr1 = "rotation_angle_" + fromNumber(pos);
		string btn = "rotation_remove_" + fromNumber(pos);
		TwRemoveVar(lastBar,ctr0.c_str());
		TwRemoveVar(lastBar,ctr1.c_str());
		TwRemoveVar(lastBar,btn.c_str());
	}
}

string FracRotation::getDeCode() {
	string b = "\
	fRotation(p,dr,fop<N>_rotMatrix);\n\
	";

	replaceAll(b,"<N>",fromNumber(pos));

	return b;
}

string FracRotation::getCbCode() {
	string b = "\n\
			   \n\
	cbuffer fop<N>_Rotation : register( b<BN> ) \n\
	{\n\
		float4x4 fop<N>_rotMatrix;\n\
	};\n";

	replaceAll(b,"<N>",fromNumber(pos));
	replaceAll(b,"<BN>",fromNumber(pos + FracOperator::minBufferNum));

	return b;
}

void FracRotation::setRotation(float3& axis, float& angle) {
	rot_axis = axis;
	rot_angle = angle;
}

void FracRotation::updateBuffer(ID3D11DeviceContext* dxCon) {
	float3 na = normalize(rot_axis);
	buf.rot_matrix = glm::rotate(mat4x4(),rot_angle,na);

	D3D11_MAPPED_SUBRESOURCE mapSR;
	dxCon->Map(cb,0,D3D11_MAP_WRITE_DISCARD,0,&mapSR);
	*((FracRotationBuffer*)mapSR.pData) = buf;
	dxCon->Unmap(cb,0);
}