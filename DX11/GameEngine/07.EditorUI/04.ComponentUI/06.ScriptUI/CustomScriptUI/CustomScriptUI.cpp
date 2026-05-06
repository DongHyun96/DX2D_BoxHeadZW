#include "pch.h"
#include "CustomScriptUI.h"

#include "GameEngine/03.Manager/09.EditorMgr/EditorMgr.h"

CustomScriptUI::CustomScriptUI(const string& _Name, SCRIPT_TYPE _CustomScriptType)
    : ComponentUI(COMPONENT_TYPE::SCRIPT, _Name), m_CustomScriptType(_CustomScriptType)
{
}

CustomScriptUI::~CustomScriptUI()
{
}

void CustomScriptUI::OnRemoveComponentConfirmed(bool _Confirmed)
{
    if (_Confirmed)
    {
        if (GetTargetObject()->RemoveScript(m_TargetScript))
            DebugUtil::AddDebugLog("Remove Custom Script Succeeded", DEF_COLOR_RED, 10.f);    
        
        EditorMgr::GetInst()->SetTargetObjectToInspectors(GetTargetObject());
    }
}

void CustomScriptUI::SetTargetObject(const Ptr<GameObject>& _TargetObject)
{
    m_TargetObject = _TargetObject;
    
    if (!m_TargetObject)
    {
        SetActive(false);
        return;
    }
    
    // Valid한 TargetObject가 들어온 경우 ->
    // TargetObject가 일치하는 CustomScript를 소유하고 있다면, 이 CustomScriptUI 활성화 처리
    if (CScript* Script = m_TargetObject->GetScriptComponent(m_CustomScriptType))
    {
        m_TargetScript = Script;
        SetActive(true);
    }
    else SetActive(false);
}
