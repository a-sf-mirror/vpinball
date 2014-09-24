static const char copyshader[] = \

"float4 w_h : register(c0);"
"sampler2D back : register(s0);" //RGB
"float4 ps_main(float2 uorg:texcoord):color"
"{"
"const float2 u = uorg + w_h.xy*0.5;"
"return tex2D(back,u);"
"}";

static const char stereo3Dshader[] = \

#if 1
//!! opt.?
"float4 ms_zpd_ya_td : register(c0);"
"float4 w_h_height : register(c1);"
"sampler2D back : register(s0);" //RGB
"sampler2D depth : register(s1);" //INTZ
"float4 ps_main(float2 uorg:texcoord):color"
"{"
"float2 u = uorg + w_h_height.xy*0.5;"
"const float MaxSeparation = ms_zpd_ya_td.x;"
"const float ZPD = ms_zpd_ya_td.y;"
"const bool yaxis = (ms_zpd_ya_td.z != 0.0);" //!! uniform
"const bool topdown = (ms_zpd_ya_td.w != 0.0);" //!! uniform
"const int y = w_h_height.z*u.y;"
"const bool aa = (w_h_height.w != 0.0);" //!! uniform
"const bool l = topdown ? (u.y < 0.5) : ((y+1)/2 == y/2);" //last check actually means (y&1)
"if(topdown) { u.y *= 2.0; if(!l) u.y -= 1.0; }"  //!! !topdown: (u.y+w_h_height.y) ?
"const float su = l ? MaxSeparation : -MaxSeparation;"
"float minDepth = min(min(tex2D(depth,u + (yaxis ? float2(0.0,0.5*su) : float2(0.5*su,0.0))).x, tex2D(depth,u + (yaxis ? float2(0.0,0.666*su) : float2(0.666*su,0.0))).x), tex2D(depth,u + (yaxis ? float2(0.0,su) : float2(su,0.0))).x);"
"float parallax = MaxSeparation - min(MaxSeparation*ZPD/(0.5*ZPD+minDepth*(1.0-0.5*ZPD)), MaxSeparation);"
"if(!l)"
"parallax = -parallax;"
"if(yaxis)"
"parallax = -parallax;"
"const float4 col = tex2D(back,u + (yaxis ? float2(0.0,parallax) : float2(parallax,0.0)));"
"if(!aa)"
"return col;" // otherwise blend with 'missing' scanline
"minDepth = min(min(tex2D(depth,u + (yaxis ? float2(0.0,0.5*su+w_h_height.y) : float2(0.5*su,w_h_height.y))).x, tex2D(depth,u + (yaxis ? float2(0.0,0.666*su+w_h_height.y) : float2(0.666*su,w_h_height.y))).x), tex2D(depth,u + (yaxis ? float2(0.0,su+w_h_height.y) : float2(su,w_h_height.y))).x);"
"parallax = MaxSeparation - min(MaxSeparation*ZPD/(0.5*ZPD+minDepth*(1.0-0.5*ZPD)), MaxSeparation);"
"if(!l)"
"parallax = -parallax;"
"if(yaxis)"
"parallax = -parallax;"
"return (col + tex2D(back,u + (yaxis ? float2(0.0,parallax+w_h_height.y) : float2(parallax,w_h_height.y))))*0.5;"
"}";
#else
"float4 w_h_height : register(c1);"
"sampler2D depth : register(s1);" //INTZ
"float2 hash(float2 gridcell)" // gridcell is assumed to be an integer coordinate
"{"
"const float3 o = float3(26.0f, 161.0f, 26.0f);"
"const float d = 71.0f;"
"const float lf = 1.0f/951.135664f;"
"float3 P = float3(gridcell.x,gridcell.y,gridcell.x+1.0f);"
"P = P - floor(P * ( 1.0f / d )) * d;"
"P += o;"
"P *= P;"
"return frac( P.xz * P.yy * lf );"
"}"
"float3 get_normal(float depth0, float2 u)" // use neighboring pixels
"{"
"const float depth1 = tex2D(depth, float2(u.x, u.y+w_h_height.y)).x;"
"const float depth2 = tex2D(depth, float2(u.x+w_h_height.x, u.y)).x;"
"return normalize(float3(w_h_height.y * (depth2 - depth0), (depth1 - depth0) * w_h_height.x, w_h_height.y * w_h_height.x));" //!!
"}"
"float3 sphere_sample(float2 t)"
"{"
"const float z = t.x*2.0-1.0;"
"const float tt = t.y*(2.0*3.1415926535897932384626433832795);"
"const float r = sqrt(1.0-z*z);"
"return float3(r*cos(tt),r*sin(tt),z);"
"}"
"float4 ps_main(float2 uorg:texcoord):color"
"{"
"const float2 u = uorg + w_h_height.xy*0.5;"
"const float2 ushift = hash(uorg*w_h_height.z)+w_h_height.w;" // jitter samples via hash of position on screen and then jitter samples by time //!! see below for non-shifted variant
"const float total_strength = 1.38;" //!!
"const float base = 0.2;" //!!
"const float area = 0.07;" //!!
"const float falloff = 0.000002;" //!!
"const float radius = 0.006;" //!!
"const int samples = 9;" //4,8,9,13,21,25 korobov,fibonacci
"const float depth0 = tex2D(depth, u).x;"
"const float3 normal = get_normal(depth0, u);"
"const float radius_depth = radius/depth0;"
"float occlusion = 0.0;"
"for(int i=0; i < samples; ++i) {"
"const float2 r = float2(i*(1.0 / samples), i*(2.0 / samples));" //1,5,2,8,13,7 korobov,fibonacci //!! could also use progressive/extensible lattice via rad_inv(i)*(1501825329, 359975893) (check precision though as this should be done in double or uint64)
"const float3 ray = radius_depth * sphere_sample(frac(r+ushift));" // shift lattice
"const float2 hemi_ray = u + sign(dot(ray,normal)) * ray.xy;"
"const float occ_depth = tex2D(depth, hemi_ray).x;"
"const float3 occ_normal = get_normal(occ_depth, hemi_ray);"
"const float diff_depth = depth0 - occ_depth;"
"float diff_norm = dot(occ_normal,normal);"
"diff_norm *= diff_norm;"
"diff_norm *= diff_norm;" //!!
"diff_norm = 1.0-diff_norm*diff_norm;"
"occlusion += step(falloff, diff_depth) * diff_norm * (1.0-smoothstep(falloff, area, diff_depth));"
"}"
"const float ao = 1.0 - total_strength * occlusion * (1.0 / samples);"
"return saturate(ao + base);"
"}";
#endif

