#pragma once
#include "CGameUI.h"
#include <string>

class CText : public CGameUI
{
private:
    wstring m_strText{};
    wstring m_FontStyle = L"None";
    float   m_fFontSize = 20.f;
    Vec4    m_Color{};

public:
    CText();
    CText(const CText& _Origin);
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
