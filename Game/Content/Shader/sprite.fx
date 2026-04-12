#ifndef _SPRITE
#define _SPRITE

#include "value.fx"

#define AtlasTex    g_tex_0
#define LeftTopUV   g_vec2_0
#define SliceUV     g_vec2_1

#define RenderOffset g_vec4_3.xy
#define RenderScale  g_vec4_3.zw
#define RenderPivot  g_vec4_2.xy


struct VS_IN
{
    float3 vPos : POSITION; // Sementic : Layout 에서 설명한 이름       
    float2 vUV  : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition    : SV_Position; // 래스터라이져로 보낼때, NDC 좌표
    float3 vWorldPos    : POSITION;
    float2 vUV          : TEXCOORD;
};

VS_OUT VS_Sprite(VS_IN _input)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    float2 localXY = (_input.vPos.xy - (RenderPivot - float2(0.5f, 0.5f))) * RenderScale + RenderOffset;
    float4 vWorld  = mul(float4(localXY, _input.vPos.z, 1.f), g_matWorld);
    
    float4 vView = mul(vWorld, g_matView);
    float4 vProj = mul(vView, g_matProj);

    output.vWorldPos = vWorld;
    output.vPosition = vProj;
    output.vUV = _input.vUV;
    
    return output;
}

// 입력된 텍스쳐를 사용해서 픽셀쉐이더의 출력 색상으로 지정한다.
float4 PS_Sprite(VS_OUT _input) : SV_Target
{
    // _input.vUV -> Sprite 한장 내의 비율
    // SliceUV -> Sprite 한장 내에서의 가로, 세로 비율 UV좌표값
    // LeftTopUV -> 사용할 Sprite Frame x, y 비율 좌표
    
    float2 vSpriteUV = _input.vUV * SliceUV + LeftTopUV;
    float4 vColor = AtlasTex.Sample(g_sam_1, vSpriteUV);
    
    if (vColor.a == 0.f) discard;
    
    // 물체가 받는 빛의 총합
    float3 LightColor =  float3(0.f, 0.f, 0.f);
    
    // 반복문을 돌면서, 모든 광원으로부터 어느정도의 빛을 받는지 합산
    for (int i = 0; i < Light2DCount; ++i)
        LightColor += CalcLight2D(i, _input.vWorldPos);

    // 물체의 색상에, 자신이 받는 최종빛 총량을 곱한다.
    vColor.rgb *= LightColor;
    
    return vColor;
}





#endif