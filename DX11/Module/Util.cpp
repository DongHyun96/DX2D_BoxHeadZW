#include "pch.h"
#include "Util.h"

#include "GameEngine/07.EditorUI/12.DebugLogUI/DebugLogUI.h"

#ifdef _DEBUG
void DebugUtil::AddDebugLog(const wstring& _msg, const Vec4& _Color, float _TotalLifeTime)
{
    const wstring debugString = _msg + L"\n";
    OutputDebugStringW(debugString.c_str());
        
    DebugLogUIData LogData{};
    LogData.LogStr          = string(debugString.begin(), debugString.end());;
    LogData.Color           = _Color;
    LogData.TotalLifeTime   = _TotalLifeTime;
        
    DebugLogUI::AddDebugInfo(LogData);
}
#else
#endif

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

EDIRECTION GetDirection(const Vec3& _Vector)
{
    Vec2 v = ToVec2(_Vector);
    return GetDirection(v);
}

EDIRECTION GetDirection(const Vec2& _Vector)
{
    Vec2 v = _Vector;
    
    if (v.LengthSquared() == 0.f)
        return EDIRECTION::END;
    
    v.Normalize();
    
    const float Angle = atan2f(v.y, v.x);
    float Degree = XMConvertToDegrees(Angle);
    
    if (Degree < 0.f) Degree += 360.f; // 음수값 방지

    // -22.5 ~ 22.5 -> 0 ~ 45
    Degree += 22.5f;
    if (Degree >= 360.f) Degree -= 360.f;
    
    const int DirIndex = static_cast<int>(Degree / 45.f); // 이걸 45도로 나누면 0 ~ 7 사이의 정수 인덱스로 나누어 떨어짐
    return static_cast<EDIRECTION>(DirIndex);
}


