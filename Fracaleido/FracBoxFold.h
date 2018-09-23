////////////////////////////////////////////////////////////////////////////////////////
//
// Box Fold Operator
//
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "FracOperator.h"
#include "glm.h"

struct FracBoxFoldBuffer {
	float fop_foldingLimit;
};

class FracBoxFold : public FracOperator{
	public:
		FracBoxFold(ID3D11Device* dxDev, float foldingLimit = 1.0f);
		~FracBoxFold();

		void addControlsToBar(TwBar* bar);
		void removeControlsFromBar();
		string getDeCode();
		string getCbCode();
		void updateBuffer(ID3D11DeviceContext* dxCon);

		void setFoldingLimit(float& foldingLimit);
	private:
		TwBar* lastBar;
		FracBoxFoldBuffer buf;
};