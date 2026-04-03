#include "pch.h"
#include "CBarrel.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/BackgroundTile/CBackgroundTile.h"

map<CBarrel*, CellCoord> CBarrel::m_mapSpawnedBarrel{};

const float CBarrel::m_LateExplodeWaitTime = 0.2f;

CBarrel::CBarrel()
    : CStructure(SCRIPT_TYPE::BARREL)
{
}

CBarrel::~CBarrel()
{
}

void CBarrel::Begin()
{
    // CreateObject 처리가 되면서 Begin이 호출되면서 자연스럽게 SpawnedBarrel에 집어넣을 수 있음
    // TODO : 이미 맵에 깔려있는 애들은 BackgroundCellManager가 Begin시점에 nullptr일수도 있음
    if (CBackgroundTile* BackgroundCellManager = GM->GetBackgroundCellManager())
    {
        if (GetIsPreviewObject()) return;
        m_CellCoord = BackgroundCellManager->GetWorldPosToCellCoord(ToVec2(Transform()->GetRelativePos()));
        m_mapSpawnedBarrel[this] = m_CellCoord; 
    }
}

void CBarrel::Tick()
{
    CStructure::Tick();
    UpdateLateExplosion();
}

void CBarrel::UpdateLateExplosion()
{
    if (!m_MarkedLateExplosion) return;
    
    m_LateExplodeTimer += DT;
    if (m_LateExplodeTimer < m_LateExplodeWaitTime) return;
    
    Destroy(); // Self Destroy 처리
    GM->SpawnExplosionDome(Transform()->GetRelativePos(), 40.f);
    TryExplodeAdjacentCells(); // 연쇄 폭파 처리 continue
}

void CBarrel::RemoveSpawnedBarrelFromStaticMap(CBarrel* _TargetToRemove)
{
    if (!m_mapSpawnedBarrel.contains(_TargetToRemove)) return;
    m_mapSpawnedBarrel.erase(_TargetToRemove);
}

void CBarrel::TryExplodeAdjacentCells()
{
    /*// Adjacent한 Cell에서 자기자신 제거
    RemoveSpawnedBarrelFromStaticMap(this);*/

    set<CBarrel*> CheckedExplosion{};
    for (const pair<CBarrel* const, CellCoord>& Pair : m_mapSpawnedBarrel)
    {
        // 자기 자신이라면 continue (들어올 일 없긴 함)
        if (Pair.first == this) continue;
        
        const CellCoord& CurrentCellCoord = Pair.second;

        // Adjacent한 Cell
        if (m_CellCoord.x - 1 <= CurrentCellCoord.x && CurrentCellCoord.x <= m_CellCoord.x + 1 &&
            m_CellCoord.y -1 <= CurrentCellCoord.y && CurrentCellCoord.y <= m_CellCoord.y + 1)
        {
            Pair.first->ExecuteLateExplosion();
            CheckedExplosion.insert(Pair.first);
        }
    }

    for (CBarrel* WillExplode : CheckedExplosion)
        RemoveSpawnedBarrelFromStaticMap(WillExplode);
}

void CBarrel::ExecuteLateExplosion()
{
    m_MarkedLateExplosion = true;    
    m_LateExplodeTimer = 0.f;
}
