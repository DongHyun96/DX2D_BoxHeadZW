#pragma once
#include "CGameUI.h"
#include <functional>

class CButton : public CGameUI
{
private:
    Ptr<ATexture> m_IdleTex;
    Ptr<ATexture> m_HoverTex;
    Ptr<ATexture> m_ClickedTex;
    
    function<void()> m_CallBackFunc;

    bool m_bMouseOn;
    bool m_bLBtnDown;

public:
    CButton();
    CButton(const CButton& _Origin);
    virtual ~CButton() override;
    CLONE(CButton);
    
public:
    virtual void Init() override;
    virtual void Begin() override;
    virtual void Tick() override;

public:
    virtual void OnMouseOn();
    virtual void OnMouseOut();
    virtual void OnLBtnDown();
    virtual void OnLBtnUp();
    virtual void OnLBtnClicked();

    void SetCallBack(std::function<void()> _Func) { m_CallBackFunc = _Func; }

    void SetIdleTex(const Ptr<ATexture>& _Tex) { m_IdleTex = _Tex; }
    void SetHoverTex(const Ptr<ATexture>& _Tex) { m_HoverTex = _Tex; }
    void SetClickedTex(const Ptr<ATexture>& _Tex) { m_ClickedTex = _Tex; }

private:
    bool CheckMouseOn();

public:
    void SaveToLevelFile(FILE* _File) override;
    void LoadFromLevelFile(FILE* _File) override;
};
