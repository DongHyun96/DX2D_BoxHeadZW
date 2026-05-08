#include "pch.h"
#include "GameUIAnimationUI.h"

#include <algorithm>

#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/03.Manager/09.EditorMgr/EditorMgr.h"
#include "GameEngine/05.GameObject/GameObject.h"
#include "GameEngine/06.Component/RenderComponent/CRenderComponent.h"
#include "GameEngine/07.EditorUI/07.TreeUI/TreeUI.h"
#include "GameEngine/07.EditorUI/10.ConfirmUI/ConfirmUI.h"
#include "Source/ScriptMgr.h"
#include "Source/Scripts/UIScript/CText.h"
#include "Source/Scripts/UIScript/UIAnimation/CUIAnimation.h"
#include "Source/Scripts/UIScript/UIAnimation/UIAnimationData.h"
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

    /*bool ApplyKeyFrameToTarget(GameObject* _TargetObject, const UIAnimKeyFrameData& _KeyFrameData)
    {
        if (!_TargetObject || !_TargetObject->Transform() || !_KeyFrameData.Transform) return false;

        _TargetObject->Transform()->CopyRelativePosScaleRot(_KeyFrameData.Transform);

        if (const Ptr<CText>& Text = _TargetObject->GetScriptComponent<CText>())
            Text->SetColor(_KeyFrameData.TintColor);
        else if (const Ptr<CRenderComponent>& RenderCom = _TargetObject->GetRenderCom())
            RenderCom->GetMaterial()->SetScalar(VEC4_0, _KeyFrameData.TintColor);

        return true;
    }*/
}

GameUIAnimationUI::GameUIAnimationUI()
    : CustomScriptUI("GameUIAnimation(Script)", SCRIPT_TYPE::UIANIMATION)
{
    m_ComponentTitle = "GameUIAnimation";
}

GameUIAnimationUI::~GameUIAnimationUI()
{
}

void GameUIAnimationUI::Tick_UI()
{
    ComponentUI::Tick_UI();

    CUIAnimation* Animation = static_cast<CUIAnimation*>(GetScript());

    // 현재 레벨이 정지(STOP) 상태인지 체크
    const bool bCanEdit = (LevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::STOP);
    
    ImGui::BeginDisabled(!bCanEdit);
    
    // 데이터가 변했을 때를 대비한 Selection 안전장치
    ClearSelectionIfInvalid(Animation);

    float MaxAnimTime = 2.f;
    const vector<UIAnimTrack>& Tracks = Animation->GetTracks();

    for (const UIAnimTrack& Track : Tracks)
    {
        if (!Track.KeyFrames.empty())
        {
            const float LastTime = Track.KeyFrames.back().Time;
            if (LastTime > MaxAnimTime) MaxAnimTime = LastTime;
        }
    }
    MaxAnimTime += m_MaxAnimTimeMargin; // 여백 1초 추가
    
    // 1. 상단 재생/컨트롤 툴바
    RenderToolbar(Animation, bCanEdit, MaxAnimTime);
    ImGui::Separator();

    // 2. 메인 에디터 (좌: 트랙 리스트 / 우: 도프시트 타임라인)
    RenderMainEditor(Animation, bCanEdit, MaxAnimTime);
    ImGui::Separator();

    // 3. 하단 인스펙터 (선택된 트랙/키프레임 편집기)
    RenderBottomInspector(Animation, bCanEdit);

    ImGui::EndDisabled();
    
    // 삭제 예약 처리
    HandlePendingRemovals(Animation);
}

