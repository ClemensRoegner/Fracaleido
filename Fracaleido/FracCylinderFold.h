////////////////////////////////////////////////////////////////////////////////////////
//
// Cylinder Fold Operator
//
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "FracOperator.h"
#include "glm.h"

struct FracCylinderFoldBuffer {
	float fop_minRadius2;
	float fop_fixedRadius2;
	float fop_foldingLimit;
};

class FracCylinderFold : public FracOperator{
	public:
		FracCylinderFold(ID3D11Device* dxDev, float minRadius2 = 0.25f, float fixedRadius2 = 1.0f, float foldingLimit = 1.0f);
		~FracCylinderFold();

		void addControlsToBar(TwBar* bar);
		void removeControlsFromBar();
		string getDeCode();
		string getCbCode();
		void updateBuffer(ID3D11DeviceContext* dxCon);

		void setMinRadius2(float& minRadius2);
		void setFixedRadius2(float& fixedRadius2);
		void setFoldingLimit(float& foldingLimit);
	private:
		TwBar* lastBar;
		FracCylinderFoldBuffer buf;
};