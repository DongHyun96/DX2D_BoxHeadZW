#ifndef _FLIPBOOK
#define _FLIPBOOK

#include "value.fx"

#define AtlasTex g_tex_0

struct FlipbookInstanceData
{
    row_major matrix 	World;
    float4              RenderTransform;
    float4              UV0; // xy: LeftTop, zw: Slice
    float4              UV1; // xy: Background, zw: Offset
    float4              TintColor;
};

StructuredBuffer<FlipbookInstanceData> g_FlipbookInstanceBuffer : register(t21);

struct VS_IN
{
    float3 	vPos 		: POSITION;
    float2 	vUV 		: TEXCOORD;
    uint 	InstanceID 	: SV_InstanceID;
};

struct VS_OUT
{
    float4               vPosition 	: SV_Position;
    float3               vWorldPos 	: POSITION;
    float2               vUV 		: TEXCOORD0;
    nointerpolation uint InstanceID : TEXCOORD1;
};

VS_OUT VS_Flipbook(VS_IN _input)
{
    VS_OUT output = (VS_OUT) 0.f;
    const FlipbookInstanceData instanceData = g_FlipbookInstanceBuffer[_input.InstanceID];

    const float2 localXY = _input.vPos.xy * instanceData.RenderTransform.zw + instanceData.RenderTransform.xy;
    const float4 vWorld = mul(float4(localXY, _input.vPos.z, 1.f), instanceData.World);

    const float4 vView = mul(vWorld, g_matView);
    const float4 vProj = mul(vView, g_matProj);

    output.vPosition = vProj;
    output.vWorldPos = vWorld;
    output.vUV = _input.vUV;
    output.InstanceID = _input.InstanceID;

    return output;
}

float4 PS_Flipbook(VS_OUT _input) : SV_Target
{
    const FlipbookInstanceData instanceData = g_FlipbookInstanceBuffer[_input.InstanceID];
    const float2 LeftTopUV 		= instanceData.UV0.xy;
    const float2 SliceUV 		= instanceData.UV0.zw;
    const float2 BackgroundUV 	= instanceData.UV1.xy;
    const float2 OffsetUV 		= instanceData.UV1.zw;

    float4 vColor = float4(1.f, 1.f, 1.f, 1.f);

    if (g_btex_0)
    {
        const float2 LeftTop = LeftTopUV + SliceUV * 0.5f - BackgroundUV * 0.5f;
        const float2 SampleUV = LeftTop + BackgroundUV * _input.vUV - OffsetUV;

        if (LeftTopUV.x <= SampleUV.x && SampleUV.x <= LeftTopUV.x + SliceUV.x
         && LeftTopUV.y <= SampleUV.y && SampleUV.y <= LeftTopUV.y + SliceUV.y)
        {
            vColor = AtlasTex.Sample(g_sam_1, SampleUV);
        }
        else
        {
            discard;
        }

        if (vColor.a == 0.f) discard;
    }

    vColor *= instanceData.TintColor;

    float3 LightColor = float3(0.f, 0.f, 0.f);
    for (int i = 0; i < Light2DCount; ++i)
        LightColor += CalcLight2D(i, _input.vWorldPos);

    vColor.rgb *= LightColor;
    return vColor;
}

#endif
