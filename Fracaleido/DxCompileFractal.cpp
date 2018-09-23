////////////////////////////////////////////////////////////////////////////////////////
//
// Logic for compiling the user generated fractal shader
//
////////////////////////////////////////////////////////////////////////////////////////

#include "DxCompileFractal.h"
#include "CoreMacros.h"
#include "GraphicMacros.h"
#include "CoreMsg.h"
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <vector>
#include <algorithm>

bool delFunc(FracOperator* f) {
	return f == nullptr;
}

bool compileFractal() { //function to generate new source code for shader and compile it
	SAFE_RELEASE( dxCS );

	for (UINT i = 0; i<fracOps.size(); i++) {
		if (fracOps.at(i) != nullptr && fracOps.at(i)->isToDelete()) {
			SAFE_DELETE(fracOps.at(i));
		}
	}

	//delete obsolete operators from array
	fracOps.erase(std::remove_if(fracOps.begin(), fracOps.end(), delFunc), fracOps.end());

	HRESULT hr;
	ID3DBlob* pErrorBlob = NULL;

	ifstream t = ifstream("ComputeShader.hlsl");
	string str;

	t.seekg(0, std::ios::end);   
	str.reserve(t.tellg());
	t.seekg(0, std::ios::beg);

	str.assign((std::istreambuf_iterator<char>(t)),std::istreambuf_iterator<char>());

	for(UINT i=0; i<fracOps.size();i++) { //replacing all the strings
		if(fracOps.at(i)) {
			string buf = fracOps.at(i)->getCbCode();
			string op = fracOps.at(i)->getDeCode();

			replace(str,"//<BUFFERS>",buf+" //<BUFFERS>");
			replace(str,"//<OPERATORS>",op+" //<OPERATORS>");
			replace(str,"//<TOPERATORS>",op+" //<TOPERATORS>");
		}
	}

	t.close();

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
	#if defined( DEBUG ) || defined( _DEBUG )
		dwShaderFlags |= D3DCOMPILE_DEBUG;
	#endif

    ID3DBlob* pBlob = NULL;
	hr = D3DCompile(str.c_str(),str.length(),NULL,NULL,NULL,"CSMain","cs_5_0",dwShaderFlags,0,&pBlob,&pErrorBlob);
    if ( FAILED(hr) )
    {
        if ( pErrorBlob ) OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );

        SAFE_RELEASE( pErrorBlob );
        SAFE_RELEASE( pBlob );    

        return false;
    }    

	hr = dxDev->CreateComputeShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &dxCS ); //compute everything

    SAFE_RELEASE( pErrorBlob );
    SAFE_RELEASE( pBlob );

	for(UINT i=0; i<fracOps.size();i++) { //update status
		if(fracOps.at(i)) fracOps.at(i)->setIsCompiled(true);
	}

	return true;
}