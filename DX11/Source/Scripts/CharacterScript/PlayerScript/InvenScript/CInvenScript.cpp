#include "pch.h"
#include "CInvenScript.h"

#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/UIScript/InGameUIManager/CIngameUIManager.h"

CInvenScript::CInvenScript()
    : CScript(SCRIPT_TYPE::INVENSCRIPT)
{
}

CInvenScript::~CInvenScript()
{
}

void CInvenScript::Begin()
{
    // 초기값 부여
    m_mapStructureCount = 
    {
        {PLAYER_STRUCTURE_TYPE::BARRICADE,          30},    
        {PLAYER_STRUCTURE_TYPE::BARREL,             30},    
        {PLAYER_STRUCTURE_TYPE::TURRET_MACHINE_GUN, 30},    
        {PLAYER_STRUCTURE_TYPE::TURRET_MORTAR,      30},    
        {PLAYER_STRUCTURE_TYPE::TURRET_ROCKET,      30},    
    };
    
}

void CInvenScript::Tick()
{
}

bool CInvenScript::HasAnyStructure() const
{
    for (const pair<const PLAYER_STRUCTURE_TYPE, int>& Pair : m_mapStructureCount)
        if (Pair.second != 0) return true;
    return false;
}

int CInvenScript::ReduceCurrentStructureCount(PLAYER_STRUCTURE_TYPE _StructureType, int _ReduceAmount)
{
    m_mapStructureCount[_StructureType] -= _ReduceAmount;
    
    if (m_mapStructureCount[_StructureType] < 0)
    {
        DebugUtil::AddDebugLog("[CInvenScript::ReduceCurrentStructureCount] : Count Reduced to under 0.", DEF_COLOR_BLUE, 10.f);
        m_mapStructureCount[_StructureType] = 0;
    }
    return m_mapStructureCount[_StructureType];
}

int CInvenScript::IncreaseCurrentStructureCount(PLAYER_STRUCTURE_TYPE _StructureType, int _IncreaseAmount)
{
    m_mapStructureCount[_StructureType] += _IncreaseAmount;
    return m_mapStructureCount[_StructureType];
}

int CInvenScript::GetStructureCount(PLAYER_STRUCTURE_TYPE _StructureType) const
{
    if (!m_mapStructureCount.contains(_StructureType)) return 0;
    return m_mapStructureCount.at(_StructureType);
}

void CInvenScript::ReduceCurrentAmmoCount(PLAYER_HANDSTATE _HandState, int _ReduceAmount)
{
    if (!m_AmmoLeft.contains(_HandState)) return;
    
    m_AmmoLeft[_HandState] = max(m_AmmoLeft[_HandState] - _ReduceAmount, 0);
    
    if (_HandState != PLAYER_HANDSTATE::PISTOL)
        GM->GetIngameUIManager()->GetAmmoCountUIAreaRef().UpdateCurrentAmmoCount(m_AmmoLeft[_HandState]);   
}
