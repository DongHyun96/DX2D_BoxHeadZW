#ifndef _STD2D
#define _STD2D

#include "value.fx"

#define TintColor g_vec4_0

struct VS_IN
{
    float3 vPos     : POSITION; // Sementic : Layout 에서 설명한 이름       
    float2 vUV      : TEXCOORD;
    float4 vColor   : COLOR;
};

struct VS_OUT
{
    float4 vPosition    : SV_Position; // 래스터라이져로 보낼때, NDC 좌표
    float3 vWorldPos    : POSITION;
    float2 vUV          : TEXCOORD;
    float4 vColor       : COLOR;
};


VS_OUT VS_Std2D(VS_IN _input)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    //                          ( scale.x     0         0       0)
    // (x, y, z, 동차좌표)  X    (   0      scale.y      0       0)
    //                          (   0         0      scale.z    0)
    //                          (  Pos.x    Pos.y      Pos.x    1) 
    // 동차좌표가 1이나 0 이냐에 따라서, 곱하는 변환행렬의 4행 이동정보를 적용할지 말지가 결정된다
    
    float4 vWorld   = mul(float4(_input.vPos, 1.f), g_matWorld);
    float4 vView    = mul(vWorld, g_matView);
    float4 vProj    = mul(vView, g_matProj);
    
    // Proj Matrix를 곱한 뒤, ViewZ 값으로 나누는 작업을 해줘야 NDC 좌표계로 제대로 mapping됨
    // Proj Matrix를 곱한 결과 : (VX, VY, VZ, 1.f) X ProjMat == (PX * VZ, PY * VZ, PZ * VZ, VZ) -> VS에서 실질적인 NDC좌표계로 투영된 결과가 나오지 않는다
    
    // vProj.xyz /= vProj.w; -> 이 w(z값)로 나누는 작업을 Rasterizer에서 알아서 해준다.(이 때가 최종 NDC 좌표로 매핑됨)
    
    /***************************************************************************************************************************************************/
    
    // 투영행렬 특성상, 정확한 NDC 좌표를 얻기 위해서
    // 투영행렬을 곱할 View좌표의 Z 값으로 나누는 작업이 행렬안에 있어야하는데 이것이 불가능하기 때문에
    // 연산 결과의 w(4열) 자리에 View 좌표의 Z가 출력되도록 한다.
    
    // 투영행렬을 곱하고 얻은 결과값의 x, y, z를 w로 나눠야 최종 NDC 좌표를 얻을 수 있다.
    
    // x, y, z를 w로 나누는 작업을 Rasterizer에서 진행하기 때문에
    // 수동으로 나누는 코드를 작성할 필요는 없다.
    output.vWorldPos    = vWorld;
    output.vPosition    = vProj;
    output.vUV          = _input.vUV;
    output.vColor       = _input.vColor;
    
    return output;
}

// 정점에서 반환한 값이 보간(Interpolation) 되어 픽셀 쉐이더의 입력으로 들어온다
float4 PS_Test(VS_OUT _input) : SV_Target
{
    return _input.vColor;
}

float4 PS_Test2(VS_OUT _input) : SV_Target
{
    return float4(0.2f, 0.8f, 0.3f, 1.f);
}

// 입력된 텍스쳐를 사용해서 픽셀쉐이더의 출력 색상으로 지정한다.
float4 PS_Std2D(VS_OUT _input) : SV_Target
{
    float4 vColor = float4(1.f, 0.f, 1.f, 1.f);
    
    if (g_btex_0) vColor = g_tex_0.Sample(g_sam_1, _input.vUV);
    
    vColor *= TintColor;
    
    // 입력 UV는 정점에서 반환한 값을 보간받아서 Pixel Shader에 입력됨
    
    /*if (color.a == 0.f || color.r > 0.99f && color.b > 0.99f && color.g == 0.f)
    {
        discard; // 아무것도 안찍겠다(파이프라인 과정 여기서 끝낸다) (출력 자체를 안함(AlphaBlending과 다르게 깊이출력 또한 출력 x)
    }*/

    // 아무것도 안찍겠다(파이프라인 과정 여기서 끝낸다) (출력 자체를 안함(AlphaBlending과 다르게 깊이출력 또한 출력 x)
    if (vColor.a == 0.f) discard;

    // TODO : 이 if문 지우기 (ConstData testing)
    if (g_int_0 == 1)
        vColor.r *= 2.f;
    
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