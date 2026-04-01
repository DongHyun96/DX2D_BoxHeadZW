#pragma once

enum class SIXTEEN_DIRECTION;

namespace DebugUtil
{
    
#ifdef _DEBUG
    /// <summary> 디버거 출력에 Debug log 남기기 </summary>
    void AddDebugLog(const wstring& _msg, const Vec4& _Color = Vec4(1.f, 1.f, 1.f, 1.f), float _TotalLifeTime = 3.f);
    void AddDebugLog(const string& _msg, const Vec4& _Color = Vec4(1.f, 1.f, 1.f, 1.f), float _TotalLifeTime = 3.f);
    
    void SetPermanentDebugLog(const string& _LogKey, const string& _Msg, const Vec4& _Color);
    
#else
    void AddDebugLog(const wstring& _msg, const Vec4& _Color = Vec4(1.f, 1.f, 1.f, 1.f), float _TotalLifeTime = 3.f) {}
    void AddDebugLog(const string& _msg, const Vec4& _Color = Vec4(1.f, 1.f, 1.f, 1.f), float _TotalLifeTime = 3.f) {}
    void SetPermanentDebugLog(const string& _LogKey, const string& _Msg, const Vec4& _Color) {}
#endif
}


// 0 ~ 1 제한
// float Saturate(float _Data);


/// <summary>
/// RandomColor 색상 구하기
/// </summary>
/// <param name="_bUseRandomAlpha"> : Alpha값까지 Random으로 정할지 결정, false라면 alpha는 1.f </param>
/// <returns> : RandomColor </returns>
Vec4 GetRandomColor(bool _bUseRandomAlpha = false);

float Lerp(float _Src, float _Dst, float _Alpha);

/// <summary>
/// 새로운 Range로 Mapping한 값 구하기
/// </summary>
/// <param name="_Num"> : Mapping처리할 값 </param>
/// <param name="_SrcMin"> : 기존 Range Min </param>
/// <param name="_SrcMax"> : 기존 Range Max </param>
/// <param name="_DstMin"> : 새로 Mapping할 범위의 Min </param>
/// <param name="_DstMax"> : 새로 Mapping할 범위의 Max </param>
/// <returns> : 새로 Mapping처리된 값 </returns>
template<typename T>
T MappingToNewRange(const T& _Num, const T& _SrcMin, const T& _SrcMax, const T& _DstMin, const T& _DstMax)
{
    /*if (_SrcMin > _SrcMax)
    {
        DebugUtil::AddDebugLog(L"[MappingToNewRange] : wrong Src range received.");
        return T();
    }
    
    if (_DstMin > _DstMax)
    {
        DebugUtil::AddDebugLog(L"[MappingToNewRange] : wrong Dst range received.");
        return T();
    }*/
    
    if (_SrcMin == _SrcMax)
        return static_cast<T>((_Num - _SrcMin) * (_DstMax - _DstMin) + _DstMin);
    
    return static_cast<T>((_Num - _SrcMin) * (_DstMax - _DstMin) / (_SrcMax - _SrcMin) + _DstMin);
}

template <typename T>
T GetRandom(const T& _Start, const T& _End)
{
    if (_Start > _End)
    {
        DebugUtil::AddDebugLog(L"[GetRandom] wrong range received.");
        return _Start;
    }

    if (_Start == _End) return _Start;

    static thread_local mt19937 gen{ random_device{}() };

    if constexpr (is_integral_v<T>)
    {
        uniform_int_distribution<T> dist(_Start, _End);
        return dist(gen);
    }
    else if constexpr (is_floating_point_v<T>)
    {
        uniform_real_distribution<T> dist(_Start, _End);
        return dist(gen);
    }
    else
    {
        static_assert(is_arithmetic_v<T>, "GetRandom supports only integral or floating-point types");
        return T{};
    }
}

template<typename T>
T PickRandom(const vector<T>& _Vec)
{
    if (_Vec.empty())
    {
        assert(nullptr);
        return T();
    }
    
    int RandomIdx = GetRandom<int>(0, _Vec.size() - 1);
    return _Vec[RandomIdx];
}

/// <summary>
/// Z값을 제외한 X, Y값으로 Vec3에서 Vec2값 반환
/// </summary>
static Vec2 ToVec2(const Vec3& _V) { return {_V.x, _V.y}; }
static Vec3 ToVec3(const Vec2& _V) { return {_V.x, _V.y, 0.f}; } // z값 0으로 고정

static float ConvertToAngle(float _Degree) { return _Degree * (XM_PI / 180.f); }

/// <summary>
/// 주어진 Vector와 Angle만큼 벌어진 Vector 구하기
/// </summary>
static Vec2 GetSpreadVector(const Vec2& _V, const float& _Angle)
{
    if (_Angle == 0.f) return _V;
    
    const float Cos = cosf(_Angle);
    const float Sin = sinf(_Angle);
    
    return {_V.x * Cos - _V.y * Sin, _V.x * Sin + _V.y * Cos};
}


enum class EDIRECTION;

/// <summary>
/// 해당 Vector 방향에 대응되는 EDIRECTION 구하기 
/// 만약 Zero vector를 받았다면 END를 return
/// </summary>
EDIRECTION GetEightDirection(const Vec3& _Vector);
EDIRECTION GetEightDirection(const Vec2& _Vector);


/// <summary>
/// 해당 Eight Direction에 대응되는 Angle값 구하기 
/// </summary>
float GetEightDirectionToAngle(EDIRECTION _Direction);

/// <summary>
/// 해당 Vector 방향에 대응되는 SIXTEEN_DIRECTION 구하기
/// </summary>
SIXTEEN_DIRECTION GetSixteenDirection(const Vec3& _Vector);
SIXTEEN_DIRECTION GetSixteenDirection(const Vec2& _Vector);