// Approximation of FXAA
static const char FXAAshader1[] = \

"float4 w_h : register(c0);"
"sampler2D back : register(s0);" //RGB
"float luma(float3 l)"
"{"
"return 0.25*l.x+0.5*l.y+0.25*l.z;" //!!
"}"
"float4 ps_main(float2 uorg:texcoord):color"
"{"
"const float2 u = uorg + w_h.xy*0.5;"
"const float2 offs = w_h.xy;"
"const float rNW = luma(tex2D(back, u - offs).xyz);"
"const float rN = luma(tex2D(back, u - float2(0.0,offs.y)).xyz);"
"const float rNE = luma(tex2D(back, u - float2(-offs.x,offs.y)).xyz);"
"const float rW = luma(tex2D(back, u - float2(offs.x,0.0)).xyz);"
"const float rM = luma(tex2D(back, u).xyz);"
"const float rE = luma(tex2D(back, u + float2(offs.x,0.0)).xyz);"
"const float rSW = luma(tex2D(back, u + float2(-offs.x,offs.y)).xyz);"
"const float rS = luma(tex2D(back, u + float2(0.0,offs.y)).xyz);"
"const float rSE = luma(tex2D(back, u + offs).xyz);"
"const float rMrN = rM+rN;"
"const float lumaNW = rMrN+rNW+rW;"
"const float lumaNE = rMrN+rNE+rE;"
"const float rMrS = rM+rS;"
"const float lumaSW = rMrS+rSW+rW;"
"const float lumaSE = rMrS+rSE+rE;"
"const bool g0 = (lumaSW > lumaSE);"
"const float tempMax = g0 ? lumaSW : lumaSE;"
"const float tempMin = g0 ? lumaSE : lumaSW;"
"const bool g1 = (lumaNW > lumaNE);"
"const float tempMax2 = g1 ? lumaNW : lumaNE;"
"const float tempMin2 = g1 ? lumaNE : lumaNW;"
"const float lumaMin = min(rM, min(tempMin, tempMin2));"
"const float lumaMax = max(rM, max(tempMax, tempMax2));"
"const float SWSE = lumaSW + lumaSE;"
"const float NWNE = lumaNW + lumaNE;"
"float2 dir = float2(SWSE - NWNE, (lumaNW + lumaSW) - (lumaNE + lumaSE));"
"const float temp = 1.0/(min(abs(dir.x), abs(dir.y)) + max((NWNE + SWSE)*0.03125, 0.0078125));" //!! tweak?
"dir = min(8.0, max(-8.0, dir*temp)) * offs;" //!! tweak?
"const float4 rgbA = 0.5 * (tex2D(back,u-dir*(0.5/3.0)) + tex2D(back,u+dir*(0.5/3.0)));"
"const float4 rgbB = 0.5 * rgbA + 0.25 * (tex2D(back,u-dir*0.5) + tex2D(back,u+dir*0.5));"
"const float lumaB = luma(rgbB.xyz);"
"return ((lumaB < lumaMin) || (lumaB > lumaMax)) ? rgbA : rgbB;"
"}";

