#ifndef _BILLBOARD
#define _BILLBOARD

#include "value.fx"

#define BILLBOARD_SCALE g_vec2_0 // 빌보드의 가로세로 크기정보로 사용

struct VS_IN
{
    float3 vPos     : POSITION; // Sementic : Layout 에서 설명한 이름       
    float2 vUV      : TEXCOORD;
    float4 vColor   : COLOR;
};

struct VS_OUT
{
    float4 vPosition    : SV_Position; // 래스터라이져로 보낼때, NDC 좌표
    float2 vUV          : TEXCOORD;
    float4 vColor       : COLOR;
};


VS_OUT VS_Billboard(VS_IN _input)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    //                          ( scale.x     0         0       0)
    // (x, y, z, 동차좌표)  X    (   0      scale.y      0       0)
    //                          (   0         0      scale.z    0)
    //                          (  Pos.x    Pos.y      Pos.x    1) 
    // 동차좌표가 1이나 0 이냐에 따라서, 곱하는 변환행렬의 4행 이동정보를 적용할지 말지가 결정된다
    
    float4 vWorld = mul(float4(0.f, 0.f, 0.f, 1.f), g_matWorld);
    float4 vView = mul(vWorld, g_matView);

    vView.xy += _input.vPos.xy * BILLBOARD_SCALE;

    float4 vProj = mul(vView, g_matProj);
 
    output.vPosition    = vProj;
    output.vUV          = _input.vUV;
    output.vColor       = _input.vColor;
    
    return output;
}

// 입력된 텍스쳐를 사용해서 픽셀쉐이더의 출력 색상으로 지정한다.
float4 PS_Billboard(VS_OUT _input) : SV_Target
{
    // 입력 UV는 정점에서 반환한 값을 보간받아서 Pixel Shader에 입력됨
    float4 color = g_tex_0.Sample(g_sam_1, _input.vUV);
    
    //if (color.a == 0.f || color.r > 0.99f && color.b > 0.99f && color.g == 0.f)
    //{
    //    discard; // 아무것도 안찍겠다(파이프라인 과정 여기서 끝낸다) (출력 자체를 안함(AlphaBlending과 다르게 깊이출력 또한 출력 x)
    //}
    
    return color;
}




#endif
