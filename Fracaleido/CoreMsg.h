////////////////////////////////////////////////////////////////////////////////////////
//
// Helpers for errors and messages
//
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <conio.h>
#include <string>
#include <windows.h>

using namespace std;

void printErr(char* str); //should trigger an alert as well
void printMsg(char* str); //standard console output and later an output at a specific output window