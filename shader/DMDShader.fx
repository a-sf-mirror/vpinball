//!! split into pure sprite & pure DMD?

#include "Helpers.fxh"

float4 vColor_Intensity;
float3 vRes_Alpha;

texture Texture0;

sampler2D texSampler0 : TEXUNIT0 = sampler_state // DMD
{
	Texture	  = (Texture0);
    MIPFILTER = NONE;
    MAGFILTER = POINT;
    MINFILTER = POINT;
    // Set texture to mirror, so the alpha state of the texture blends correctly to the outside
	ADDRESSU  = MIRROR;
	ADDRESSV  = MIRROR;
};

sampler2D texSampler1 : TEXUNIT0 = sampler_state // Sprite
{
	Texture	  = (Texture0);
    MIPFILTER = LINEAR;
    MAGFILTER = LINEAR;
    MINFILTER = LINEAR;
    // Set texture to mirror, so the alpha state of the texture blends correctly to the outside
	ADDRESSU  = MIRROR;
	ADDRESSV  = MIRROR;
};

//
// VS function output structures 
//

struct VS_OUTPUT 
{ 
   float4 pos  : POSITION;
   float2 tex0 : TEXCOORD0;
}; 

VS_OUTPUT vs_main (float4 vPosition : POSITION0,
                   float2 tc        : TEXCOORD0)
{ 
   VS_OUTPUT Out;

   Out.pos = float4(vPosition.xy, 0.0,1.0);
   Out.tex0 = tc;
   
   return Out;
}

// transformation matrices (only used for flashers so far)
float4x4 matWorldViewProj : WORLDVIEWPROJ;

VS_OUTPUT vs_simple_main(float4 vPosition : POSITION0,
    float2 tc : TEXCOORD0)
{
    VS_OUTPUT Out;

    Out.pos = mul(vPosition, matWorldViewProj);
    Out.tex0 = tc;

    return Out;
}

//
// PS functions (DMD and "sprites")
//

#if 0 // raw pixelated output
float4 ps_main_DMD_no(in VS_OUTPUT IN) : COLOR
{
   const float4 rgba = tex2Dlod(texSampler0, float4(IN.tex0, 0.,0.));
   float3 color = vColor_Intensity.xyz * vColor_Intensity.w; //!! create function that resembles LUT from VPM?
   if(rgba.a != 0.0)
      color *= rgba.bgr;
   else
      color *= rgba.b * (255.9 / 100.);

   return float4(InvToneMap(InvGamma(color)), 1.); //!! meh, this sucks a bit performance-wise, but how to avoid this when doing fullscreen-tonemap/gamma without stencil and depth read?
}
#endif

float4 ps_main_DMD_big(in VS_OUTPUT IN) : COLOR
{
   const float4 rgba = tex2Dlod(texSampler0, float4(IN.tex0, 0.,0.));
   float3 color = vColor_Intensity.xyz * vColor_Intensity.w; //!! create function that resembles LUT from VPM?
   if(rgba.a != 0.0)
      color *= rgba.bgr;
   else
      color *= rgba.b * (255.9 / 100.);

   const float2 xy = IN.tex0 * vRes_Alpha.xy;
   const float2 dist = frac(xy)*2.2-1.1;
   const float d = dist.x*dist.x+dist.y*dist.y;

   color *= smoothstep(0.,1.,1.0-d*d);

   /*float3 color2 = float3(0,0,0);
   [unroll] for(int j = -1; j <= 1; ++j)
     [unroll] for(int i = -1; i <= 1; ++i)
	 {
	 //collect glow from neighbors
	 }*/

   return float4(InvToneMap(InvGamma(color/*+color2*/)), 1.); //!! meh, this sucks a bit performance-wise, but how to avoid this when doing fullscreen-tonemap/gamma without stencil and depth read?
}

