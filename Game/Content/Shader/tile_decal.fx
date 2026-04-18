#ifndef _TILE_DECAL
#define _TILE_DECAL

#include "value.fx"

struct DecalInstanceData
{
    row_major matrix World;
    float4 TintColor;
    float2 LeftTopUV;
    float2 SliceUV;
};

StructuredBuffer<DecalInstanceData> g_DecalInstanceBuffer : register(t21);

struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV  : TEXCOORD;
    uint   InstanceID : SV_InstanceID;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV       : TEXCOORD;
    float4 vTintColor : COLOR;
};

VS_OUT VS_TileDecal(VS_IN _input)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    DecalInstanceData data = g_DecalInstanceBuffer[_input.InstanceID];
    
    float4 vWorldPos = mul(float4(_input.vPos, 1.f), data.World);
    float4 vViewPos  = mul(vWorldPos, g_matView);
    float4 vProjPos  = mul(vViewPos, g_matProj);
    
    output.vPosition = vProjPos;
    output.vUV = _input.vUV * data.SliceUV + data.LeftTopUV;
    output.vTintColor = data.TintColor;
    
    return output;
}

float4 PS_TileDecal(VS_OUT _input) : SV_Target
{
    float4 vColor = g_tex_0.Sample(g_sam_1, _input.vUV);
    vColor *= _input.vTintColor;
    
    if (vColor.a <= 0.f)
        discard;
        
    return vColor;
}

#endif
