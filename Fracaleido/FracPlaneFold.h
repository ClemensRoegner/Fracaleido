////////////////////////////////////////////////////////////////////////////////////////
//
// Plane Fold Operator
//
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "FracOperator.h"
#include "glm.h"

struct FracPlaneFoldBuffer {
	float3 fop_plane;
};

class FracPlaneFold : public FracOperator{
	public:
		FracPlaneFold(ID3D11Device* dxDev, float3 p = float3(0.0f,0.0f,0.0f));
		~FracPlaneFold();

		void addControlsToBar(TwBar* bar);
		void removeControlsFromBar();
		string getDeCode();
		string getCbCode();
		void updateBuffer(ID3D11DeviceContext* dxCon);

		void setPlane(float3& p);
		void normalizePlane();
	private:
		TwBar* lastBar;
		FracPlaneFoldBuffer buf;
};