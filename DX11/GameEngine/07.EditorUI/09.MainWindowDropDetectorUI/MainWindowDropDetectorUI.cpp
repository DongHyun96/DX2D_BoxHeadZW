#include "pch.h"
#include "MainWindowDropDetectorUI.h"

#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/04.Asset/09.Prefab/APrefab.h"
#include "GameEngine/07.EditorUI/04.ComponentUI/06.ScriptUI/ScriptUI.h"
#include "GameEngine/07.EditorUI/07.TreeUI/TreeUI.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/BackgroundTile/CBackgroundTile.h"
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
    if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::STOP)
    {
        m_IsBackgroundTilePainting = false;
        m_HasLastPaintedCell = false;
        return;
    }

    const bool isDragDropActive = ImGui::IsDragDropActive();
    const bool isBackgroundTileEditEnabled = ScriptUI::IsBackgroundTileCellEditingEnabled();
    CBackgroundTile* backgroundTile = nullptr;
    if (isBackgroundTileEditEnabled)
    {
        backgroundTile = ScriptUI::GetBackgroundTileEditingTarget();
        if (!backgroundTile) backgroundTile = GM->GetBackgroundCellManager();
    }

    if (!isDragDropActive && !backgroundTile)
    {
        m_IsBackgroundTilePainting = false;
        m_HasLastPaintedCell = false;
        return;
    }

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
    const bool isDragDropActive = ImGui::IsDragDropActive();
    const bool isBackgroundTileEditEnabled = ScriptUI::IsBackgroundTileCellEditingEnabled();
    CBackgroundTile* backgroundTile = nullptr;
    if (isBackgroundTileEditEnabled)
    {
        backgroundTile = ScriptUI::GetBackgroundTileEditingTarget();
        if (!backgroundTile) backgroundTile = GM->GetBackgroundCellManager();
    }

    ImVec2 _min = m_HasSceneRect ? m_SceneMin : vp->Pos;
    ImVec2 _max = m_HasSceneRect ? m_SceneMax : ImVec2(vp->Pos.x + vp->Size.x, vp->Pos.y + vp->Size.y);
    ImVec2 size = ImVec2(_max.x - _min.x, _max.y - _min.y);

    if (size.x <= 0.f || size.y <= 0.f) return;

    ImGui::SetCursorScreenPos(_min);
    ImGui::InvisibleButton("##MainWindowDropTarget", size);

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
    {
        ImDrawList* dl = ImGui::GetForegroundDrawList();
        const ImU32 borderColor = backgroundTile ? IM_COL32(255, 180, 0, 255) : IM_COL32(255, 230, 0, 255);
        dl->AddRect(_min, _max, borderColor, 0.f, 0, 2.0f);
    }

    if (backgroundTile)
        TickBackgroundTileCellEditing(backgroundTile, _min, _max);
    
    if (isDragDropActive && ImGui::BeginDragDropTarget())
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
                    ChangeLevel(pAsset->GetKey(), true);
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

bool MainWindowDropDetectorUI::TryGetMouseCellCoord(CBackgroundTile* _BackgroundTile, const ImVec2& _Min, const ImVec2& _Max, int& _OutCellX, int& _OutCellY)
{
    if (!_BackgroundTile) return false;

    const ImVec2 mousePos = ImGui::GetIO().MousePos;
    if (mousePos.x < _Min.x || mousePos.x > _Max.x || mousePos.y < _Min.y || mousePos.y > _Max.y)
        return false;

    const Vec3 mouseWorldPos = GetMouseWorldPosInSceneRect(_Min, _Max);
    const CellCoord cellCoord = _BackgroundTile->GetWorldPosToCellCoord(Vec2(mouseWorldPos.x, mouseWorldPos.y));
    if (_BackgroundTile->IsCellCoordOutOfBounds(cellCoord))
        return false;

    _OutCellX = cellCoord.x;
    _OutCellY = cellCoord.y;
    return true;
}

