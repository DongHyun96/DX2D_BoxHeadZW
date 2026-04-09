#include "pch.h"
#include "CStructureStat.h"

#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/Structure/CBarrel.h"
#include "Source/Scripts/Structure/CStructure.h"

CStructureStat::CStructureStat()
    : CStatScript(SCRIPT_TYPE::STRUCTURESTAT)
{
    m_BarrelExplosionDesc.SpawnPos                   = Vec3();
    m_BarrelExplosionDesc.ExplosionSizeFactor        = 1.f;
    m_BarrelExplosionDesc.FPS                        = 1300.f;
    m_BarrelExplosionDesc.DamageAmount               = 50.f;
    m_BarrelExplosionDesc.SpawnedBy                  = nullptr;
    m_BarrelExplosionDesc.UseCollisionForDamaging    = true;
    m_BarrelExplosionDesc.PlayExplosionSound         = true;
    m_BarrelExplosionDesc.UpwardVelocity             = Vec2::UnitY;
    m_BarrelExplosionDesc.DamagePulseDelaySec        = 0.03f;
    m_BarrelExplosionDesc.DamagePulseDurationSec     = 0.06f;
    m_BarrelExplosionDesc.DamagePulseSpriteIdx       = 1;
    m_BarrelExplosionDesc.SecondaryBurstCount        = 2;
    m_BarrelExplosionDesc.SecondaryBurstRadius       = 65.f;
    m_BarrelExplosionDesc.SecondaryBurstMinDelaySec  = 0.04f;
    m_BarrelExplosionDesc.SecondaryBurstMaxDelaySec  = 0.12f;
    m_BarrelExplosionDesc.SecondaryBurstDamageScale  = 0.f; // visual only
    m_BarrelExplosionDesc.SecondaryBurstSizeScale    = 0.5f;
    m_BarrelExplosionDesc.SecondaryBurstPlaySound    = false;
}

CStructureStat::~CStructureStat()
{
}

bool CStructureStat::TakeDamage(float _DamageAmount, GameObject* _DamageCauser)
{
    // Preview 오브젝트인 경우, 그냥 넘어가야함
    CStructure* StructureScript = GetOwner()->GetScriptComponent<CStructure>().Get();
    if (StructureScript->GetIsPreviewObject()) return false;
    
    if (!CStatScript::TakeDamage(_DamageAmount, _DamageCauser)) return false;
    
    if (IsDead())
    {
        // 자기자신 Destroy
        Destroy();
        
        // Taken Cell 상태 되돌리기
        const CellCoord cellCoord = GM->GetBackgroundCellManager()->GetWorldPosToCellCoord(Transform()->GetWorldPos2D());
        GM->GetBackgroundCellManager()->SetCellTaken(cellCoord, false);
        CStructure* StructureScript = GetOwner()->GetScriptComponent<CStructure>().Get();
        CStructure::RemoveInstalledStructure(StructureScript);

        // Barrel 종류인 경우, 연쇄 폭파 작동 처리
        if (Ptr<CBarrel> Barrel = dynamic_cast<CBarrel*>(StructureScript))
        {
            m_BarrelExplosionDesc.SpawnPos            = Transform()->GetRelativePos();
            m_BarrelExplosionDesc.ExplosionSizeFactor = GetRandom(1.2f, 1.5f);
            m_BarrelExplosionDesc.UpwardVelocity      = Vec2::UnitY * GetRandom(0.25f, 0.55f);
            GM->SpawnExplosion(m_BarrelExplosionDesc);

            // 자기자신 CellCoord map 에서 제거
            CBarrel::RemoveSpawnedBarrelFromStaticMap(Barrel.Get());
            
            // 연쇄 폭파 처리
            Barrel->TryExplodeAdjacentCells();
        }
    }
    return true;
}
