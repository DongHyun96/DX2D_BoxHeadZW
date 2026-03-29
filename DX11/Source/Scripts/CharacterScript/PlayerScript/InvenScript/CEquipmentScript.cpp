#include "pch.h"
#include "CEquipmentScript.h"

#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "Source/ScriptMgr.h"
#include "Source/Scripts/CharacterScript/PlayerScript/WeaponScript/CWeaponPistol.h"

CEquipmentScript::CEquipmentScript()
    : CScript(SCRIPT_TYPE::EQUIPMENTSCRIPT)
{
    // TODO : 권총을 제외한 다른 무기들은 파밍을 통해 구할 것
    EquipWeapon(PLAYER_HANDSTATE::PISTOL);
}

CEquipmentScript::~CEquipmentScript()
{
}

void CEquipmentScript::Begin()
{
    CWeaponScript::AddHitScanLayer(LevelMgr::GetInst()->GetCurLevel()->GetLayerIndexByLayerName(L"Enemy"));
    CWeaponScript::AddHitScanLayer(LevelMgr::GetInst()->GetCurLevel()->GetLayerIndexByLayerName(L"MapObstacle"));
}

void CEquipmentScript::Tick()
{
    for (const pair<const PLAYER_HANDSTATE, Ptr<CWeaponScript>>& Pair : m_mapEquippedWeapons)
        Pair.second->WeaponTick();
    
    
}

void CEquipmentScript::EquipWeapon(PLAYER_HANDSTATE EquipSlot)
{
    // TODO : 나머지 Weapon 들에 대해서도 클래스 만들고 Concrete 클래스 객체 할당하는 처리 넣기
    switch (EquipSlot)
    {
    case PLAYER_HANDSTATE::PISTOL: m_mapEquippedWeapons[EquipSlot] = new CWeaponPistol; return;
        
    case PLAYER_HANDSTATE::UZI:
        break;
    case PLAYER_HANDSTATE::SHOTGUN:
        break;
    case PLAYER_HANDSTATE::MINIGUN:
        break;
    case PLAYER_HANDSTATE::ROCKET:
        break;
    case PLAYER_HANDSTATE::UNARMED: case PLAYER_HANDSTATE::END: break; 
    }
}

Ptr<CWeaponScript> CEquipmentScript::GetEquippedWeapon(PLAYER_HANDSTATE _HandState) const
{
    if (!m_mapEquippedWeapons.contains(_HandState)) return nullptr;
    return m_mapEquippedWeapons.at(_HandState);
}
