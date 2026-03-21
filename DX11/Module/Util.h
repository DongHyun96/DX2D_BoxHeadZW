#pragma once

namespace DebugUtil
{
    
#ifdef _DEBUG
    /// <summary> 디버거 출력에 Debug log 남기기 </summary>
    static void AddDebugLog(const std::wstring& msg)
    {
        const wstring debugString = msg + L"\n";
        OutputDebugStringW(debugString.c_str());
    }
#else
    static void AddDebugLog(const std::wstring&) {}
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
    if (_SrcMin > _SrcMax)
    {
        DebugUtil::AddDebugLog(L"[MappingToNewRange] : wrong Src range received.");
        return T();
    }
    
    if (_DstMin > _DstMax)
    {
        DebugUtil::AddDebugLog(L"[MappingToNewRange] : wrong Dst range received.");
        return T();
    }
    
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
