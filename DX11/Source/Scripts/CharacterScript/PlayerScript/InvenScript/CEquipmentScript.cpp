#include "pch.h"
#include "CEquipmentScript.h"

#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "Source/ScriptMgr.h"
#include "Source/Scripts/CharacterScript/PlayerScript/WeaponScript/CWeaponMinigun.h"
#include "Source/Scripts/CharacterScript/PlayerScript/WeaponScript/CWeaponPistol.h"
#include "Source/Scripts/CharacterScript/PlayerScript/WeaponScript/CWeaponRocket.h"
#include "Source/Scripts/CharacterScript/PlayerScript/WeaponScript/CWeaponShotgun.h"
#include "Source/Scripts/CharacterScript/PlayerScript/WeaponScript/CWeaponUzi.h"


CEquipmentScript::CEquipmentScript()
    : CScript(SCRIPT_TYPE::EQUIPMENTSCRIPT)
{
    // Default로 제공되는 무기는 가지고 시작
    EquipWeapon(PLAYER_HANDSTATE::PISTOL);
    EquipWeapon(PLAYER_HANDSTATE::UZI);
    /*EquipWeapon(PLAYER_HANDSTATE::SHOTGUN);
    EquipWeapon(PLAYER_HANDSTATE::MINIGUN);
    EquipWeapon(PLAYER_HANDSTATE::ROCKET);*/
}

CEquipmentScript::~CEquipmentScript()
{
}

void CEquipmentScript::Begin()
{
    CWeaponScript::AddHitScanLayer(LevelMgr::GetInst()->GetCurLevel()->GetLayerIndexByLayerName(L"MapObstacle")); // 2
    CWeaponScript::AddHitScanLayer(LevelMgr::GetInst()->GetCurLevel()->GetLayerIndexByLayerName(L"Enemy")); // 4
    CWeaponScript::AddHitScanLayer(LevelMgr::GetInst()->GetCurLevel()->GetLayerIndexByLayerName(L"Barrel")); // 8
    
}

void CEquipmentScript::Tick()
{
    for (const pair<const PLAYER_HANDSTATE, Ptr<CWeaponScript>>& Pair : m_mapEquippedWeapons)
        Pair.second->WeaponTick();
    
    
}

bool CEquipmentScript::EquipWeapon(PLAYER_HANDSTATE EquipSlot)
{
    if (m_mapEquippedWeapons.contains(EquipSlot)) return false;
    
    switch (EquipSlot)
    {
    case PLAYER_HANDSTATE::PISTOL: // Default Weapon
    {
        m_mapEquippedWeapons[EquipSlot] = new CWeaponPistol;
        return true;
    }
    case PLAYER_HANDSTATE::UZI:
    {
        m_mapEquippedWeapons[EquipSlot] = new CWeaponUzi;
        return true;
    }
    case PLAYER_HANDSTATE::SHOTGUN:
    {
        m_mapEquippedWeapons[EquipSlot] = new CWeaponShotgun;
        return true;
    }
    case PLAYER_HANDSTATE::MINIGUN:
    {
        m_mapEquippedWeapons[EquipSlot] = new CWeaponMinigun;
        
        return true;
    }
    case PLAYER_HANDSTATE::ROCKET:
    {
        m_mapEquippedWeapons[EquipSlot] = new CWeaponRocket;    
        return true;
    }
    default: return false;
    }
}

Ptr<CWeaponScript> CEquipmentScript::GetEquippedWeapon(PLAYER_HANDSTATE _HandState) const
{
    if (!m_mapEquippedWeapons.contains(_HandState)) return nullptr;
    return m_mapEquippedWeapons.at(_HandState);
}

PLAYER_HANDSTATE CEquipmentScript::GetNextWeaponTypeToUnlock() const
{
    auto it = prev(m_mapEquippedWeapons.end());
    return static_cast<PLAYER_HANDSTATE>(static_cast<int>(it->first) + 1);
}