void GameUIAnimationUI::SetTargetObject(const Ptr<GameObject>& _TargetObject)
{
    // 이전 TargetObject가 존재했다면, Animation Stop 처리
    // Outliner ReNew를 통해서도 호출됨 -> Level Play 상태에서의 ReNew 로는 들어오지 않음
    // -> Level Stop시에만 SetTargetObject가 호출됨
    // Inspector 포커스를 잃었을 때, 포커스가 잡힌 CUIAnimation을 들고 있었던 GameObject의 Focus가 모두 사라졌는지 확인해서
    // 모두 사라졌다면, Animation Stop 호출 처리
    GameObject* PrevTargetObject = GetTargetObject().Get();
    CustomScriptUI::SetTargetObject(_TargetObject);

    // 이전 TargetObject가 없었다면, 따로 Animation Stop 처리할 일 x
    if (!PrevTargetObject) return;

    // 이전 CUIAnimation Object Focus를 잃은 경우
    // 현재 띄워져있는 모든 Inspector에 대한 이 GameObject Focus가 사라졌다면, 해당 Animation Stop 처리를 함으로써
    // 레퍼런스로 들고 있었던 GameObject들 원본값으로 되돌리기
    for (const Ptr<Inspector>& inspector : EditorMgr::GetInst()->GetInspectors())
    {
        // 아직 해당 GO의 Focus가 남은 Inspector가 존재하는 상황 (Lock이 걸려있는 오브젝트가 있다거나,
        // 아직 Inspector SetTargetObject Loop가 모두 돌지 않았거나(마지막 오브젝트 Loop가 들어왔을 때 비로소 제대로 해제 처리가 됨)...)
        if (inspector->GetTargetObject() == PrevTargetObject) return;
    }
    
    // 해당 GO의 Focus를 모두 잃은 상태, Animation Stop 처리
    if (Ptr<CUIAnimation> UIAnimationScript = PrevTargetObject->GetScriptComponent<CUIAnimation>())
        UIAnimationScript->Stop();
}

void GameUIAnimationUI::RenderToolbar(CUIAnimation* _Animation, bool _bCanEdit, float _MaxAnimTime)
{
    // 재생 컨트롤
    if (ImGui::Button("Play")) { _Animation->Play(); }
    ImGui::SameLine();
    if (ImGui::Button("Play(AutoStop)")) { _Animation->Play(UIAnimEndHandling::BACK_TO_STOP); }
    ImGui::SameLine();
    if (ImGui::Button("Play(Loop)")) { _Animation->Play(UIAnimEndHandling::LOOP); }
    ImGui::SameLine();
    if (ImGui::Button("Stop")) { _Animation->Stop(); }

    ImGui::SameLine(0, 20.0f);

    // Time indicator 시간 조정
    float CurrentDisplayTime = _Animation->IsPlaying() ? _Animation->GetAnimTimer() : _Animation->GetEditingAnimTimer();
    
    ImGui::BeginDisabled(_Animation->IsPlaying() || !_bCanEdit);
    ImGui::SetNextItemWidth(300.0f);
    if (ImGui::SliderFloat("Time", &CurrentDisplayTime, 0.0f, _MaxAnimTime, "%.3f s"))
        _Animation->SetEditingAnimTime(CurrentDisplayTime);
    ImGui::EndDisabled();
    
    // 상태 텍스트
    if (_Animation->IsPlaying())
        ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "[Playing] Time: %.2f sec", _Animation->GetAnimTimer());
    else
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "[Stopped]");

    // 타임라인 확대/축소 슬라이더
    ImGui::SameLine(ImGui::GetWindowWidth() - 200.0f);
    ImGui::SetNextItemWidth(150.0f);
    ImGui::SliderFloat("Zoom", &m_TimelineScale, 50.0f, 500.0f, "%.0f px/s");
}

