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
    float4 vPosition  : SV_Position;
    float2 vUV        : TEXCOORD;
    float4 vTintColor : COLOR;
    float3 vWorldPos  : POSITION;
};

VS_OUT VS_TileDecal(VS_IN _input)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    DecalInstanceData data = g_DecalInstanceBuffer[_input.InstanceID];
    
    float4 vWorldPos = mul(float4(_input.vPos, 1.f), data.World);
    float4 vViewPos  = mul(vWorldPos, g_matView);
    float4 vProjPos  = mul(vViewPos, g_matProj);

    output.vPosition  = vProjPos;
    output.vWorldPos  = vWorldPos;
    output.vUV        = _input.vUV * data.SliceUV + data.LeftTopUV;
    output.vTintColor = data.TintColor;
    
    return output;
}

float4 PS_TileDecal(VS_OUT _input) : SV_Target
{
    float4 vColor = g_tex_0.Sample(g_sam_1, _input.vUV);
    vColor       *= _input.vTintColor;
    
    if (vColor.a == 0.f) discard;
    
    // 광원 적용
    
    // 구조화버퍼의 요소 개수, 요소 하나의 크기 정보 가져오기
    // 성능 이슈가 있다. 웬만하면 상수버퍼로 해당 데이터를 따로 전달받는것이 나음
    /*uint count  = 0;
    uint stride = 0;
    g_Light2D.GetDimensions(count, stride);*/

    // 물체가 받는 빛의 총합
    float3 LightColor =  float3(0.f, 0.f, 0.f);
    
    for (int i = 0; i < Light2DCount; ++i)
        LightColor += CalcLight2D(i, _input.vWorldPos);

    // 물체의 색상에, 자신이 받는 최종빛 총량을 곱한다.
    vColor.rgb *= LightColor;
    
    return vColor;
}

#endif
