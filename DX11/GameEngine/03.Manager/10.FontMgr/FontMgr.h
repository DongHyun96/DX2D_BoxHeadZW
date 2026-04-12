#pragma once

#include <FontEngine/FW1FontWrapper.h>
#include <FontEngine/FW1CompileSettings.h>
#include <dwrite_3.h> // DirectWrite 추가
#include <unordered_map>

#ifdef _DEBUG
#pragma comment(lib, "FontEngine/FW1FontWrapper_debug.lib")
#else
#pragma comment(lib, "FontEngine/FW1FontWrapper.lib")
#endif

#define FONT_RGBA(r, g, b, a) (((((BYTE)a << 24 ) | (BYTE)b << 16) | (BYTE)g << 8) | (BYTE)r)

struct CustomFontInfo
{
    wstring FamilyName;       // 예: "Pretendard"
    DWRITE_FONT_WEIGHT Weight;     // 예: DWRITE_FONT_WEIGHT_BOLD
    DWRITE_FONT_STYLE Style;       // 예: DWRITE_FONT_STYLE_NORMAL
};

class FontMgr : public Singleton<FontMgr>
{
    SINGLE(FontMgr);
private:
    IFW1Factory* m_FW1Factory{};
    IFW1FontWrapper* m_FontWrapper{};

    // DWrite 커스텀 컬렉션용 멤버 변수
    IDWriteFactory5* m_pDWriteFactory5{};
    IDWriteFontCollection1* m_pCustomCollection{};

    // 텍스트 포맷 캐싱 컨테이너 (매 프레임 생성 방지 및 성능 최적화)
    unordered_map<wstring, IDWriteTextFormat*> m_mapTextFormat;
    
private:

    unordered_map<wstring, CustomFontInfo> m_mapFontInfo;

    vector<wstring> m_vecFontPath;
    vector<wstring> m_vecFontName; // UI 표시용 Full Name들이 담길 곳

public:
    void Init();
    
    // _FontPath : Content 경로 이후의 경로 (예: L"Font\\MyFont.ttf")
    // _FontName : 폰트 파일 내부의 실제 이름 (예: L"Galmuri11")
    void LoadFont(const wstring& _FontPath, const wstring& _FontName);

    // 기본 DrawFont
    void DrawFont(const wchar_t* _pStr, float _fPosX, float _fPosY, float _fFontSize, UINT _Color);
    void DrawFont(const wchar_t* _pStr, float _fPosX, float _fPosY, float _fFontSize, const Vec4& _Color);

    // 폰트 스타일이 적용 가능한 확장 DrawFont
    void DrawFont(const wchar_t* _pStr, const wchar_t* _pFontName, float _fPosX, float _fPosY, float _fFontSize, UINT _Color, 
                  DWRITE_FONT_WEIGHT _Weight = DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE _Style = DWRITE_FONT_STYLE_NORMAL);
    
    void DrawFont(const wchar_t* _pStr, const wchar_t* _pFontName, float _fPosX, float _fPosY, float _fFontSize, const Vec4& _Color, 
                  DWRITE_FONT_WEIGHT _Weight = DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE _Style = DWRITE_FONT_STYLE_NORMAL);

    const vector<wstring>& GetFontNames() const { return m_vecFontName; }
};