float4 ps_main_DMD(in VS_OUTPUT IN) : COLOR
{
   const float4 rgba = tex2Dlod(texSampler0, float4(IN.tex0, 0.,0.));
   float3 color = vColor_Intensity.xyz * (1.25 * vColor_Intensity.w); //!! create function that resembles LUT from VPM?
   if(rgba.a != 0.0)
      color *= rgba.bgr;
   else
      color *= rgba.b * (255.9 / 100.);

   const float2 xy = IN.tex0 * vRes_Alpha.xy;
   const float2 dist = frac(xy)*2.-1.;
   const float d = dist.x*dist.x+dist.y*dist.y;

   color *= saturate(1.0-d);

   /*float3 color2 = float3(0,0,0);
   [unroll] for(int j = -1; j <= 1; ++j)
     [unroll] for(int i = -1; i <= 1; ++i)
	 {
	 //collect glow from neighbors
	 }*/

   return float4(InvToneMap(InvGamma(color/*+color2*/)), 1.); //!! meh, this sucks a bit performance-wise, but how to avoid this when doing fullscreen-tonemap/gamma without stencil and depth read?
}

float4 ps_main_DMD_tiny(in VS_OUTPUT IN) : COLOR
{
   const float4 rgba = tex2Dlod(texSampler0, float4(IN.tex0, 0.,0.));
   float3 color = vColor_Intensity.xyz * (1.5 * vColor_Intensity.w); //!! create function that resembles LUT from VPM?
   if(rgba.a != 0.0)
      color *= rgba.bgr;
   else
      color *= rgba.b * (255.9 / 100.);

   const float2 xy = IN.tex0 * vRes_Alpha.xy;
   const float2 dist = frac(xy)*1.4142-0.7071;
   const float d = dist.x*dist.x+dist.y*dist.y;

   color *= 1.0-pow(d, 0.375);

   /*float3 color2 = float3(0,0,0);
   [unroll] for(int j = -1; j <= 1; ++j)
     [unroll] for(int i = -1; i <= 1; ++i)
	 {
	 //collect glow from neighbors
	 }*/

   //if (rgba.b > 200.0)
   //   return float4(InvToneMap(InvGamma(min(color,float3(1.5,1.5,1.5))/*+color2*/)), 0.5f);
   //else
   return float4(InvToneMap(InvGamma(min(color,float3(1.5,1.5,1.5))/*+color2*/)), vRes_Alpha.z); //!! meh, this sucks a bit performance-wise, but how to avoid this when doing fullscreen-tonemap/gamma without stencil and depth read?
}

float4 ps_main_noDMD(in VS_OUTPUT IN) : COLOR
{
   const float4 l = tex2D(texSampler1, IN.tex0);

   return float4(InvToneMap(InvGamma(l.xyz * vColor_Intensity.xyz * vColor_Intensity.w)), l.w); //!! meh, this sucks a bit performance-wise, but how to avoid this when doing fullscreen-tonemap/gamma without stencil and depth read?
}

float4 ps_main_noDMD_notex(in VS_OUTPUT IN) : COLOR
{
   return float4(InvToneMap(InvGamma(vColor_Intensity.xyz * vColor_Intensity.w)), 1.0f); //!! meh, this sucks a bit performance-wise, but how to avoid this when doing fullscreen-tonemap/gamma without stencil and depth read?
}

technique basic_DMD
{ 
   pass P0 
   { 
      VertexShader = compile vs_3_0 vs_main(); 
	  PixelShader = compile ps_3_0 ps_main_DMD();
   } 
}

technique basic_DMD_big
{ 
   pass P0 
   { 
      VertexShader = compile vs_3_0 vs_main(); 
	  PixelShader = compile ps_3_0 ps_main_DMD_big();
   } 
}

technique basic_DMD_tiny
{ 
   pass P0 
   { 
      VertexShader = compile vs_3_0 vs_main(); 
	  PixelShader = compile ps_3_0 ps_main_DMD_tiny();
   } 
}


technique basic_noDMD
{ 
   pass P0 
   { 
      VertexShader = compile vs_3_0 vs_main(); 
	  PixelShader = compile ps_3_0 ps_main_noDMD();
   } 
}

technique basic_noDMD_notex
{ 
   pass P0 
   { 
      VertexShader = compile vs_3_0 vs_main(); 
	  PixelShader = compile ps_3_0 ps_main_noDMD_notex();
   } 
}

technique basic_DMD_tiny_world
{
    pass P0
    {
        VertexShader = compile vs_3_0 vs_simple_main();
        PixelShader = compile ps_3_0 ps_main_DMD_tiny();
    }
}
