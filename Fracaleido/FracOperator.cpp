////////////////////////////////////////////////////////////////////////////////////////
//
// General Operator Base class
//
////////////////////////////////////////////////////////////////////////////////////////

#include "FracOperator.h"
#include "GraphicMacros.h"

vector<FracOperator*> fracOps;
int fracOpsActive = 0;

FracOperator::FracOperator() : pos(0), cb(NULL), toDelete(false), isComp(false) {
	
}

FracOperator::~FracOperator(){
	SAFE_RELEASE(cb);
}

void FracOperator::setPos(int p){
	pos = p;
}

int FracOperator::getPos() {
	return pos;
}

void FracOperator::setToDelete() {
	toDelete = true;
}

bool FracOperator::isToDelete() {
	return toDelete;
}

void FracOperator::setIsCompiled(bool b) {
	isComp = b;
}

bool FracOperator::isCompiled() {
	return isComp;
}

void TW_CALL FracOpRemoveControlBtn(void* userData) {
	FracOperator* fo = (FracOperator*) userData;
	fo->removeControlsFromBar();
	fo->setToDelete();
}

int getMaxPosOfOperators() {
	int n = 0;
	auto it = fracOps.begin();
	while(it!=fracOps.end()) {
		if((*it)) {
			n = max(n,(*it)->getPos());
		}
		it++;
	}
	return n;
}