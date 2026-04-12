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

    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
    
};
