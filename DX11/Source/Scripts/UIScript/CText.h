#pragma once
#include "CGameUI.h"
#include <string>

#include "GameEngine/03.Manager/10.FontMgr/FontMgr.h"

class CText : public CGameUI
{
private:
    wstring m_strText{};
    wstring m_FontStyle = L"None";
    float   m_fFontSize = 20.f;
    Vec4    m_Color{};
    
private:
    
    FONT_ALIGN m_Align = FONT_ALIGN::LEFT;
    

public:
    CText();
    virtual ~CText() override;
    CLONE(CText);

public:
    virtual void Init() override;
    virtual void Tick() override;
public:
    
    void Render();
    
public:
    
    void SetText(const wstring& _Text) { m_strText = _Text; }
    void SetAlpha(float _Alpha) { m_Color.w = _Alpha; }
    void SetColor(const Vec4& _Color) { m_Color = _Color; }
    
    const Vec4& GetColor() const { return m_Color; }
    float GetAlpha() const { return m_Color.w; }
    
    /*void SetFontSize(float _FontSize) { m_fFontSize = _FontSize; }
    void SetFontStyle(const wstring& _FontStyle) { m_FontStyle = _FontStyle; }
    void SetColor(const Vec4& _Color) { m_Color = _Color; }
    void SetAlign(FONT_ALIGN _Align) { m_Align = _Align; }*/
    
public:

    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
    
};
