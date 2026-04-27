#include "pch.h"
#include "GameUIAnimationUI.h"

#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/05.GameObject/GameObject.h"
#include "GameEngine/06.Component/RenderComponent/CRenderComponent.h"
#include "GameEngine/07.EditorUI/07.TreeUI/TreeUI.h"
#include "Source/Scripts/UIScript/CText.h"
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
        if (!_GameObject) return "<Missing GameObject>";
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

void GameUIAnimationUI::SyncTrackEditCache(CUIAnimation* _Animation)
{
    if (!_Animation) return;

    const vector<UIAnimTrack>& Tracks = _Animation->GetTracks();
    const size_t PrevTrackCount = m_NewKeyFrameTimes.size();
    m_NewKeyFrameTimes.resize(Tracks.size());

    for (size_t i = PrevTrackCount; i < Tracks.size(); ++i)
    {
        if (Tracks[i].KeyFrames.empty()) m_NewKeyFrameTimes[i] = 0.f;
        else                             m_NewKeyFrameTimes[i] = Tracks[i].KeyFrames.back().Time + 0.1f;
    }
}

void GameUIAnimationUI::Tick_UI()
{
    ComponentUI::Tick_UI();

    CUIAnimation* Animation = static_cast<CUIAnimation*>(GetScript());
    if (!Animation) return;

    SyncTrackEditCache(Animation);

    const bool bCanEdit = (LevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::STOP);

    ImGui::Text("State : %s", Animation->IsPlaying() ? "Playing" : "Stopped");
    if (Animation->IsPlaying())
    {
        ImGui::SameLine();
        ImGui::Text("(Time %.2f)", Animation->GetAnimTimer());
    }

    if (ImGui::Button("Play"))
    {
        if (!Animation->Play(false))
            DebugUtil::AddDebugLog("UIAnimation : Play failed", DEF_COLOR_RED);
    }
    ImGui::SameLine();
    if (ImGui::Button("Play + StopAtEnd"))
    {
        if (!Animation->Play(true))
            DebugUtil::AddDebugLog("UIAnimation : Play failed", DEF_COLOR_RED);
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop"))
        Animation->Stop();

    ImGui::SeparatorText("Track Add");

    ImGui::BeginDisabled(!bCanEdit);

    /*if (ImGui::Button("Add Owner To Track"))
    {
        if (!Animation->AddGameObjectToAnimate(GetTargetObject().Get()))
            DebugUtil::AddDebugLog("UIAnimation : Add owner track failed", DEF_COLOR_RED);
        else
            SyncTrackEditCache(Animation);
    }*/

    ImGui::Button("Drop Outliner GameObject Here", ImVec2(-FLT_MIN, 0.f));
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("Outliner"))
        {
            if (!TreeUI::IsPayloadMultiData(Payload))
            {
                DWORD_PTR Data = *static_cast<DWORD_PTR*>(Payload->Data);
                GameObject* ReceivedObject = reinterpret_cast<GameObject*>(Data);

                if (!Animation->AddGameObjectToAnimate(ReceivedObject))
                    DebugUtil::AddDebugLog("UIAnimation : Add dropped object track failed", DEF_COLOR_RED);
                else
                    SyncTrackEditCache(Animation);
            }
        }

        ImGui::EndDragDropTarget();
    }

    ImGui::EndDisabled();

    if (!bCanEdit)
        ImGui::TextColored(ImVec4(1.f, 0.5f, 0.5f, 1.f), "Track/KeyFrame editing is available only in STOP state.");

    ImGui::SeparatorText("Tracks");

    vector<UIAnimTrack>& Tracks = Animation->GetTracks();
    if (Tracks.empty())
    {
        ImGui::Text("No tracks.");
        return;
    }

    int RemoveTrackIdx = -1;
    int RemoveKeyTrackIdx = -1;
    int RemoveKeyIdx = -1;

    for (int TrackIdx = 0; TrackIdx < static_cast<int>(Tracks.size()); ++TrackIdx)
    {
        UIAnimTrack& Track = Tracks[TrackIdx];
        GameObject* TargetObject = Track.TargetObjectReference.GetGameObject();

        ImGui::PushID(TrackIdx);

        const string Header = "Track " + to_string(TrackIdx) + " : " + GetGameObjectName(TargetObject);
        if (ImGui::TreeNodeEx(Header.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Target : %s", GetGameObjectName(TargetObject).c_str());

            ImGui::BeginDisabled(!bCanEdit);
            if (ImGui::SmallButton("Remove Track"))
                RemoveTrackIdx = TrackIdx;
            ImGui::EndDisabled();

            if (TargetObject)
            {
                ImGui::BeginDisabled(!bCanEdit);
                if (ImGui::Button("Capture Original From Target"))
                    Track.OriginalStateData.SetAnimDataFromGameObject(TargetObject, -1.f);

                ImGui::SameLine();
                if (ImGui::Button("Apply Original To Target"))
                    ApplyKeyFrameToTarget(TargetObject, Track.OriginalStateData);
                ImGui::EndDisabled();
            }

            if (TrackIdx < m_NewKeyFrameTimes.size())
            {
                float& NewKeyTime = m_NewKeyFrameTimes[TrackIdx];

                ImGui::SetNextItemWidth(150.f);
                ImGui::InputFloat("New Key Time", &NewKeyTime, 0.1f, 1.f, "%.2f");
                if (NewKeyTime < 0.f) NewKeyTime = 0.f;

                ImGui::SameLine();
                ImGui::BeginDisabled(!bCanEdit);
                if (ImGui::Button("Add KeyFrame"))
                {
                    if (!Animation->AddNewKeyFrame(TrackIdx, NewKeyTime))
                        DebugUtil::AddDebugLog("UIAnimation : Add keyframe failed", DEF_COLOR_RED);
                    else
                        NewKeyTime += 0.1f;
                }
                ImGui::EndDisabled();
            }

            ImGui::SeparatorText("KeyFrames");

            if (Track.KeyFrames.empty())
            {
                ImGui::Text("No keyframes.");
            }
            else
            {
                for (int KeyIdx = 0; KeyIdx < static_cast<int>(Track.KeyFrames.size()); ++KeyIdx)
                {
                    UIAnimKeyFrameData& KeyFrame = Track.KeyFrames[KeyIdx];

                    ImGui::PushID(KeyIdx);

                    char KeyHeader[128]{};
                    sprintf_s(KeyHeader, "KeyFrame %d (t = %.2f)", KeyIdx, KeyFrame.Time);

                    if (ImGui::TreeNodeEx(KeyHeader, ImGuiTreeNodeFlags_None))
                    {
                        if (KeyIdx == 0)
                            ImGui::TextColored(ImVec4(0.7f, 0.8f, 1.f, 1.f), "First keyframe is base key.");

                        bool bUseLerp = KeyFrame.bUseLerp;
                        ImGui::BeginDisabled(!bCanEdit);
                        if (ImGui::Checkbox("Use Lerp", &bUseLerp))
                            KeyFrame.bUseLerp = bUseLerp;
                        ImGui::EndDisabled();

                        if (!KeyFrame.Transform && TargetObject)
                            KeyFrame.SetAnimDataFromGameObject(TargetObject, KeyFrame.Time);

                        if (KeyFrame.Transform)
                        {
                            Vec3 Pos = KeyFrame.Transform->GetRelativePos();
                            Vec3 Scale = KeyFrame.Transform->GetRelativeScale();
                            Vec3 RotDeg = KeyFrame.Transform->GetRelativeRot() * (180.f / XM_PI);

                            ImGui::BeginDisabled(!bCanEdit);
                            if (ImGui::DragFloat3("Position", Pos, 1.f))
                                KeyFrame.Transform->SetRelativePos(Pos);

                            if (ImGui::DragFloat3("Scale", Scale, 1.f))
                                KeyFrame.Transform->SetRelativeScale(Scale);

                            if (ImGui::DragFloat3("Rotation (Deg)", RotDeg, 0.5f))
                                KeyFrame.Transform->SetRelativeRot(RotDeg * (XM_PI / 180.f));

                            Vec4 TintColor = KeyFrame.TintColor;
                            if (ImGui::ColorEdit4("Tint Color", TintColor, ImGuiColorEditFlags_Float))
                                KeyFrame.TintColor = TintColor;
                            ImGui::EndDisabled();
                        }

                        if (TargetObject)
                        {
                            ImGui::BeginDisabled(!bCanEdit);
                            if (ImGui::Button("Capture KeyFrame From Target"))
                            {
                                if (!KeyFrame.SetAnimDataFromGameObject(TargetObject, KeyFrame.Time))
                                    DebugUtil::AddDebugLog("UIAnimation : Capture keyframe failed", DEF_COLOR_RED);
                            }

                            ImGui::SameLine();
                            if (ImGui::Button("Apply KeyFrame To Target"))
                            {
                                if (!ApplyKeyFrameToTarget(TargetObject, KeyFrame))
                                    DebugUtil::AddDebugLog("UIAnimation : Apply keyframe failed", DEF_COLOR_RED);
                            }
                            ImGui::EndDisabled();
                        }

                        const bool bCanRemoveKey = bCanEdit && KeyIdx > 0 && Track.KeyFrames.size() > 1;
                        ImGui::BeginDisabled(!bCanRemoveKey);
                        if (ImGui::Button("Remove KeyFrame"))
                        {
                            RemoveKeyTrackIdx = TrackIdx;
                            RemoveKeyIdx = KeyIdx;
                        }
                        ImGui::EndDisabled();

                        ImGui::TreePop();
                    }

                    ImGui::PopID();
                }
            }

            ImGui::TreePop();
        }

        ImGui::PopID();
    }

    if (RemoveTrackIdx >= 0)
    {
        if (!Animation->RemoveTrack(RemoveTrackIdx))
            DebugUtil::AddDebugLog("UIAnimation : Remove track failed", DEF_COLOR_RED);

        SyncTrackEditCache(Animation);
        return;
    }

    if (RemoveKeyTrackIdx >= 0 && RemoveKeyIdx >= 0)
    {
        if (!Animation->RemoveKeyFrame(RemoveKeyTrackIdx, RemoveKeyIdx))
            DebugUtil::AddDebugLog("UIAnimation : Remove keyframe failed", DEF_COLOR_RED);
    }
}
