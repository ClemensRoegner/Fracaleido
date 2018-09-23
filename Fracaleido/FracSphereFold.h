////////////////////////////////////////////////////////////////////////////////////////
//
// Sphere Fold Operator
//
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "FracOperator.h"
#include "glm.h"

struct FracSphereFoldBuffer {
	float fop_minRadius2;
	float fop_fixedRadius2;
};

class FracSphereFold : public FracOperator{
	public:
		FracSphereFold(ID3D11Device* dxDev, float minRadius2 = 0.25f, float fixedRadius2 = 1.0f);
		~FracSphereFold();

		void addControlsToBar(TwBar* bar);
		void removeControlsFromBar();
		string getDeCode();
		string getCbCode();
		void updateBuffer(ID3D11DeviceContext* dxCon);

		void setMinRadius2(float& minRadius2);
		void setFixedRadius2(float& fixedRadius2);
	private:
		TwBar* lastBar;
		FracSphereFoldBuffer buf;
};