void MainWindowDropDetectorUI::TickBackgroundTileCellEditing(CBackgroundTile* _BackgroundTile, const ImVec2& _Min, const ImVec2& _Max)
{
    if (!_BackgroundTile) return;

    const float tileSize = _BackgroundTile->GetTileSize();
    if (tileSize <= 0.f) return;

    const float worldHalfSize = _BackgroundTile->GetWorldSizeHalf();
    const float worldMin = -worldHalfSize;
    const float worldMax = worldHalfSize;

    const Vec4 gridColor(1.f, 1.f, 1.f, 0.25f);
    for (UINT i = 0; i <= CELL_ROW_COUNT; ++i)
    {
        const float linePos = worldMin + (tileSize * static_cast<float>(i));
        DrawDebugLine(Vec3(linePos, worldMin, 0.f), Vec3(linePos, worldMax, 0.f), gridColor, 0.f, false);
        DrawDebugLine(Vec3(worldMin, linePos, 0.f), Vec3(worldMax, linePos, 0.f), gridColor, 0.f, false);
    }

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        m_IsBackgroundTilePainting = false;
        m_HasLastPaintedCell = false;
    }

    int hoveredCellX{};
    int hoveredCellY{};
    const bool hasHoveredCell = TryGetMouseCellCoord(_BackgroundTile, _Min, _Max, hoveredCellX, hoveredCellY);

    if (hasHoveredCell && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        const CellCoord hoveredCell(hoveredCellX, hoveredCellY);

        m_IsBackgroundTilePainting = true;
        m_BackgroundTilePaintValue = ScriptUI::GetBackgroundTileBrushTakenValue();
        _BackgroundTile->SetCellTaken(hoveredCell, m_BackgroundTilePaintValue);

        m_HasSelectedCell = true;
        m_SelectedCellX = hoveredCellX;
        m_SelectedCellY = hoveredCellY;
        m_HasLastPaintedCell = true;
        m_LastPaintedCellX = hoveredCellX;
        m_LastPaintedCellY = hoveredCellY;
    }

    if (m_IsBackgroundTilePainting && ImGui::IsMouseDown(ImGuiMouseButton_Left) && hasHoveredCell)
    {
        if (!m_HasLastPaintedCell || m_LastPaintedCellX != hoveredCellX || m_LastPaintedCellY != hoveredCellY)
        {
            const CellCoord hoveredCell(hoveredCellX, hoveredCellY);
            _BackgroundTile->SetCellTaken(hoveredCell, m_BackgroundTilePaintValue);

            m_HasLastPaintedCell = true;
            m_LastPaintedCellX = hoveredCellX;
            m_LastPaintedCellY = hoveredCellY;
        }

        m_HasSelectedCell = true;
        m_SelectedCellX = hoveredCellX;
        m_SelectedCellY = hoveredCellY;
    }

    for (UINT y = 0; y < CELL_ROW_COUNT; ++y)
    {
        for (UINT x = 0; x < CELL_ROW_COUNT; ++x)
        {
            const CellCoord cellCoord(static_cast<int>(x), static_cast<int>(y));
            if (!_BackgroundTile->IsCellTaken(cellCoord)) continue;

            const Vec2 cellCenterPos = _BackgroundTile->GetCellCoordToWorldPos(cellCoord);
            DrawDebugRect
            (
                Vec3(cellCenterPos.x, cellCenterPos.y, 0.f),
                Vec3(tileSize - 1.f, tileSize - 1.f, 1.f),
                Vec3(0.f, 0.f, 0.f),
                Vec4(1.f, 0.2f, 0.2f, 1.f),
                0.f,
                false
            );
        }
    }

    if (hasHoveredCell)
    {
        const Vec2 hoveredCellCenterPos = _BackgroundTile->GetCellCoordToWorldPos(CellCoord(hoveredCellX, hoveredCellY));
        DrawDebugRect
        (
            Vec3(hoveredCellCenterPos.x, hoveredCellCenterPos.y, 0.f),
            Vec3(tileSize - 1.f, tileSize - 1.f, 1.f),
            Vec3(0.f, 0.f, 0.f),
            Vec4(0.2f, 1.f, 1.f, 1.f),
            0.f,
            false
        );
    }

    if (m_HasSelectedCell)
    {
        const CellCoord selectedCell(m_SelectedCellX, m_SelectedCellY);
        if (!_BackgroundTile->IsCellCoordOutOfBounds(selectedCell))
        {
            const Vec2 selectedCenterPos = _BackgroundTile->GetCellCoordToWorldPos(selectedCell);
            DrawDebugRect
            (
                Vec3(selectedCenterPos.x, selectedCenterPos.y, 0.f),
                Vec3(tileSize - 1.f, tileSize - 1.f, 1.f),
                Vec3(0.f, 0.f, 0.f),
                Vec4(1.f, 0.9f, 0.1f, 1.f),
                0.f,
                false
            );
        }
    }
}

Vec3 MainWindowDropDetectorUI::GetMouseWorldPosInSceneRect()
{
    ImGuiViewport* vp = ImGui::GetMainViewport();
    
    ImVec2 _min = m_HasSceneRect ? m_SceneMin : vp->Pos;
    ImVec2 _max = m_HasSceneRect ? m_SceneMax : ImVec2(vp->Pos.x + vp->Size.x, vp->Pos.y + vp->Size.y);
    
    return GetMouseWorldPosInSceneRect(_min, _max);
}
