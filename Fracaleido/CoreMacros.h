////////////////////////////////////////////////////////////////////////////////////////
//
// Useful macros
//
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#define GETBYOFFSET(data,type,offsetInBytes) *(##type*)(((byte*)&##data)+offsetInBytes)

#define GETSET(type,var,name) \
	type get##name() { return var;} \
	void set##name(##type& v) { var = v;}

#define LISTFUNC(type,name) \
	private: vector<##type> list##name; \
	public:\
	UINT add##name(##type& e) { list##name.push_back(e); return list##name.size()-1; } \
	void add##name(##type& e, UINT i) { list##name.insert(list##name.begin()+i,e); } \
	void remove##name(UINT i) { list##name.erase(list##name.begin()+i); }

#define SAFE_DELETE(p) if(p) { delete p; p = NULL; }