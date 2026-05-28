#include "pch.h"
#include "CBarrel.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/BackgroundTile/CBackgroundTile.h"

map<CBarrel*, CellCoord> CBarrel::m_mapSpawnedBarrel{};


const float CBarrel::m_LateExplodeWaitTime = 0.1f;


CBarrel::CBarrel()
    : CStructure(SCRIPT_TYPE::BARREL)
{
    m_ExplosionDesc.SpawnPos                   = Vec3();
    m_ExplosionDesc.ExplosionSizeFactor        = 1.f;
    m_ExplosionDesc.FPS                        = 50.f;
    m_ExplosionDesc.DamageAmount               = 50.f;
    m_ExplosionDesc.SpawnedBy                  = nullptr;
    m_ExplosionDesc.UseCollisionForDamaging    = true;
    m_ExplosionDesc.PlayExplosionSound         = true;
    m_ExplosionDesc.UpwardVelocity             = Vec2::UnitY;
    m_ExplosionDesc.DamagePulseDelaySec        = 0.03f;
    m_ExplosionDesc.DamagePulseDurationSec     = 0.06f;
    m_ExplosionDesc.DamagePulseSpriteIdx       = 1;
    m_ExplosionDesc.SecondaryBurstCount        = 2;
    m_ExplosionDesc.SecondaryBurstRadius       = 65.f;
    m_ExplosionDesc.SecondaryBurstMinDelaySec  = 0.04f;
    m_ExplosionDesc.SecondaryBurstMaxDelaySec  = 0.12f;
    m_ExplosionDesc.SecondaryBurstDamageScale  = 0.f; // visual only
    m_ExplosionDesc.SecondaryBurstSizeScale    = 0.5f;
    m_ExplosionDesc.SecondaryBurstPlaySound    = false;
    
    m_LateExplosionSpawnDesc = m_ExplosionDesc;
}

CBarrel::~CBarrel()
{
}

void CBarrel::Begin()
{
    CStructure::Begin();
    
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
    
    // 여기서 자기자신의 Taken 기록을 지워야 한다
    GM->GetBackgroundCellManager()->SetCellTaken(Transform()->GetWorldPos2D(), false);
    CStructure* StructureScript = GetOwner()->GetScriptComponent<CStructure>().Get();
    CStructure::RemoveInstalledStructure(StructureScript);
    
    m_LateExplosionSpawnDesc.SpawnPos            = Transform()->GetRelativePos();
    m_LateExplosionSpawnDesc.ExplosionSizeFactor = GetRandom(1.2f, 1.5f);
    m_LateExplosionSpawnDesc.UpwardVelocity      = Vec2::UnitY * GetRandom(0.25f, 0.55f);
    
    if (GM->SpawnExplosion(m_LateExplosionSpawnDesc))
        GM->GetBackgroundCellManager()->SpawnScorchDecal(Transform()->GetWorldPos2D(), Vec2::One * GetRandom(3.f, 4.f));
    
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

    for (auto iter = CheckedExplosion.begin(); iter != CheckedExplosion.end(); ++iter)
        RemoveSpawnedBarrelFromStaticMap(*iter);
}

bool CBarrel::DestroyStructure(bool _DestroyedByDamaged)
{
    if (!CStructure::DestroyStructure(_DestroyedByDamaged)) return false;

    // 자기자신 CellCoord map 에서 제거
    CBarrel::RemoveSpawnedBarrelFromStaticMap(this);

    // Damage를 입어서 터진 것이라면, ExplosionEffect 스폰 및 연쇄 폭파 적용
    if (_DestroyedByDamaged)
    {
        m_ExplosionDesc.SpawnPos            = Transform()->GetRelativePos();
        m_ExplosionDesc.ExplosionSizeFactor = GetRandom(1.2f, 1.5f);
        m_ExplosionDesc.UpwardVelocity      = Vec2::UnitY * GetRandom(0.25f, 0.55f);
        GM->SpawnExplosion(m_ExplosionDesc);
            
        // 연쇄 폭파 처리
        TryExplodeAdjacentCells();
    }
    
    return true;
}

void CBarrel::ExecuteLateExplosion()
{
    m_MarkedLateExplosion = true;    
    m_LateExplodeTimer = 0.f;
}
