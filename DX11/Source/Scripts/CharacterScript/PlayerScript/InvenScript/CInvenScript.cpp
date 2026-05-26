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
    
    GM->GetIngameUIManager()->GetAmmoCountUIArea()->UpdateCurrentAmmoCount(m_mapStructureCount[_StructureType]);
    
    return m_mapStructureCount[_StructureType];
}

int CInvenScript::IncreaseCurrentStructureCount(PLAYER_STRUCTURE_TYPE _StructureType, int _IncreaseAmount)
{
    m_mapStructureCount[_StructureType] += _IncreaseAmount;

    // GameLog 처리
    wstring GameLog{};
    switch (_StructureType)
    {
    case PLAYER_STRUCTURE_TYPE::BARRICADE:          GameLog = L"EARNED BARRICADE";          break; 
    case PLAYER_STRUCTURE_TYPE::BARREL:             GameLog = L"EARNED BARREL";             break;
    case PLAYER_STRUCTURE_TYPE::TURRET_MACHINE_GUN: GameLog = L"EARNED TURRET_MACHINE_GUN"; break;
    case PLAYER_STRUCTURE_TYPE::TURRET_MORTAR:      GameLog = L"EARNED TURRET_MORTAR";      break;
    case PLAYER_STRUCTURE_TYPE::TURRET_ROCKET:      GameLog = L"EARNED TURRET_ROCKET";      break;
    case PLAYER_STRUCTURE_TYPE::END:                                                        return 0;
    }
    
    GM->GetIngameUIManager()->AddGameLog(GameLog);
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
        GM->GetIngameUIManager()->GetAmmoCountUIArea()->UpdateCurrentAmmoCount(m_AmmoLeft[_HandState]);   
}

void CInvenScript::IncreaseCurrentAmmoCount(PLAYER_HANDSTATE _HandState, int _IncreaseAmount)
{
    m_AmmoLeft.at(_HandState) += _IncreaseAmount;
    
    // GameLog 처리
    wstring GameLog{};

    switch (_HandState)
    {
    case PLAYER_HANDSTATE::PISTOL: case PLAYER_HANDSTATE::UNARMED: case PLAYER_HANDSTATE::END: return;
    case PLAYER_HANDSTATE::UZI:     GameLog = L"EARNED UZI AMMO";       break;
    case PLAYER_HANDSTATE::SHOTGUN: GameLog = L"EARNED SHOTGUN AMMO";   break;
    case PLAYER_HANDSTATE::MINIGUN: GameLog = L"EARNED MINIGUN AMMO";   break;
    case PLAYER_HANDSTATE::ROCKET:  GameLog = L"EARNED ROCKET AMMO";    break;
    }
    
    GM->GetIngameUIManager()->AddGameLog(GameLog);
}
