#include "pch.h"
#include "CInvenScript.h"

#include "Source/ScriptMgr.h"

CInvenScript::CInvenScript()
    : CScript(SCRIPT_TYPE::INVENSCRIPT)
{
}

CInvenScript::~CInvenScript()
{
}

void CInvenScript::Begin()
{
    // For testing (나중에는 파밍해서 처리할 것)
    m_mapStructureCount = 
    {
        {PLAYER_STRUCTURE_TYPE::BARRICADE,          50},    
        {PLAYER_STRUCTURE_TYPE::BARREL,             50},    
        {PLAYER_STRUCTURE_TYPE::TURRET_MACHINE_GUN, 50},    
        {PLAYER_STRUCTURE_TYPE::TURRET_MORTAR,      50},    
        {PLAYER_STRUCTURE_TYPE::TURRET_ROCKET,      50},    
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
