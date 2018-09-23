////////////////////////////////////////////////////////////////////////////////////////
//
// Shader for fractal RT. This shader is compiled on the fly
//
////////////////////////////////////////////////////////////////////////////////////////

#define eps 0.00001
#define iter 25
#define viewIter 200
#define trapIter 8
#define PI 3.14159265359f

cbuffer cameraBuffer : register( b0 )
{
	float4 pos;
	float4 dir;
	float4 up;
	float4 side; //must be left side
	float angle;
	float w;
	float h;
	float ratio;
	float skew;
};

cbuffer fracBuffer : register( b1 )
{
	float fracScale;
	float3 fracOffset;
	int fracOffsetFromPos;
};

cbuffer fracColorBuffer : register( b2 ) 
{
	float4 phong;
	float4 simpleColor;
	float4 trapColor1;
	float4 trapColor2;
	float4 trapColor3;
};

//this is a marker for the parameter buffers
//<BUFFERS>

RWTexture2D<float4> posTexture : register (u0);
RWTexture2D<float4> normalTexture : register (u1);
RWTexture2D<float4> colorTexture : register (u2);

//----------------------KALEIDO FUNCTIONS-----------------------

void fPlaneFold(inout float3 p,inout float dr,in float3 plane) {
	float t = dot(p,plane);
	if(t<0.0f) {
		p -= 2.0f * t * plane;
	}
}

void fSphereFold(inout float3 p, inout float dr, in float minRadius2, in float fixedRadius2) {
	float r2 = dot(p,p);
	if (r2<minRadius2) { 
		// linear inner scaling
		float temp = (fixedRadius2/minRadius2);
		p *= temp;
		dr*= temp;
	} else if (r2<fixedRadius2) { 
		// this is the actual sphere inversion
		float temp =(fixedRadius2/r2);
		p *= temp;
		dr*= temp;
	}
}

void fBoxFold(inout float3 p, inout float dr, in float foldingLimit) {
	p = clamp(p, -foldingLimit, foldingLimit) * 2.0 - p;
}

void fRotation(inout float3 p, inout float dr, in float4x4 rotationMatrix) {
	p = mul(float4(p,1),rotationMatrix).xyz;
}

void fCylinderFold(inout float3 p, inout float dr, in float minRadius2, in float fixedRadius2, in float foldingLimit) {
	
	float r2 = dot(p,p);
	if (r2<minRadius2) { 
		// linear inner scaling
		float temp =  exp(minRadius2);
		p *= temp;
		dr*= temp;
	} else if (r2<fixedRadius2) { 
		// this is the actual sphere inversion
		float temp = exp(r2);
		p *= temp;
		dr*= temp;
	}

}

//----------------------GENERAL FUNCTIONS-----------------------

float3 CartesianToSpherical( in float3 xyz ) {

	float r = length( xyz );
	xyz *= 1.f/r;
	float theta = acos( xyz.z );
	float phi = atan2( xyz.y, xyz.x );
	phi += ( phi < 0 ) ? 2*PI : 0;  // only if you want [0,2pi)

	return float3( phi, theta, r );
}

float3 SphericalToCartesian( in float3 ptr ) {
	return float3(ptr.z * sin(ptr.x) * cos(ptr.y),ptr.z * sin(ptr.x) * sin(ptr.y),ptr.z * sin(ptr.x));
}

float getFractalDistance(in float3 p) {

	float scale = fracScale;
	float3 offset;
	if(fracOffsetFromPos>0)
		offset = p;
	else
		offset = fracOffset;
	float dr = 1.0;

    for(uint i=0; i<iter; i++) { //this function includes a marker where the operator calls are made
		
		//<OPERATORS> 

		p = p*scale + offset;
		dr = dr*abs(scale)+1.0f;
    }
		
	return length(p) / abs(dr);
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


//normal functions
float3 getNormalAt(float3 pos) {
	float h =  2.0f * eps;
	float3 normal;
	normal.x = getFractalDistance(float3(pos.x + h,pos.y,pos.z)) - getFractalDistance(float3(pos.x - h,pos.y,pos.z));
	normal.y = getFractalDistance(float3(pos.x,pos.y + h,pos.z)) - getFractalDistance(float3(pos.x,pos.y - h,pos.z));
	normal.z = getFractalDistance(float3(pos.x,pos.y,pos.z + h)) - getFractalDistance(float3(pos.x,pos.y,pos.z - h));
	return normalize(normal);
}

 void getNormalBasis(in float3 n,inout float3 t,inout float3 b) {
	t = normalize(cross(n, (abs(n.x)<0.1f)?float3(1,0,0):float3(0,1,0))); //numerical stability
	b = normalize(cross(n,t));
 }

//-----------------------SHADER FUNCTION------------------------

[numthreads(32, 32, 1)]
void CSMain( uint3 threadID : SV_DispatchThreadID ) {
	float pixelWidth = 1.0f / (float(min(w,h)) * 1.0f);

	float3 cpos = pos.xyz;
	float3 view = dir.xyz + up.xyz*skew*(threadID.y/h-0.5)*-2.0 + side.xyz*skew*ratio*(threadID.x/w-0.5)*2.0;
	view = normalize(view);

	bool hit = false;
	float ad = 0.0f; //accumulated distance
	float md = length(cpos) + 25.0; //maximal distance = initial distance + maximal radius
	float3 ld;
	float3 ldt;
	float ldp;

	for(uint i=0; i<viewIter; i++) {
		float d = getFractalDistance(cpos);
		ad += d;

		cpos += view * d;

		if(d<(pixelWidth*ad)) { //close enough - subpixel width
			hit = true;
			break;
		}	

		if(ad>md) { //too far
			break;
		}
	}

	float3 normal = float3(0,0,0);
	if(hit) {
		//normal vector
		float3 normal = getNormalAt(cpos);

		posTexture[threadID.xy] = float4(cpos,1);
		normalTexture[threadID.xy] = float4(normal,1);

		if(simpleColor.w>0.0f) { //just a simple color
			colorTexture[threadID.xy] = simpleColor;
		}
		else {
			//color - via orbit rap
			float trapDist = 1e20f;
			float3 p = CartesianToSpherical(abs(cpos*0.8f)); //p.z *= 0.1f;
			float dr = 1.0;
			float high = 0.0f;
			for(uint i=0; i<trapIter; i++) {

				//<TOPERATORS> 

			}
			p = normalize(abs(p));

			colorTexture[threadID.xy] = p.x * trapColor1 + p.y * trapColor2 + p.z * trapColor3;
		}
	}
	else { //no hit
		posTexture[threadID.xy] = float4(0,0,0,0);
		normalTexture[threadID.xy] = float4(0,0,0,0);
		colorTexture[threadID.xy] = float4(0,0,0,0);
	}
}