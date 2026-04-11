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

namespace
{
    const Vec4 GRID_COLOR(1.f, 1.f, 1.f, 0.25f);
    const Vec4 CELL_TAKEN_COLOR(1.f, 0.2f, 0.2f, 1.f);
    const Vec4 SPAWN_DEST_COLOR(1.0f, 0.0f, 1.0f, 1.f);
    const Vec4 HOVERED_CELL_COLOR(0.2f, 1.f, 1.f, 1.f);
    const Vec4 SELECTED_CELL_COLOR = DEF_COLOR_MAGENTA;
}

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

    // 레벨이 실행 중이면 편집 기능을 차단한다.
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

    // 드래그 앤 드롭 중이거나 배경 타일 편집 모드가 아니면 오버레이 창을 띄우지 않는다.
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

    if (ImGui::Begin("MainWindowDropDetector##Overlay", nullptr, flags))
    {
        Tick_UI();
    }
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

    // 씬 뷰 영역(Scene Rect) 계산
    ImVec2 _min = m_HasSceneRect ? m_SceneMin : vp->Pos;
    ImVec2 _max = m_HasSceneRect ? m_SceneMax : ImVec2(vp->Pos.x + vp->Size.x, vp->Pos.y + vp->Size.y);
    ImVec2 size = ImVec2(_max.x - _min.x, _max.y - _min.y);

    if (size.x <= 0.f || size.y <= 0.f) return;

    // 투명 버튼을 깔아 마우스 입력을 가로챈다.
    ImGui::SetCursorScreenPos(_min);
    ImGui::InvisibleButton("##MainWindowDropTarget", size);

    // 호버링 중일 때 테두리 표시
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
    {
        ImDrawList* dl = ImGui::GetForegroundDrawList();
        const ImU32 borderColor = backgroundTile ? IM_COL32(255, 180, 0, 255) : IM_COL32(255, 230, 0, 255);
        dl->AddRect(_min, _max, borderColor, 0.f, 0, 2.0f);
    }

    // 배경 타일 편집 처리
    if (backgroundTile)
    {
        TickBackgroundTileCellEditing(backgroundTile, _min, _max);
    }
    
    // 드래그 앤 드롭 처리
    if (isDragDropActive && ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Content"))
        {
            if (TreeUI::IsPayloadMultiData(payload))
            {
                // Multi-drop 처리 로직 (필요 시 구현)
            }
            else
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
                    
                    if (NewObject && NewObject->Transform())
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

void MainWindowDropDetectorUI::TickBackgroundTileCellEditing(CBackgroundTile* _BackgroundTile, const ImVec2& _Min, const ImVec2& _Max)
{
    if (!_BackgroundTile) return;

    const float tileSize = _BackgroundTile->GetTileSize();
    if (tileSize <= 0.f) return;

    // 1. 그리드 렌더링
    const float worldHalfSize = _BackgroundTile->GetWorldSizeHalf();
    const float worldMin = -worldHalfSize;
    const float worldMax = worldHalfSize;

    for (UINT i = 0; i <= CELL_ROW_COUNT; ++i)
    {
        const float linePos = worldMin + (tileSize * static_cast<float>(i));
        DrawDebugLine(Vec3(linePos, worldMin, 0.f), Vec3(linePos, worldMax, 0.f), GRID_COLOR, 0.f, false);
        DrawDebugLine(Vec3(worldMin, linePos, 0.f), Vec3(worldMax, linePos, 0.f), GRID_COLOR, 0.f, false);
    }

    // 2. 입력 처리 및 데이터 업데이트
    int hoveredX{}, hoveredY{};
    const bool hasHoveredCell = TryGetMouseCellCoord(_BackgroundTile, _Min, _Max, hoveredX, hoveredY);
    const CellCoord hoveredCell(hoveredX, hoveredY);

    // 마우스 릴리즈 시 페인팅 상태 해제
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        m_IsBackgroundTilePainting = false;
        m_HasLastPaintedCell = false;
    }

    // 브러쉬 적용 함수 (중복 로직 통합)
    auto ApplyBrush = [&](const CellCoord& _TargetCoord)
    {
        TILE_EDIT_BRUSH brush = ScriptUI::GetBackgroundTileBrush();
        if (brush == TILE_EDIT_BRUSH::NONE) return;

        bool changed = false;
        switch (brush)
        {
        case TILE_EDIT_BRUSH::CELL_TAKEN:
            _BackgroundTile->SetCellTaken(_TargetCoord, true);
            changed = true;
            break;
        case TILE_EDIT_BRUSH::CELL_EMPTY:
            _BackgroundTile->SetCellTaken(_TargetCoord, false);
            changed = true;
            break;
        case TILE_EDIT_BRUSH::SPAWN_ADD:
        {
            FIRST_SPAWN_LOC curLoc = ScriptUI::GetBackgroundTileSelectedSpawnLoc();
            _BackgroundTile->GetFirstSpawnDestinations()[curLoc].insert(_TargetCoord);
            changed = true;
            break;
        }
        case TILE_EDIT_BRUSH::SPAWN_REMOVE:
        {
            FIRST_SPAWN_LOC curLoc = ScriptUI::GetBackgroundTileSelectedSpawnLoc();
            _BackgroundTile->GetFirstSpawnDestinations()[curLoc].erase(_TargetCoord);
            changed = true;
            break;
        }
        }

        if (changed)
        {
            m_HasSelectedCell = true;
            m_SelectedCellX = _TargetCoord.x;
            m_SelectedCellY = _TargetCoord.y;
            m_HasLastPaintedCell = true;
            m_LastPaintedCellX = _TargetCoord.x;
            m_LastPaintedCellY = _TargetCoord.y;
        }
    };

    // LMB 클릭 시
    if (hasHoveredCell && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        if (ScriptUI::GetBackgroundTileBrush() != TILE_EDIT_BRUSH::NONE)
        {
            m_IsBackgroundTilePainting = true;
            ApplyBrush(hoveredCell);
        }
    }

    // LMB 드래그 중일 때
    if (hasHoveredCell && m_IsBackgroundTilePainting && ImGui::IsMouseDown(ImGuiMouseButton_Left))
    {
        // 이전 프레임에 칠한 셀과 다를 때만 적용
        if (!m_HasLastPaintedCell || m_LastPaintedCellX != hoveredX || m_LastPaintedCellY != hoveredY)
        {
            ApplyBrush(hoveredCell);
        }
    }

    // RMB 클릭 시: 현재 선택된 LOC에서 해당 셀 무조건 삭제 (편의 기능)
    if (hasHoveredCell && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        FIRST_SPAWN_LOC curLoc = ScriptUI::GetBackgroundTileSelectedSpawnLoc();
        _BackgroundTile->GetFirstSpawnDestinations()[curLoc].erase(hoveredCell);
    }

    // 3. 시각화 (데이터 렌더링)
    TILE_EDIT_BRUSH currentBrush = ScriptUI::GetBackgroundTileBrush();
    // 브러쉬 종류에 따라 SpawnDestinations를 먼저 그릴지, CellTaken을 먼저 그릴지 결정 (현재 브러쉬 레이어를 위에 둠)
    bool drawSpawnDestLast = (currentBrush == TILE_EDIT_BRUSH::SPAWN_ADD || 
                               currentBrush == TILE_EDIT_BRUSH::SPAWN_REMOVE || 
                               currentBrush == TILE_EDIT_BRUSH::NONE);

    auto RenderCellTaken = [&]() {
        for (UINT y = 0; y < CELL_ROW_COUNT; ++y)
        {
            for (UINT x = 0; x < CELL_ROW_COUNT; ++x)
            {
                CellCoord coord(static_cast<int>(x), static_cast<int>(y));
                if (_BackgroundTile->IsCellTaken(coord))
                {
                    Vec2 center = _BackgroundTile->GetCellCoordToWorldPos(coord);
                    DrawDebugRect(Vec3(center.x, center.y, 0.f), Vec3(tileSize - 1.f, tileSize - 1.f, 1.f), Vec3::Zero, CELL_TAKEN_COLOR, 0.f, false);
                }
            }
        }
    };

    auto RenderSpawnDest = [&]() {
        FIRST_SPAWN_LOC curLoc = ScriptUI::GetBackgroundTileSelectedSpawnLoc();
        auto& spawnMap = _BackgroundTile->GetFirstSpawnDestinations();
        auto it = spawnMap.find(curLoc);
        if (it != spawnMap.end())
        {
            for (const auto& coord : it->second)
            {
                Vec2 center = _BackgroundTile->GetCellCoordToWorldPos(coord);
                DrawDebugRect(Vec3(center.x, center.y, 0.f), Vec3(tileSize - 1.f, tileSize - 1.f, 1.f), Vec3::Zero, SPAWN_DEST_COLOR, 0.f, false);
            }
        }
    };

    if (drawSpawnDestLast)
    {
        RenderCellTaken();
        RenderSpawnDest();
    }
    else
    {
        RenderSpawnDest();
        RenderCellTaken();
    }

    // 4. 가이드 시각화 (호버링/선택)
    if (hasHoveredCell)
    {
        Vec2 center = _BackgroundTile->GetCellCoordToWorldPos(hoveredCell);
        DrawDebugRect(Vec3(center.x, center.y, 0.f), Vec3(tileSize - 1.f, tileSize - 1.f, 1.f), Vec3::Zero, HOVERED_CELL_COLOR, 0.f, false);
    }

    if (m_HasSelectedCell)
    {
        CellCoord selectedCell(m_SelectedCellX, m_SelectedCellY);
        if (!_BackgroundTile->IsCellCoordOutOfBounds(selectedCell))
        {
            Vec2 center = _BackgroundTile->GetCellCoordToWorldPos(selectedCell);
            DrawDebugRect(Vec3(center.x, center.y, 0.f), Vec3(tileSize - 1.f, tileSize - 1.f, 1.f), Vec3::Zero, SELECTED_CELL_COLOR, 0.f, false);
        }
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

    const float u = (mouse.x - _Min.x) / w;
    const float v = (mouse.y - _Min.y) / h;
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

bool MainWindowDropDetectorUI::TryGetMouseCellCoord(CBackgroundTile* _BackgroundTile, const ImVec2& _Min, const ImVec2& _Max, int& _OutCellX, int& _OutCellY)
{
    if (!_BackgroundTile) return false;

    const Vec3 mouseWorldPos = GetMouseWorldPosInSceneRect(_Min, _Max);
    // 마우스가 영역 밖에 있으면 (0,0,0)을 반환할 수 있으므로 좌표 변환 결과로 판단
    const CellCoord cellCoord = _BackgroundTile->GetWorldPosToCellCoord(Vec2(mouseWorldPos.x, mouseWorldPos.y));
    
    if (_BackgroundTile->IsCellCoordOutOfBounds(cellCoord))
        return false;

    _OutCellX = cellCoord.x;
    _OutCellY = cellCoord.y;
    return true;
}
