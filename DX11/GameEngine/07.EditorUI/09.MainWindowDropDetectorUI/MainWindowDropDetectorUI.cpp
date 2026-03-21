#include "pch.h"
#include "MainWindowDropDetectorUI.h"

#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/04.Asset/09.Prefab/APrefab.h"
#include "GameEngine/07.EditorUI/07.TreeUI/TreeUI.h"
#include "ImGui/imgui_internal.h"

MainWindowDropDetectorUI::MainWindowDropDetectorUI()
    : EditorUI("MainWindowDropDetector")
{
}

MainWindowDropDetectorUI::~MainWindowDropDetectorUI()
{
}

void MainWindowDropDetectorUI::Tick()
{
    if (!IsActive()) return;
    if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::STOP) return;
    if (!ImGui::IsDragDropActive()) return;

    ImGuiViewport* vp = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(vp->Pos);
    ImGui::SetNextWindowSize(vp->Size);
    ImGui::SetNextWindowViewport(vp->ID);
    ImGui::SetNextWindowBgAlpha(0.f);

    const ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("MainWindowDropDetector##Overlay", nullptr, flags);
    Tick_UI();
    ImGui::End();
}

void MainWindowDropDetectorUI::Tick_UI()
{
    ImGuiViewport* vp = ImGui::GetMainViewport();

    ImVec2 _min = m_HasSceneRect ? m_SceneMin : vp->Pos;
    ImVec2 _max = m_HasSceneRect ? m_SceneMax : ImVec2(vp->Pos.x + vp->Size.x, vp->Pos.y + vp->Size.y);
    ImVec2 size = ImVec2(_max.x - _min.x, _max.y - _min.y);

    if (size.x <= 0.f || size.y <= 0.f) return;

    ImGui::SetCursorScreenPos(_min);
    ImGui::InvisibleButton("##MainWindowDropTarget", size);

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
    {
        ImDrawList* dl = ImGui::GetForegroundDrawList();
        dl->AddRect(_min, _max, IM_COL32(255, 230, 0, 255), 0.f, 0, 2.0f);       
    }
    
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Content"))
        {
            // Level의 경우 Level 전환 (단일로 받을 시에만)
            // Prefab의 경우 새로운 Prefab 게임오브젝트 추가 (마우스 포인터 위치에)

            if (TreeUI::IsPayloadMultiData(payload)) // Multi Drop 시
            {
                const DWORD_PTR* data   = static_cast<const DWORD_PTR*>(payload->Data);
                const int count         = payload->DataSize / sizeof(DWORD_PTR);
            }
            else // Single Item Drop 시
            {
                DWORD_PTR data = *static_cast<DWORD_PTR*>(payload->Data);
                Ptr<Asset> pAsset = reinterpret_cast<Asset*>(data);
                
                if (pAsset->GetType() == ASSET_TYPE::LEVEL)
                {
                    ChangeLevel(pAsset->GetKey());
                    return;
                }
                
                if (pAsset->GetType() == ASSET_TYPE::PREFAB)
                {
                    Ptr<APrefab> Prefab = static_cast<APrefab*>(pAsset.Get());
                    GameObject* NewObject = Prefab->InstantiateAndSpawnToCurLevel();
                    
                    if (NewObject->Transform())
                    {
                        Vec3 SpawnPos = GetMouseWorldPosInSceneRect(_min, _max); 
                        NewObject->Transform()->SetRelativePos(SpawnPos);
                    }
                }
            }
        }

        ImGui::EndDragDropTarget();
    }
}

Vec3 MainWindowDropDetectorUI::GetMouseWorldPosInSceneRect(const ImVec2& _Min, const ImVec2& _Max)
{
    const Ptr<CCamera> pPOVCam = RenderMgr::GetInst()->GetPOVCam();
    if (!pPOVCam) return Vec3::Zero;

    ImVec2 mouse = ImGui::GetIO().MousePos;

    if (mouse.x < _Min.x || mouse.x > _Max.x || mouse.y < _Min.y || mouse.y > _Max.y)
        return Vec3::Zero;

    const float w = _Max.x - _Min.x;
    const float h = _Max.y - _Min.y;
    if (w <= 0.f || h <= 0.f) return Vec3::Zero;

    const float u = (mouse.x - _Min.x) / w;         // 0..1
    const float v = (mouse.y - _Min.y) / h;         // 0..1
    const float ndcX = u * 2.f - 1.f;
    const float ndcY = 1.f - v * 2.f;

    const float worldW = pPOVCam->GetWidth() * pPOVCam->GetOrthoScale();
    const float worldH = (pPOVCam->GetWidth() / pPOVCam->GetAspectRatio()) * pPOVCam->GetOrthoScale();

    Vec3 vWorld = pPOVCam->Transform()->GetWorldPos();
    vWorld += pPOVCam->Transform()->GetDir(DIR::RIGHT) * (ndcX * worldW * 0.5f);
    vWorld += pPOVCam->Transform()->GetDir(DIR::UP) * (ndcY * worldH * 0.5f);
    vWorld.z = 0.f;

    return vWorld;
}

Vec3 MainWindowDropDetectorUI::GetMouseWorldPosInSceneRect()
{
    ImGuiViewport* vp = ImGui::GetMainViewport();
    
    ImVec2 _min = m_HasSceneRect ? m_SceneMin : vp->Pos;
    ImVec2 _max = m_HasSceneRect ? m_SceneMax : ImVec2(vp->Pos.x + vp->Size.x, vp->Pos.y + vp->Size.y);
    
    return GetMouseWorldPosInSceneRect(_min, _max);
}
