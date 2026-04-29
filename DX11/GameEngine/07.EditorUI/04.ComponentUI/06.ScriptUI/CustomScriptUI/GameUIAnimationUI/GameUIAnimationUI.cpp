#include "pch.h"
#include "GameUIAnimationUI.h"

#include <algorithm>

#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/05.GameObject/GameObject.h"
#include "GameEngine/06.Component/RenderComponent/CRenderComponent.h"
#include "GameEngine/07.EditorUI/07.TreeUI/TreeUI.h"
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

    bool ApplyKeyFrameToTarget(GameObject* _TargetObject, const UIAnimKeyFrameData& _KeyFrameData)
    {
        if (!_TargetObject || !_TargetObject->Transform() || !_KeyFrameData.Transform) return false;

        _TargetObject->Transform()->CopyRelativePosScaleRot(_KeyFrameData.Transform);

        if (const Ptr<CText>& Text = _TargetObject->GetScriptComponent<CText>())
            Text->SetColor(_KeyFrameData.TintColor);
        else if (const Ptr<CRenderComponent>& RenderCom = _TargetObject->GetRenderCom())
            RenderCom->GetMaterial()->SetScalar(VEC4_0, _KeyFrameData.TintColor);

        return true;
    }
}

GameUIAnimationUI::GameUIAnimationUI()
    : CustomScriptUI("GameUIAnimation(Script)")
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
    if (!Animation) return;

    // 현재 레벨이 정지(STOP) 상태인지 체크
    const bool bCanEdit = (LevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::STOP);

    // 데이터가 변했을 때를 대비한 Selection 안전장치
    ClearSelectionIfInvalid(Animation);

    // 1. 상단 재생/컨트롤 툴바
    RenderToolbar(Animation, bCanEdit);
    ImGui::Separator();

    // 2. 메인 에디터 (좌: 트랙 리스트 / 우: 도프시트 타임라인)
    RenderMainEditor(Animation, bCanEdit);
    ImGui::Separator();

    // 3. 하단 인스펙터 (선택된 트랙/키프레임 편집기)
    RenderBottomInspector(Animation, bCanEdit);

    // 삭제 예약 처리
    HandlePendingRemovals(Animation);
}

