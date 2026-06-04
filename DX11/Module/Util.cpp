#include "pch.h"
#include "Util.h"

#include "GameEngine/07.EditorUI/12.DebugLogUI/DebugLogUI.h"

namespace DebugUtil
{
#ifdef _DEBUG
    void AddDebugLog(const wstring& _msg, const Vec4& _Color, float _TotalLifeTime)
    {
        const wstring debugString = _msg + L"\n";
        OutputDebugStringW(debugString.c_str());

        DebugLogUIData LogData{};
        LogData.LogStr = string(debugString.begin(), debugString.end());
        LogData.Color = _Color;
        LogData.TotalLifeTime = _TotalLifeTime;

        DebugLogUI::AddDebugInfo(LogData);
    }

    void AddDebugLog(const string& _msg, const Vec4& _Color, float _TotalLifeTime)
    {
        AddDebugLog(wstring(_msg.begin(), _msg.end()), _Color, _TotalLifeTime);
    }

    void SetPermanentDebugLog(const string& _LogKey, const string& _Msg, const Vec4& _Color)
    {
        DebugLogUIData LogData{};
        LogData.LogStr = _Msg;
        LogData.Color = _Color;

        DebugLogUI::SetPermanentDebugInfo(_LogKey, LogData);
    }
#else
    void AddDebugLog(const wstring&, const Vec4&, float) {}
    void AddDebugLog(const string&, const Vec4&, float) {}
    void SetPermanentDebugLog(const string&, const string&, const Vec4&) {}
#endif
}

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

Vec3 Lerp(const Vec3& _Src, const Vec3& _Dst, float _Alpha)
{
    return { Lerp(_Src.x, _Dst.x, _Alpha), Lerp(_Src.y, _Dst.y, _Alpha), Lerp(_Src.z, _Dst.z, _Alpha) };
}

Vec4 Lerp(const Vec4& _Src, const Vec4& _Dst, float _Alpha)
{
    return { Lerp(_Src.x, _Dst.x, _Alpha), Lerp(_Src.y, _Dst.y, _Alpha), Lerp(_Src.z, _Dst.z, _Alpha), Lerp(_Src.w, _Dst.w, _Alpha) };
    
}

Vec2 MappingToNewRangeUnclamped(const Vec2& _Src, const Vec2& _SrcMin, const Vec2& _SrcMax, const Vec2& _DstMin, const Vec2& _DstMax)
{
    const float X = MappingToNewRangeUnclamped(_Src.x, _SrcMin.x, _SrcMax.x, _DstMin.x, _DstMax.x);
    const float Y = MappingToNewRangeUnclamped(_Src.y, _SrcMin.y, _SrcMax.y, _DstMin.y, _DstMax.y);
    return {X, Y};
}

Vec3 MappingToNewRangeUnclamped(const Vec3& _Src, const Vec3& _SrcMin, const Vec3& _SrcMax, const Vec3& _DstMin, const Vec3& _DstMax)
{
    const float X = MappingToNewRangeUnclamped(_Src.x, _SrcMin.x, _SrcMax.x, _DstMin.x, _DstMax.x);
    const float Y = MappingToNewRangeUnclamped(_Src.y, _SrcMin.y, _SrcMax.y, _DstMin.y, _DstMax.y);
    const float Z = MappingToNewRangeUnclamped(_Src.z, _SrcMin.z, _SrcMax.z, _DstMin.z, _DstMax.z);
    return {X, Y, Z};
}

Vec2 MappingToNewRangeClamped(const Vec2& _Src, const Vec2& _SrcMin, const Vec2& _SrcMax, const Vec2& _DstMin, const Vec2& _DstMax)
{
    const float X = MappingToNewRangeClamped(_Src.x, _SrcMin.x, _SrcMax.x, _DstMin.x, _DstMax.x);
    const float Y = MappingToNewRangeClamped(_Src.y, _SrcMin.y, _SrcMax.y, _DstMin.y, _DstMax.y);
    return {X, Y};
}

Vec3 MappingToNewRangeClamped(const Vec3& _Src, const Vec3& _SrcMin, const Vec3& _SrcMax, const Vec3& _DstMin, const Vec3& _DstMax)
{
    const float X = MappingToNewRangeClamped(_Src.x, _SrcMin.x, _SrcMax.x, _DstMin.x, _DstMax.x);
    const float Y = MappingToNewRangeClamped(_Src.y, _SrcMin.y, _SrcMax.y, _DstMin.y, _DstMax.y);
    const float Z = MappingToNewRangeClamped(_Src.z, _SrcMin.z, _SrcMax.z, _DstMin.z, _DstMax.z);
    return {X, Y, Z};
}

bool CheckProbability(float _Probability)
{
    return GetRandom<float>(0.f, 1.f) < _Probability;
}

bool CheckProbabilityPercent(float _Percent)
{
    return GetRandom<float>(0.f, 100.f) < _Percent;
}

EDIRECTION GetEightDirection(const Vec3& _Vector)
{
    return GetEightDirection(ToVec2(_Vector));
}

EDIRECTION GetEightDirection(const Vec2& _Vector)
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

float GetEightDirectionToAngle(EDIRECTION _Direction)
{
    int DirectionToInt = static_cast<int>(_Direction);
    return DirectionToInt * XM_PIDIV4;    
}

SIXTEEN_DIRECTION GetSixteenDirection(const Vec3& _Vector)
{
    return GetSixteenDirection(ToVec2(_Vector));
}

SIXTEEN_DIRECTION GetSixteenDirection(const Vec2& _Vector)
{
    const float Angle = GetVectorAngle(_Vector);
    float Degree = XMConvertToDegrees(Angle);

    // 0 ~ 22.5
    Degree += 22.5f;
    
    if (Degree >= 360.f) Degree -= 360.f;
    
    const int DirIndex = static_cast<int>(Degree / 22.5f); // 이걸 22.5로 나누면 0 ~ 15 사이의 정수 인덱스로 나누어 떨어짐
    return static_cast<SIXTEEN_DIRECTION>(DirIndex);
}

string ToString(const Vec2& _V)
{
    return "{ " + to_string(_V.x) + ", " + to_string(_V.y) + " }";
}

string ToString(const Vec3& _V)
{
    return "{ " + to_string(_V.x) + ", " + to_string(_V.y) + " }";
}