void GameUIAnimationUI::RenderMainEditor(CUIAnimation* _Animation, bool _bCanEdit, float _MaxAnimTime)
{
    ImGui::BeginDisabled(!_bCanEdit);
    ImGui::Button("Drop GameObject Here to Add Track", ImVec2(-FLT_MIN, 30.0f));
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("Outliner"))
        {
            if (!TreeUI::IsPayloadMultiData(Payload))
            {
                DWORD_PTR Data = *static_cast<DWORD_PTR*>(Payload->Data);
                if (Ptr<GameObject> ReceivedObj = reinterpret_cast<GameObject*>(Data))
                    _Animation->AddGameObjectToAnimate(ReceivedObj.Get());
            }
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::EndDisabled();

    vector<UIAnimTrack>& Tracks = _Animation->GetTracks();

    if (Tracks.empty())
    {
        ImGui::Text("No tracks.");
        return;
    }
    
    ImVec2 timelineAreaPos = ImGui::GetCursorScreenPos();
    static float s_ActualTimelineStartX = timelineAreaPos.x + 200.0f;

    static ImGuiTableFlags tableFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg;
    if (ImGui::BeginTable("AnimDopeSheet", 2, tableFlags, ImVec2(0.0f, 250.0f)))
    {
        ImGui::TableSetupColumn("Tracks", ImGuiTableColumnFlags_WidthFixed, 200.0f);
        ImGui::TableSetupColumn("Timeline", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        
        for (int TrackIdx = 0; TrackIdx < static_cast<int>(Tracks.size()); ++TrackIdx)
        {
            UIAnimTrack& Track = Tracks[TrackIdx];
            GameObject* TargetObj = Track.TargetObjectReference.GetGameObject();

            ImGui::TableNextRow();

            // --- [1열] 아웃라이너 ---
            ImGui::TableSetColumnIndex(0);
            ImGui::PushID(TrackIdx);
            
            bool bIsSelectedTrack = (m_SelectedTrackIdx == TrackIdx);
            string TrackName = "##" + to_string(TrackIdx);
            
            if (ImGui::Selectable(TrackName.c_str(), bIsSelectedTrack, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap))
            {
                m_SelectedTrackIdx = TrackIdx;
                m_SelectedKeyIdx = -1;
            }
            
            ImGui::SameLine();
            ImGui::Text(GetGameObjectName(TargetObj).c_str());
            ImGui::PopID();

            // --- [2열] 도프시트 타임라인 ---
            ImGui::TableSetColumnIndex(1);
            
            ImVec2 cellPos = ImGui::GetCursorScreenPos();
            float cellHeight = ImGui::GetTextLineHeightWithSpacing();

            s_ActualTimelineStartX = cellPos.x;

            for (float t = 0.0f; t <= _MaxAnimTime; t += 0.5f)
            {
                float x = cellPos.x + (t * m_TimelineScale);
                drawList->AddLine(ImVec2(x, cellPos.y), ImVec2(x, cellPos.y + cellHeight), IM_COL32(100, 100, 100, 50));
            }

            const int keyCount = static_cast<int>(Track.KeyFrames.size());
            for (int KeyIdx = 0; KeyIdx < keyCount; ++KeyIdx)
            {
                const float time = Track.KeyFrames[KeyIdx].Time;
                const float keyX = cellPos.x + (time * m_TimelineScale);
                const float keyY = cellPos.y + (cellHeight * 0.5f);

                ImGui::SetCursorScreenPos(ImVec2(keyX - 6.0f, cellPos.y));
                ImGui::PushID(TrackIdx * 1000.f + KeyIdx);
                ImGui::InvisibleButton("##Key", ImVec2(12.0f, cellHeight));

                // 다이아몬드 모양 키프레임 기능
                if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
                {
                    m_SelectedTrackIdx = TrackIdx;
                    m_SelectedKeyIdx = KeyIdx;

                    const float mouseXInTimeline = ImGui::GetIO().MousePos.x - cellPos.x;
                    const float newTime          = max(mouseXInTimeline / m_TimelineScale, 0.f);

                    if (Track.KeyFrames[KeyIdx].Time != newTime)
                    {
                        Track.KeyFrames[KeyIdx].Time = newTime;
                        m_SelectedKeyIdx = _Animation->SortKeyFrames(TrackIdx, m_SelectedKeyIdx);
                    }
                }

                if (ImGui::IsItemClicked())
                {
                    m_SelectedTrackIdx = TrackIdx;
                    m_SelectedKeyIdx = KeyIdx;
                }
    
                ImGui::PopID();

                bool bIsSelectedKey = (m_SelectedTrackIdx == TrackIdx && m_SelectedKeyIdx == KeyIdx);
                ImU32 keyColor = bIsSelectedKey ? IM_COL32(255, 50, 50, 255) : IM_COL32(255, 165, 0, 255);

                const ImVec2 p1(keyX, keyY - 5.0f);
                const ImVec2 p2(keyX + 5.0f, keyY);
                const ImVec2 p3(keyX, keyY + 5.0f);
                const ImVec2 p4(keyX - 5.0f, keyY);
                
                drawList->AddQuadFilled(p1, p2, p3, p4, keyColor);
                drawList->AddQuad(p1, p2, p3, p4, IM_COL32(0, 0, 0, 255), 1.f);
            }
        }

        // Current Time Indicator

        const float currentDisplayTime = _Animation->IsPlaying() ? _Animation->GetAnimTimer() : _Animation->GetEditingAnimTimer();
        const float scrubberX          = s_ActualTimelineStartX + (currentDisplayTime * m_TimelineScale);
        ImVec2 tableEndPos       = ImGui::GetCursorScreenPos();
        
        drawList->AddLine
        (
            ImVec2(scrubberX, timelineAreaPos.y), 
            ImVec2(scrubberX, tableEndPos.y), 
            IM_COL32(0, 255, 0, 255), 2.f
        );

        ImVec2 triP1(scrubberX, timelineAreaPos.y);
        ImVec2 triP2(scrubberX - 6.f, timelineAreaPos.y - 12.f);
        ImVec2 triP3(scrubberX + 6.f, timelineAreaPos.y - 12.f);
        drawList->AddTriangleFilled(triP1, triP2, triP3, IM_COL32(0, 255, 0, 255));

        ImGui::EndTable();
    }
}

void GameUIAnimationUI::RenderBottomInspector(CUIAnimation* _Animation, bool _bCanEdit)
{
    vector<UIAnimTrack>& Tracks = _Animation->GetTracks();

    // 선택된 대상이 없을 때
    if (m_SelectedTrackIdx < 0 || m_SelectedTrackIdx >= Tracks.size())
    {
        ImGui::TextDisabled("Select a track or keyframe in the dope sheet to edit properties.");
        return;
    }

    UIAnimTrack& SelectedTrack = Tracks[m_SelectedTrackIdx];
    GameObject* TargetObj = SelectedTrack.TargetObjectReference.GetGameObject();

    ImGui::Text("Inspector: [%s]", GetGameObjectName(TargetObj).c_str());
    
    // --- [1] 트랙 레벨 기능 (트랙 선택 시 + 항상 보임) ---
    ImGui::BeginDisabled(!_bCanEdit);
    
    // 원본 상태 복사/적용
    if (ImGui::Button("Capture Origin (from Target)")) 
        SelectedTrack.OriginalStateData.SetAnimDataFromGameObject(TargetObj, -1.f);
    ImGui::SameLine();
    
    // 키프레임 추가 기능
    ImGui::Spacing();
    static float s_NewKeyTime = 0.0f;
    ImGui::SetNextItemWidth(100.0f);
    ImGui::InputFloat("Time##AddKey", &s_NewKeyTime, 0.1f, 1.0f, "%.2f");
    ImGui::SameLine();
    if (ImGui::Button("Add KeyFrame"))
    {
        if (_Animation->AddNewKeyFrame(m_SelectedTrackIdx, s_NewKeyTime))
            s_NewKeyTime += 0.1f;
    }

    ImGui::SameLine(ImGui::GetWindowWidth() - 120.0f);
    if (ImGui::Button("Remove Track"))
    {
        m_RemoveTrackIdx = m_SelectedTrackIdx;
        
        Ptr<ConfirmUI> pUI = dynamic_cast<ConfirmUI*>(EditorMgr::GetInst()->FindUI("ConfirmUI").Get());
        assert(pUI.Get());
                    
        pUI->SetWarningText("Are you sure you want to delete this Track?");
        pUI->SetDelegate(this, static_cast<DELEGATE_BOOL>(&GameUIAnimationUI::OnRemoveTrackConfirmUI));
        pUI->SetActive(true);
    }

    ImGui::EndDisabled();
    ImGui::Separator();

    // --- [2] 키프레임 상세 편집 기능 (키프레임이 선택되었을 때만) ---
    if (m_SelectedKeyIdx >= 0 && m_SelectedKeyIdx < SelectedTrack.KeyFrames.size())
    {
        UIAnimKeyFrameData& KeyFrame = SelectedTrack.KeyFrames[m_SelectedKeyIdx];
        
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "Editing KeyFrame %d (Time: %.2f sec)", m_SelectedKeyIdx, KeyFrame.Time);
        
        ImGui::BeginDisabled(!_bCanEdit);

        // 수치 입력으로 키프레임 Time 조정
        float frameTime = KeyFrame.Time;
        ImGui::SetNextItemWidth(150.0f);
        if (ImGui::InputFloat("Keyframe Time", &frameTime, 0.01f, 0.1f, "%.3f"))
        {
            if (frameTime < 0.f) frameTime = 0.f;
            KeyFrame.Time = frameTime;
            // 수치 변경 후 즉시 정렬 및 인덱스 동기화
            m_SelectedKeyIdx = _Animation->SortKeyFrames(m_SelectedTrackIdx, m_SelectedKeyIdx);
        }
        
        const char* EasingNames[] = { "Linear (Constant Speed)", "Ease-Out (Fast to Slow)" };
        int easeIdx = static_cast<int>(KeyFrame.EasingType);
        
        ImGui::SetNextItemWidth(250.0f);
        if (ImGui::Combo("Easing Type", &easeIdx, EasingNames, IM_ARRAYSIZE(EasingNames)))
            KeyFrame.EasingType = static_cast<UIAnimEasingType>(easeIdx);

        if (KeyFrame.Transform)
        {
            Vec3 Pos = KeyFrame.Transform->GetRelativePos();
            Vec3 Scale = KeyFrame.Transform->GetRelativeScale();
            Vec3 RotDeg = KeyFrame.Transform->GetRelativeRot() * (180.f / XM_PI);

            ImGui::SetNextItemWidth(250.0f);
            if (ImGui::DragFloat3("Position", Pos, 1.f))
            {
                KeyFrame.Transform->SetRelativePos(Pos);
                _Animation->SetEditingAnimTime(_Animation->GetEditingAnimTimer()); // 수정한 Data로 수정처리
            }
            ImGui::SetNextItemWidth(250.0f);
            if (ImGui::DragFloat3("Scale", Scale, 0.1f))
            {
                KeyFrame.Transform->SetRelativeScale(Scale);
                _Animation->SetEditingAnimTime(_Animation->GetEditingAnimTimer());
            }
            ImGui::SetNextItemWidth(250.0f);
            if (ImGui::DragFloat3("Rotation", RotDeg, 0.5f))
            {
                KeyFrame.Transform->SetRelativeRot(RotDeg * (XM_PI / 180.f));
                _Animation->SetEditingAnimTime(_Animation->GetEditingAnimTimer());
            }

            Vec4 TintColor = KeyFrame.TintColor;
            if (ImGui::ColorEdit4("Tint Color", TintColor, ImGuiColorEditFlags_Float))
            {
                KeyFrame.TintColor = TintColor;
                _Animation->SetEditingAnimTime(_Animation->GetEditingAnimTimer());
            }
        }

        ImGui::Spacing();
        if (ImGui::Button("Capture KeyFrame from Target Object")) 
            KeyFrame.SetAnimDataFromGameObject(TargetObj, KeyFrame.Time);
        ImGui::SameLine();

        // 첫 번째 키프레임은 삭제 제한 처리
        const bool bCanDeleteKey = (m_SelectedKeyIdx > 0 && SelectedTrack.KeyFrames.size() > 1);
        ImGui::BeginDisabled(!bCanDeleteKey);
        if (ImGui::Button("Remove Selected KeyFrame"))
        {
            m_RemoveKeyTrackIdx = m_SelectedTrackIdx;
            m_RemoveKeyIdx = m_SelectedKeyIdx;
        }
        ImGui::EndDisabled();

        ImGui::EndDisabled();
    }
    else
    {
        ImGui::TextDisabled("Select a KeyFrame marker (Diamond) above to edit its Transform and Color.");
    }
}