// Full mid-quality PC FXAA 3.11
static const char FXAAshader2[] = \

"float4 w_h : register(c0);"
"sampler2D back : register(s0);" //RGB
"float luma(float3 l)"
"{"
"return 0.25*l.x+0.5*l.y+0.25*l.z;" //!!
"}"
"float4 ps_main(float2 uorg:texcoord):color"
"{"
"const float2 u = uorg + w_h.xy*0.5;"
"const float2 offs = w_h.xy;"
"const float lumaNW = luma(tex2D(back, u - offs).xyz);"
"float lumaN = luma(tex2D(back, u - float2(0.0,offs.y)).xyz);"
"const float lumaNE = luma(tex2D(back, u - float2(-offs.x,offs.y)).xyz);"
"const float lumaW = luma(tex2D(back, u - float2(offs.x,0.0)).xyz);"
"const float4 rgbyM = tex2D(back, u);"
"const float lumaM = luma(rgbyM.xyz);"
"const float lumaE = luma(tex2D(back, u + float2(offs.x,0.0)).xyz);"
"const float lumaSW = luma(tex2D(back, u + float2(-offs.x,offs.y)).xyz);"
"float lumaS = luma(tex2D(back, u + float2(0.0,offs.y)).xyz);"
"const float lumaSE = luma(tex2D(back, u + offs).xyz);"
"const float maxSM = max(lumaS, lumaM);"
"const float minSM = min(lumaS, lumaM);"
"const float maxESM = max(lumaE, maxSM);"
"const float minESM = min(lumaE, minSM);"
"const float maxWN = max(lumaN, lumaW);"
"const float minWN = min(lumaN, lumaW);"
"const float rangeMax = max(maxWN, maxESM);"
"const float rangeMin = min(minWN, minESM);"
"const float rangeMaxScaled = rangeMax * 0.166;" //0.333 (faster) .. 0.063 (slower)
"const float range = rangeMax - rangeMin;"
"const float rangeMaxClamped = max(0.0833, rangeMaxScaled);" //0.0625 (high quality/faster) .. 0.0312 (visible limit/slower)
"const bool earlyExit = range < rangeMaxClamped;"
"if(earlyExit)"
"return rgbyM;"
"const float lumaNS = lumaN + lumaS;"
"const float lumaWE = lumaW + lumaE;"
"const float subpixRcpRange = 1.0/range;"
"const float subpixNSWE = lumaNS + lumaWE;"
"const float edgeHorz1 = -2.0 * lumaM + lumaNS;"
"const float edgeVert1 = -2.0 * lumaM + lumaWE;"
"const float lumaNESE = lumaNE + lumaSE;"
"const float lumaNWNE = lumaNW + lumaNE;"
"const float edgeHorz2 = -2.0 * lumaE + lumaNESE;"
"const float edgeVert2 = -2.0 * lumaN + lumaNWNE;"
"const float lumaNWSW = lumaNW + lumaSW;"
"const float lumaSWSE = lumaSW + lumaSE;"
"const float edgeHorz4 = abs(edgeHorz1) * 2.0 + abs(edgeHorz2);"
"const float edgeVert4 = abs(edgeVert1) * 2.0 + abs(edgeVert2);"
"const float edgeHorz3 = -2.0 * lumaW + lumaNWSW;"
"const float edgeVert3 = -2.0 * lumaS + lumaSWSE;"
"const float edgeHorz = abs(edgeHorz3) + edgeHorz4;"
"const float edgeVert = abs(edgeVert3) + edgeVert4;"
"const float subpixNWSWNESE = lumaNWSW + lumaNESE;"
"float lengthSign = offs.x;"
"const bool horzSpan = edgeHorz >= edgeVert;"
"const float subpixA = subpixNSWE * 2.0 + subpixNWSWNESE;"
"if(!horzSpan) lumaN = lumaW;"
"if(!horzSpan) lumaS = lumaE;"
"if(horzSpan) lengthSign = offs.y;"
"const float subpixB = (subpixA * (1.0/12.0)) - lumaM;"
"const float gradientN = lumaN - lumaM;"
"const float gradientS = lumaS - lumaM;"
"float lumaNN = lumaN + lumaM;"
"const float lumaSS = lumaS + lumaM;"
"const bool pairN = (abs(gradientN) >= abs(gradientS));"
"const float gradient = max(abs(gradientN), abs(gradientS));"
"if(pairN) lengthSign = -lengthSign;"
"const float subpixC = clamp(abs(subpixB) * subpixRcpRange, 0.0, 1.0);"
"const float2 offNP = float2(!horzSpan ? 0.0 : offs.x, horzSpan ? 0.0 : offs.y);"
"float2 posB = u;"
"const float l05 = lengthSign * 0.5;"
"if(horzSpan) posB.y += l05;"
"else posB.x += l05;"
"float2 posN = float2(posB.x - offNP.x * 1.5, posB.y - offNP.y * 1.5);"
"float2 posP = float2(posB.x + offNP.x * 1.5, posB.y + offNP.y * 1.5);"
"const float subpixD = -2.0 * subpixC + 3.0;"
"float lumaEndN = luma(tex2D(back, posN).xyz);"
"const float subpixE = subpixC * subpixC;"
"float lumaEndP = luma(tex2D(back, posP).xyz);"
"if(!pairN) lumaNN = lumaSS;"
"const float gradientScaled = gradient * 1.0/4.0;"
"const float lumaMM = lumaM - lumaNN * 0.5;"
"const float subpixF = subpixD * subpixE;"
"const bool lumaMLTZero = (lumaMM < 0.0);"
"lumaEndN -= lumaNN * 0.5;"
"lumaEndP -= lumaNN * 0.5;"
"bool doneN = (abs(lumaEndN) >= gradientScaled);"
"bool doneP = (abs(lumaEndP) >= gradientScaled);"
"if(!doneN) posN.x -= offNP.x * 3.0;"
"if(!doneN) posN.y -= offNP.y * 3.0;"
"bool doneNP = ((!doneN) || (!doneP));"
"if(!doneP) posP.x += offNP.x * 3.0;"
"if(!doneP) posP.y += offNP.y * 3.0;"
"if(doneNP) {"
"if(!doneN) lumaEndN = luma(tex2D(back, posN.xy).xyz);"
"if(!doneP) lumaEndP = luma(tex2D(back, posP.xy).xyz);"
"if(!doneN) lumaEndN = lumaEndN - lumaNN * 0.5;"
"if(!doneP) lumaEndP = lumaEndP - lumaNN * 0.5;"
"doneN = abs(lumaEndN) >= gradientScaled;"
"doneP = abs(lumaEndP) >= gradientScaled;"
"if(!doneN) posN.x -= offNP.x * 12.0;"
"if(!doneN) posN.y -= offNP.y * 12.0;"
"doneNP = ((!doneN) || (!doneP));"
"if(!doneP) posP.x += offNP.x * 12.0;"
"if(!doneP) posP.y += offNP.y * 12.0;"
"}"
"const float dstN = horzSpan ? (u.x - posN.x) : (u.y - posN.y);"
"const float dstP = horzSpan ? (posP.x - u.x) : (posP.y - u.y);"
"const bool goodSpanN = ((lumaEndN < 0.0) != lumaMLTZero);"
"const float spanLength = dstP + dstN;"
"const bool goodSpanP = ((lumaEndP < 0.0) != lumaMLTZero);"
"const float spanLengthRcp = 1.0/spanLength;"
"const bool directionN = (dstN < dstP);"
"const float dst = min(dstN, dstP);"
"const bool goodSpan = directionN ? goodSpanN : goodSpanP;"
"const float subpixG = subpixF * subpixF;"
"const float pixelOffset = 0.5 - dst * spanLengthRcp;"
"const float subpixH = subpixG * 0.75;" //1.00 (upper limit/softer) .. 0.50 (lower limit/sharper) .. 0.00 (completely off)
"const float pixelOffsetGood = goodSpan ? pixelOffset : 0.0;"
"const float pixelOffsetSubpix = max(pixelOffsetGood, subpixH);"
"float2 un = u;"
"const float pl = pixelOffsetSubpix * lengthSign;"
"if(horzSpan) un.y += pl;"
"else un.x += pl;"
"return tex2D(back, un);"
"}";
