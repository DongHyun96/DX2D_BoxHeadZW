#pragma once
#include "GameEngine/07.EditorUI/EditorUI.h"

class ConfirmUI : public EditorUI
{
private:
    
    string              m_WarningText{};
    
    EditorUI*           m_Inst{};     // Delegate Binding된 객체
    DELEGATE_BOOL       m_MemFunc{};  // Delegate Binding된 callback 함수

public:
    
    ConfirmUI();
    virtual ~ConfirmUI() override;
    
public:
    
    void Tick_UI() override;
    
private:
    
    void OnYes();
    void OnNo();
    
public:

    void SetWarningText(const string& _WarningText) { m_WarningText = _WarningText; }
    void AddDelegate(EditorUI* _Inst, DELEGATE_BOOL _MemFunc) { m_Inst = _Inst; m_MemFunc = _MemFunc; }
    
private:
    
    virtual void Deactivate() override;
    
};
