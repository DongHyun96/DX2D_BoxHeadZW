#include "pch.h"
#include "ComponentUI.h"

#include "GameEngine/03.Manager/09.EditorMgr/EditorMgr.h"
#include "GameEngine/05.GameObject/GameObject.h"
#include "GameEngine/07.EditorUI/10.ConfirmUI/ConfirmUI.h"

ComponentUI::ComponentUI(COMPONENT_TYPE _Type, const string& _Name)
    : EditorUI(_Name) , m_ComType(_Type)
{
}

ComponentUI::~ComponentUI()
{
}

void ComponentUI::Tick_UI()
{
    ColoredButtonTitle(m_ComponentTitle);
    
    ImGui::SameLine(150);
    
    string RemoveButtonString = "Remove component##" + GetUIKey(); 
    
    if (ImGui::Button(RemoveButtonString.c_str()))
    {
        Ptr<ConfirmUI> pUI = dynamic_cast<ConfirmUI*>(EditorMgr::GetInst()->FindUI("ConfirmUI").Get());
        assert(pUI.Get());

        pUI->SetWarningText("Are you sure you want to remove this component?");
        pUI->AddDelegate(this, static_cast<DELEGATE_BOOL>(&ComponentUI::OnConfirmClicked));
        pUI->SetActive(true);
    }
    
    ImGui::Spacing(); ImGui::Spacing();
}

void ComponentUI::SetTargetObject(const Ptr<GameObject>& _TargetObject)
{
    m_TargetObject = _TargetObject;

    if (!m_TargetObject)
    {
        SetActive(false);
        return;
    }

    if (static_cast<UINT>(m_ComType) >= static_cast<UINT>(COMPONENT_TYPE::END))
    {
        SetActive(true);
        return;
    }

    SetActive(m_TargetObject->GetComponent(m_ComType) != nullptr);
}

void ComponentUI::OnConfirmClicked(bool _Confirmed)
{
    if (_Confirmed)
    {
        m_TargetObject->RemoveComponent(m_ComType);
        EditorMgr::GetInst()->SetTargetObjectToInspectors(m_TargetObject);
    }
}
