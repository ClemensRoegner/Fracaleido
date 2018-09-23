////////////////////////////////////////////////////////////////////////////////////////
//
// Rotation Operator
//
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "FracOperator.h"
#include "glm.h"

struct FracRotationBuffer {
	mat4x4 rot_matrix;
};

class FracRotation : public FracOperator{
	public:
		FracRotation(ID3D11Device* dxDev, float3 axis = float3(1.0f,0.0f,0.0f), float angle = 0.0f);
		~FracRotation();

		void addControlsToBar(TwBar* bar);
		void removeControlsFromBar();
		string getDeCode();
		string getCbCode();
		void updateBuffer(ID3D11DeviceContext* dxCon);

		void setRotation(float3& axis, float& angle);
	private:
		TwBar* lastBar;
		FracRotationBuffer buf;
		float3 rot_axis;
		float rot_angle;
};