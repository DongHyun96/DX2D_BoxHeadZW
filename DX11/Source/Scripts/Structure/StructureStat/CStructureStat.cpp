#include "pch.h"
#include "CStructureStat.h"

#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/Structure/CBarrel.h"
#include "Source/Scripts/Structure/CStructure.h"
#include "Source/Scripts/Structure/StructureHPBar/CStructureHPBar.h"

CStructureStat::CStructureStat()
    : CStatScript(SCRIPT_TYPE::STRUCTURESTAT)
{
}

CStructureStat::~CStructureStat()
{
}

void CStructureStat::Begin()
{
    CStatScript::Begin();

    if (GameObject* ChildHPBar = GetOwner()->GetChildByName(L"StructureHPBar").Get())
    {
        if (GetOwner()->GetScriptComponent<CStructure>()->GetIsPreviewObject())
        {
            ChildHPBar->SetActive(false);
            return;
        }
        
        m_StructureHPBar = ChildHPBar->GetScriptComponent<CStructureHPBar>().Get();
    }
}

bool CStructureStat::TakeDamage(float _DamageAmount, GameObject* _DamageCauser)
{
    // Preview 오브젝트인 경우, 그냥 넘어가야함
    CStructure* StructureScript = GetOwner()->GetScriptComponent<CStructure>().Get();
    if (StructureScript->GetIsPreviewObject()) return false;
    
    if (!CStatScript::TakeDamage(_DamageAmount, _DamageCauser)) return false;

    // HPBar Update
    if (m_StructureHPBar) m_StructureHPBar->UpdateHPBar(m_HP, m_HPMax);

    return IsDead() ? StructureScript->DestroyStructure(true) : true; 
}
