#pragma once
#include "GameEngine/07.EditorUI/EditorUI.h"

class ConfirmUI : public EditorUI
{
private:
    
    string              m_WarningText{};
    Entity*             m_EntityInst{}; // Delegate Binding된 객체
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
    void SetDelegate(Entity* _Inst, DELEGATE_BOOL _MemFunc) { m_EntityInst = _Inst; m_MemFunc = _MemFunc; }
    
private:
    
    virtual void Deactivate() override;
    
};
