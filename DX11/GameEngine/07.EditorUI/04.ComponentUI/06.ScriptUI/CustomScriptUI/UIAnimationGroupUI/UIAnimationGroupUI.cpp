#include "pch.h"
#include "UIAnimationGroupUI.h"

#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/03.Manager/09.EditorMgr/EditorMgr.h"
#include "GameEngine/07.EditorUI/01.Inspector/Inspector.h"
#include "Source/ScriptMgr.h"
#include "Source/Scripts/UIScript/UIAnimation/CUIAnimationGroup.h"
#include "Source/Scripts/UIScript/UIAnimation/CUIAnimation.h"

namespace
{
    string ToString(const wstring& _WString)
    {
        if (_WString.empty()) return "UnNamed";
        return string(_WString.begin(), _WString.end());
    }

    string GetGameObjectName(GameObject* _GameObject)
    {
        if (!_GameObject) return "<Missing>";
        return ToString(_GameObject->GetName());
    }
}

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
    const bool         bCanEdit = (LevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::STOP);

    ImGui::BeginDisabled(!bCanEdit);
    
    // 1. StopAll 버튼
    if (ImGui::Button("Stop All", ImVec2(-1, 0)))
        AnimGroup->StopAll();

    ImGui::Separator();
    
    // 2. 애니메이션 추가 섹션
    Tick_AddAnimationToGroup(AnimGroup);
    
    ImGui::Separator();

    // 3. 목록 표시
    Tick_ShowAnimList(AnimGroup);
    
    ImGui::EndDisabled();
}

void UIAnimationGroupUI::SetTargetObject(const Ptr<GameObject>& _TargetObject)
{
    // 이전 TargetObject가 존재했다면, StopAll 처리
    GameObject* PrevTargetObject = GetTargetObject().Get();
    CustomScriptUI::SetTargetObject(_TargetObject);

    // Level Play 중인 상태일 경우, 처리 x
    if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::STOP) return;
    
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

void UIAnimationGroupUI::Tick_AddAnimationToGroup(CUIAnimationGroup* _AnimGroup)
{
    ImGui::Text("Add Animation");
    ImGui::InputText("Key", m_AddAnimKeyBuffer, sizeof(m_AddAnimKeyBuffer));

    // GameObject Drop Target
    ImGui::Button("Drop GameObject Here", ImVec2(-1, 0));
    
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Outliner"))
        {
            if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("Outliner"))
            {
                if (!TreeUI::IsPayloadMultiData(Payload))
                {
                    DWORD_PTR Data = *static_cast<DWORD_PTR*>(Payload->Data);
                    if (Ptr<GameObject> ReceivedObj = reinterpret_cast<GameObject*>(Data))
                    {
                        const string keyStr = m_AddAnimKeyBuffer;
                        const wstring key(keyStr.begin(), keyStr.end());
                        _AnimGroup->AddAnimation(key, ReceivedObj.Get());
                        m_AddAnimKeyBuffer[0] = '\0'; // 버퍼 초기화
                    }
                }
            }
        }
        ImGui::EndDragDropTarget();
    }
}

void UIAnimationGroupUI::Tick_ShowAnimList(CUIAnimationGroup* _AnimGroup)
{
    const map<wstring, GameObjectRefHolder>& animObjs = _AnimGroup->m_mapAnimationGameObjects;

    if (ImGui::BeginTable("AnimationList", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable))
    {
        ImGui::TableSetupColumn("Key",      ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Object",   ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Play",     ImGuiTableColumnFlags_WidthFixed, 50.f);
        ImGui::TableSetupColumn("Delete",   ImGuiTableColumnFlags_WidthFixed, 50.f);
        ImGui::TableHeadersRow();

        wstring pendingDeleteKey = L"";

        for (auto& pair : animObjs)
        {
            ImGui::PushID(ToString(pair.first).c_str());
            ImGui::TableNextRow();
            
            ImGui::TableSetColumnIndex(0);
            ImGui::Text(ToString(pair.first).c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::Text(GetGameObjectName(pair.second.GetGameObject()).c_str());

            ImGui::TableSetColumnIndex(2);
            
            if (ImGui::Button("Play(Default)"))
                _AnimGroup->PlayAnimation(pair.first, UIAnimEndHandling::DEFAULT);
            
            if (ImGui::Button("Play(AutoStop)"))
                _AnimGroup->PlayAnimation(pair.first, UIAnimEndHandling::BACK_TO_STOP);
            
            if (ImGui::Button("Play(Loop)"))
                _AnimGroup->PlayAnimation(pair.first, UIAnimEndHandling::LOOP);

            ImGui::TableSetColumnIndex(3);
            
            if (ImGui::Button("Del"))
                pendingDeleteKey = pair.first;

            ImGui::PopID();
        }

        if (!pendingDeleteKey.empty())
        {
            _AnimGroup->RemoveAnimationByKey(pendingDeleteKey);
        }

        ImGui::EndTable();
    }
}