void GameUIAnimationUI::RenderToolbar(CUIAnimation* _Animation, bool _bCanEdit)
{
    // 재생 컨트롤
    if (ImGui::Button("Play")) { _Animation->Play(false); }
    ImGui::SameLine();
    if (ImGui::Button("Play(AutoStop)")) { _Animation->Play(true); }
    ImGui::SameLine();
    if (ImGui::Button("Stop")) { _Animation->Stop(); }

    ImGui::SameLine(0, 20.0f);
    
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

void GameUIAnimationUI::RenderMainEditor(CUIAnimation* _Animation, bool _bCanEdit)
{
    vector<UIAnimTrack>& Tracks = _Animation->GetTracks();

    if (Tracks.empty())
    {
        ImGui::Text("No tracks.");
        return;
    }

    float maxAnimTime = 2.0f;
    for (size_t i = 0; i < Tracks.size(); ++i)
    {
        if (!Tracks[i].KeyFrames.empty())
        {
            float lastTime = Tracks[i].KeyFrames.back().Time;
            if (lastTime > maxAnimTime) maxAnimTime = lastTime;
        }
    }
    maxAnimTime += 1.f;

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
            ImGui::Text("%s", GetGameObjectName(TargetObj).c_str());
            ImGui::PopID();

            // --- [2열] 도프시트 타임라인 ---
            ImGui::TableSetColumnIndex(1);
            
            ImVec2 cellPos = ImGui::GetCursorScreenPos();
            float cellHeight = ImGui::GetTextLineHeightWithSpacing();

            s_ActualTimelineStartX = cellPos.x;

            for (float t = 0.0f; t <= maxAnimTime; t += 0.5f)
            {
                float x = cellPos.x + (t * m_TimelineScale);
                drawList->AddLine(ImVec2(x, cellPos.y), ImVec2(x, cellPos.y + cellHeight), IM_COL32(100, 100, 100, 50));
            }

            const int keyCount = static_cast<int>(Track.KeyFrames.size());
            for (int KeyIdx = 0; KeyIdx < keyCount; ++KeyIdx)
            {
                float time = Track.KeyFrames[KeyIdx].Time;
                float keyX = cellPos.x + (time * m_TimelineScale);
                float keyY = cellPos.y + (cellHeight * 0.5f);

                ImGui::SetCursorScreenPos(ImVec2(keyX - 6.0f, cellPos.y));
                ImGui::PushID(KeyIdx);
                ImGui::InvisibleButton("##Key", ImVec2(12.0f, cellHeight));

                if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
                {
                    m_SelectedTrackIdx = TrackIdx;
                    m_SelectedKeyIdx = KeyIdx;

                    float mouseXInTimeline = ImGui::GetIO().MousePos.x - cellPos.x;
                    float newTime = mouseXInTimeline / m_TimelineScale;
                    newTime = max(newTime, 0.f);

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

                ImVec2 p1(keyX, keyY - 5.0f);
                ImVec2 p2(keyX + 5.0f, keyY);
                ImVec2 p3(keyX, keyY + 5.0f);
                ImVec2 p4(keyX - 5.0f, keyY);
                
                drawList->AddQuadFilled(p1, p2, p3, p4, keyColor);
                drawList->AddQuad(p1, p2, p3, p4, IM_COL32(0, 0, 0, 255), 1.0f);
            }
        }

        // ----------------------------------------------------
        // 타임라인 스크러빙 드래그 처리
        // ----------------------------------------------------
        
        static bool s_bIsScrubbing = false;
        
        // 타임라인 영역의 좌상단/우하단 좌표 계산
        ImVec2 timelineRectMin = ImVec2(s_ActualTimelineStartX, timelineAreaPos.y);
        ImVec2 timelineRectMax = ImVec2(timelineRectMin.x + ImGui::GetColumnWidth(1), timelineAreaPos.y + 250.0f);

        // 마우스가 타임라인 영역 내부에 있고, 왼쪽 버튼을 막 클릭했다면
        if (ImGui::IsMouseHoveringRect(timelineRectMin, timelineRectMax) && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            // 키프레임 다이아몬드 같은 다른 UI를 클릭한게 아닐 때만 (충돌 방지)
            if (!ImGui::IsAnyItemActive()) 
                s_bIsScrubbing = true;
        }

        if (s_bIsScrubbing)
        {
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                // 드래그 중: 현재 마우스 위치에 맞춰 에디팅 타임 갱신
                float mouseX         = ImGui::GetIO().MousePos.x - s_ActualTimelineStartX;
                float newEditingTime = max(mouseX / m_TimelineScale, 0.f);
                
                _Animation->SetEditingAnimTime(newEditingTime);
            }
            else
            {
                // 마우스 떼면 드래그 종료
                s_bIsScrubbing = false;
            }
        }

        // ----------------------------------------------------
        // 현재 Time 재생 인디케이터 (초록색 선)
        // ----------------------------------------------------
        float currentDisplayTime = _Animation->IsPlaying() ? _Animation->GetAnimTimer() : _Animation->GetEditingAnimTimer();
        float scrubberX = s_ActualTimelineStartX + (currentDisplayTime * m_TimelineScale);
        ImVec2 tableEndPos = ImGui::GetCursorScreenPos();
        
        drawList->AddLine(
            ImVec2(scrubberX, timelineAreaPos.y), 
            ImVec2(scrubberX, tableEndPos.y), 
            IM_COL32(0, 255, 0, 255), 2.0f
        );

        ImVec2 triP1(scrubberX, timelineAreaPos.y);
        ImVec2 triP2(scrubberX - 6.0f, timelineAreaPos.y - 12.0f);
        ImVec2 triP3(scrubberX + 6.0f, timelineAreaPos.y - 12.0f);
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
    if (ImGui::Button("Apply Origin (to Target)")) 
        ApplyKeyFrameToTarget(TargetObj, SelectedTrack.OriginalStateData);
    
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
        m_RemoveTrackIdx = m_SelectedTrackIdx;

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
        
        bool bUseLerp = KeyFrame.bUseLerp;
        if (ImGui::Checkbox("Use Lerp (Interpolate from previous)", &bUseLerp)) 
            KeyFrame.bUseLerp = bUseLerp;

        if (KeyFrame.Transform)
        {
            Vec3 Pos = KeyFrame.Transform->GetRelativePos();
            Vec3 Scale = KeyFrame.Transform->GetRelativeScale();
            Vec3 RotDeg = KeyFrame.Transform->GetRelativeRot() * (180.f / XM_PI);

            ImGui::SetNextItemWidth(250.0f);
            if (ImGui::DragFloat3("Position", Pos, 1.f)) KeyFrame.Transform->SetRelativePos(Pos);
            ImGui::SetNextItemWidth(250.0f);
            if (ImGui::DragFloat3("Scale", Scale, 0.1f)) KeyFrame.Transform->SetRelativeScale(Scale);
            ImGui::SetNextItemWidth(250.0f);
            if (ImGui::DragFloat3("Rotation", RotDeg, 0.5f)) KeyFrame.Transform->SetRelativeRot(RotDeg * (XM_PI / 180.f));

            Vec4 TintColor = KeyFrame.TintColor;
            if (ImGui::ColorEdit4("Tint Color", TintColor, ImGuiColorEditFlags_Float)) KeyFrame.TintColor = TintColor;
        }

        ImGui::Spacing();
        if (ImGui::Button("Capture KeyFrame from Target Object")) 
            KeyFrame.SetAnimDataFromGameObject(TargetObj, KeyFrame.Time);
        ImGui::SameLine();
        if (ImGui::Button("Apply KeyFrame to Target Object")) 
            ApplyKeyFrameToTarget(TargetObj, KeyFrame);

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

void GameUIAnimationUI::HandlePendingRemovals(CUIAnimation* _Animation)
{
    if (m_RemoveTrackIdx >= 0)
    {
        _Animation->RemoveTrack(m_RemoveTrackIdx);
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