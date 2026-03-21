#include "pch.h"
#include "Util.h"

Vec4 GetRandomColor(bool _bUseRandomAlpha)
{
    return Vec4
    (
        rand() / static_cast<float>(RAND_MAX),
        rand() / static_cast<float>(RAND_MAX),
        rand() / static_cast<float>(RAND_MAX),
        (_bUseRandomAlpha) ? rand() / static_cast<float>(RAND_MAX) : 1.f
    );
}

float Lerp(float _Src, float _Dst, float _Alpha)
{
    if (_Alpha <= 0.f) return _Src;
    if (_Alpha >= 1.f) return _Dst;
    
    return _Src * (1.f - _Alpha) + _Dst * _Alpha;
}