#include "pch.h"
#include "CustomScriptUI.h"

#include "GameEngine/03.Manager/09.EditorMgr/EditorMgr.h"

CustomScriptUI::CustomScriptUI(const string& _Name)
    : ComponentUI(COMPONENT_TYPE::SCRIPT, _Name)
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
