#ifndef _FUNC
#define _FUNC

#include "value.fx"



/// <summary>
/// 
/// </summary>
/// <param name="_LightIdx"></param>
/// <param name="_PixelPos"></param>
/// <returns></returns>
float3 CalcLight2D(int _LightIdx, float3 _PixelPos)
{
#define CUR_LIGHT g_Light2D[_LightIdx]
    
    float3 LightColor = float3(0.f, 0.f, 0.f);
    
    float DistRatio = 1.f; // 광원과 물체의 거리에 따른 빛의 세기 비율
    
    // Directional Light
    if (CUR_LIGHT.Type == 0)
    {
        LightColor = CUR_LIGHT.Color + CUR_LIGHT.Ambient;        
    }
    
    // Point
    else if (CUR_LIGHT.Type == 1)
    {
        LightColor = CUR_LIGHT.Color; 
        
        // 광원에서 픽셀을 향하는 방향벡터(월드공간 기준)
        float3 LightToPixel = _PixelPos - CUR_LIGHT.WorldPos;
        LightToPixel.z = 0; // xy축으로만 거리를 따져야함 (Light2D 환경)

        // 벡터의 길이 측정(광원과 픽셀 사이의 거리)
        float Dist = length(LightToPixel);
        
        // saturate -> 범위를 0~1로 잡아줌
        // 물체와 광원의 거리가 멀어질수록 빛의 세기가 0에 가까워진다.
        // DistRatio = saturate(1.f - (Dist / INPUT_LIGHT.Radius)); // 선형 처리

        float t = saturate(Dist / CUR_LIGHT.Radius);
        DistRatio = cos(t * (PI * 0.5f)); // cos은 0 ~ PI/2 범위에서만
        // DistRatio = saturate(cos(Dist * ((PI * 0.5f) / INPUT_LIGHT.Radius)));
    }

    // Spot
    else
    {
        LightColor = CUR_LIGHT.Color;
        
        float3 LightToPixel = _PixelPos - CUR_LIGHT.WorldPos;
        LightToPixel.z      = 0;
        
        float3 LightToPixelDir  = normalize(LightToPixel);
        float dotProduct        = dot(normalize(CUR_LIGHT.LightDir), LightToPixelDir);
        float cosLimit          = cos(CUR_LIGHT.Angle * 0.5f);
        
        if (dotProduct > cosLimit)
        {
            // 벡터의 길이 측정(광원과 픽셀 사이의 거리)
            float Dist = length(LightToPixel);
        
            // saturate -> 범위를 0~1로 잡아줌
            // 물체와 광원의 거리가 멀어질수록 빛의 세기가 0에 가까워진다.
            // DistRatio = saturate(1.f - (Dist / CUR_LIGHT.Radius)); // 선형 처리

            float t = saturate(Dist / CUR_LIGHT.Radius);
            DistRatio = cos(t * (PI * 0.5f)); // cos은 0 ~ PI/2 범위에서만
        }
        else DistRatio = 0.f;
        
    }
    
    
    return LightColor * DistRatio;
}


#endif
