////////////////////////////////////////////////////////////////////////////////////////
//
// Helpers for shader compilation
//
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <sstream>

using namespace std;

bool replace(std::string& str, const std::string& from, const std::string& to); //replaces one substring with another string
void replaceAll(std::string& str, const std::string& from, const std::string& to); //for every string

template <typename num>
string fromNumber(num i) {
	ostringstream tmp;
	tmp << i;
	return tmp.str();
}