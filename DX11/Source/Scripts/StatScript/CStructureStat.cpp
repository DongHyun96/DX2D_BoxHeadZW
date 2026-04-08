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
    // Preview 오브젝트인 경우, 그냥 넘어가야함
    CStructure* StructureScript = GetOwner()->GetScriptComponent<CStructure>().Get();
    if (StructureScript->GetIsPreviewObject()) return false;
    
    if (!CStatScript::TakeDamage(_DamageAmount, _DamageSourcePos)) return false;
    
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
            GM->SpawnExplosionDome(Transform()->GetRelativePos(), GetRandom(1.2f, 1.5f));

            // 자기자신 CellCoord map 에서 제거
            CBarrel::RemoveSpawnedBarrelFromStaticMap(Barrel.Get());
            
            // 연쇄 폭파 처리
            Barrel->TryExplodeAdjacentCells();
        }
    }
    return true;
}
