////////////////////////////////////////////////////////////////////////////////////////
//
// Shader for lighting (based on deferred output) 
//
////////////////////////////////////////////////////////////////////////////////////////

#define PI 3.14159265359f

struct VIn
{
    float4 pos : POSITION;
    float2 tex : TEXCOORD0;
};

struct VOut
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};

SamplerState TextureSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

Texture2D posTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D colorTexture : register(t2);

cbuffer lightBuffer : register( b0 )
{
	float4 lightDir;
	float4 lightColor;
	float4 cameraPos;
};

//----------------------GENERAL FUNCTIONS-----------------------

float3 CartesianToSpherical( in float3 xyz ) {

	float r = length( xyz );
	xyz *= 1.f/r;
	float theta = acos( xyz.z );
	float phi = atan2( xyz.y, xyz.x );

	return float3( phi, theta, r );
}

float3 SphericalToCartesian( in float3 ptr ) {
	return float3(ptr.z * sin(ptr.x) * cos(ptr.y),ptr.z * sin(ptr.x) * sin(ptr.y),ptr.z * sin(ptr.x));
}

//packaging func
float pack(float2 orig)
{
	uint ui = f32tof16(orig.x);
	ui = ui << 16;
	ui = ui | f32tof16(orig.y);
	return asfloat(ui);
}

float2 unpack(float packed)
{
	uint ui = asuint(packed);
	float2 f;
	f.y = f16tof32(ui);
	ui = ui >> 16;
	f.x = f16tof32(ui);
	return f;
}

float getOpeningAngle(in float3 p, in float3 n, in float3 ap) { //maybe add something conditional
	float3 diff = ap - p;
	float ld = length(diff);
	return 1.0-saturate(dot(n,diff/ld));
}

//-----------------------SHADER FUNCTION------------------------

VOut FS_VS( VIn input)
{
	VOut output;
	output.pos = input.pos;
	output.tex = input.tex;
	return output;
}
 
float4 FS_PS( VOut input) : SV_TARGET
{
	float4 tpos = posTexture.Sample(TextureSampler,input.tex);
	float4 tnormal = normalTexture.Sample(TextureSampler,input.tex);
	float4 tcolor = colorTexture.Sample(TextureSampler,input.tex);
	float3 pos = tpos.xyz;
	float3 normal = tnormal.xyz;

	float4 phong = float4(0.2,0.8,0.0,4.0);

	if(tpos.w>0.0) { //we hit something

		//Horizon Based Ambient Occlusion - my interpretation of it
		float4 ao = float4(1,1,1,1); //ambient occlusion
		float3 ap;

		float3 p = pos + normal * 0.005; //just an offset
		for(uint i=1; i<4; i++) {
			ap = posTexture.Sample(TextureSampler,input.tex,int2(i,0)).xyz;
			ao.x = min(ao.x,getOpeningAngle(p,normal,ap));

			ap = posTexture.Sample(TextureSampler,input.tex,int2(-i,0)).xyz;
			ao.y = min(ao.y,getOpeningAngle(p,normal,ap));

			ap = posTexture.Sample(TextureSampler,input.tex,int2(0,i)).xyz;
			ao.z = min(ao.z,getOpeningAngle(p,normal,ap));

			ap = posTexture.Sample(TextureSampler,input.tex,int2(0,-i)).xyz;
			ao.w = min(ao.w,getOpeningAngle(p,normal,ap));
		}
		float cao = (ao.x + ao.y + ao.z + ao.w) * 0.25f;

		//Direct (Phong) Lighting
		//diffuse
		float d = saturate(dot(normal,-lightDir));
		//specular
		float3 refl = reflect(normal,normalize(pos-cameraPos));
		float s = saturate(dot(refl,-lightDir));

		//sum it up and return
		return tcolor * lightColor * (cao * phong.x + d * phong.y + pow(s,phong.w) * phong.z );
	}
	else { //background is always black
		return float4(0,0,0,0);
	}
}