#include "pch.h"
#include "CStructureStat.h"

#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/Structure/CBarrel.h"
#include "Source/Scripts/Structure/CStructure.h"

CStructureStat::CStructureStat()
    : CStatScript(SCRIPT_TYPE::STRUCTURESTAT)
{
}

CStructureStat::~CStructureStat()
{
}

bool CStructureStat::TakeDamage(float _DamageAmount, const Vec2& _DamageSourcePos)
{
    if (!CStatScript::TakeDamage(_DamageAmount, _DamageSourcePos)) return false;
    
    if (IsDead())
    {
        Destroy();

        // Barrel 종류인 경우, 연쇄 폭파 작동 처리
        if (Ptr<CBarrel> Barrel = GetOwner()->GetScriptComponent<CBarrel>())
        {
            GM->SpawnExplosionDome(Transform()->GetRelativePos(), 40.f);

            // 자기자신 CellCoord map 에서 제거
            CBarrel::RemoveSpawnedBarrelFromStaticMap(Barrel.Get());
            
            // 연쇄 폭파 처리
            Barrel->TryExplodeAdjacentCells();
        }
    }
    return true;
}