void GameUIAnimationUI::ClearSelectionIfInvalid(CUIAnimation* _Animation)
{
    const vector<UIAnimTrack>& Tracks = _Animation->GetTracks();
    if (m_SelectedTrackIdx >= Tracks.size())
    {
        m_SelectedTrackIdx = -1;
        m_SelectedKeyIdx   = -1;
    }
    else if (m_SelectedTrackIdx >= 0)
    {
        const UIAnimTrack& Track = Tracks[m_SelectedTrackIdx];
        if (m_SelectedKeyIdx >= Track.KeyFrames.size())
        {
            m_SelectedKeyIdx = -1;
        }
    }
}

void GameUIAnimationUI::OnRemoveTrackConfirmUI(bool _Confirmed)
{
    if (_Confirmed)
    {
        m_PendingToRemoveTrack = true;
        return;
    }

    m_PendingToRemoveTrack = false;
    m_RemoveTrackIdx       = -1;
}

void GameUIAnimationUI::HandlePendingRemovals(CUIAnimation* _Animation)
{
    if (m_RemoveTrackIdx >= 0 && m_PendingToRemoveTrack)
    {
        _Animation->RemoveTrackByTrackIdx(m_RemoveTrackIdx);
        if (m_SelectedTrackIdx == m_RemoveTrackIdx) { m_SelectedTrackIdx = -1; m_SelectedKeyIdx = -1; }
        m_RemoveTrackIdx = -1; 
    }

    if (m_RemoveKeyTrackIdx >= 0 && m_RemoveKeyIdx >= 0)
    {
        _Animation->RemoveKeyFrame(m_RemoveKeyTrackIdx, m_RemoveKeyIdx);
        if (m_SelectedKeyIdx == m_RemoveKeyIdx) { m_SelectedKeyIdx = -1; }
        m_RemoveKeyTrackIdx = -1; 
        m_RemoveKeyIdx = -1;      
    }
}
