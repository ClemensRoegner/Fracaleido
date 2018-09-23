////////////////////////////////////////////////////////////////////////////////////////
//
// General Operator Base class
//
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <AntTweakBar.h>
#include <string>
#include "CoreString.h"
#include <d3d11.h>
#include <vector>

using namespace std;

class FracOperator {
	public:
		FracOperator();
		virtual ~FracOperator();

		void setPos(int p);
		int getPos();
		void setToDelete();
		bool isToDelete();
		void setIsCompiled(bool b);
		bool isCompiled();

		virtual void addControlsToBar(TwBar* bar) = 0;
		virtual void removeControlsFromBar() = 0;
		virtual string getDeCode() = 0;
		virtual string getCbCode() = 0;
		virtual void updateBuffer(ID3D11DeviceContext* dxCon) = 0;

		int pos;
		ID3D11Buffer* cb;

		static const int minBufferNum = 3;
	private:
		
		bool toDelete;
		bool isComp;
};

extern vector<FracOperator*> fracOps;

void TW_CALL FracOpRemoveControlBtn(void* userData);

int getMaxPosOfOperators();