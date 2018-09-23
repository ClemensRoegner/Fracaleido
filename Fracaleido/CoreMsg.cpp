////////////////////////////////////////////////////////////////////////////////////////
//
// Helpers for errors and messages
//
////////////////////////////////////////////////////////////////////////////////////////

#include "CoreMsg.h"

void printErr(char* str) { //should trigger an alert as well
	cout << "ERROR: " << str << endl;
}

void printMsg(char* str) { //standard console output and later an output at a specific output window
	cout << str << endl;
}
