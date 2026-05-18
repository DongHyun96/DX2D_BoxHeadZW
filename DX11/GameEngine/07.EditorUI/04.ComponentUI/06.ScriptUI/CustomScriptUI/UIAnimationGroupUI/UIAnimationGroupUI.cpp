#include "pch.h"
#include "UIAnimationGroupUI.h"

#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/03.Manager/09.EditorMgr/EditorMgr.h"
#include "GameEngine/07.EditorUI/01.Inspector/Inspector.h"
#include "Source/ScriptMgr.h"
#include "Source/Scripts/UIScript/UIAnimation/CUIAnimationGroup.h"

UIAnimationGroupUI::UIAnimationGroupUI()
    : CustomScriptUI("UIAnimationGroup(Script)", SCRIPT_TYPE::UIANIMATIONGROUP)
{
    m_ComponentTitle = "AnimGroup";
}

UIAnimationGroupUI::~UIAnimationGroupUI()
{
}

void UIAnimationGroupUI::Tick_UI()
{
    CustomScriptUI::Tick_UI();

    CUIAnimationGroup* AnimGroup = static_cast<CUIAnimationGroup*>(GetScript());
    const bool bCanEdit          = (LevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::STOP);
}

void UIAnimationGroupUI::OnRemoveComponentConfirmed(bool _Confirmed)
{
    CustomScriptUI::OnRemoveComponentConfirmed(_Confirmed);
}

void UIAnimationGroupUI::SetTargetObject(const Ptr<GameObject>& _TargetObject)
{
    // 이전 TargetObject가 존재했다면, StopAll 처리
    GameObject* PrevTargetObject = GetTargetObject().Get();
    CustomScriptUI::SetTargetObject(_TargetObject);

    // 이전 TargetObject가 없었다면, 따로 Animation Stop 처리할 일 x
    if (!PrevTargetObject) return;

    // 이전 TargetObject의 Animation 모두 Stop 처리

    for (const Ptr<Inspector>& inspector : EditorMgr::GetInst()->GetInspectors())
    {
        // 아직 해당 GO의 Focus가 남은 Inspector가 존재하는 상황
        // 이 때에는 StopAll 처리 x
        if (inspector->GetTargetObject() == PrevTargetObject) return;
    }

    // 해당 GO의 Focus를 모두 잃은 상태, AnimationStopAll 처리
    if (const Ptr<CUIAnimationGroup>& UIAnimationGroup = PrevTargetObject->GetScriptComponent<CUIAnimationGroup>())
        UIAnimationGroup->StopAll();
}
