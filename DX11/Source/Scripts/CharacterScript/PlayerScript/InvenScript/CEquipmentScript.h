#pragma once
#include "Source/Scripts/CharacterScript/PlayerScript/WeaponScript/CWeaponScript.h"




class CEquipmentScript : public CScript
{
private:
    
    map<PLAYER_HANDSTATE, Ptr<CWeaponScript>> m_mapEquippedWeapons{};
    
public:

    CEquipmentScript();
    virtual ~CEquipmentScript() override;
    CLONE(CEquipmentScript)
    
public:

    virtual void Begin() override;
    virtual void Tick() override;
    
public:
    
    /// <summary>
    /// 해당 Slot에 Weapon 새로이 장착
    /// 주의 : 아예 새로운 Weapon으로 장착 처리됨
    /// </summary>
    /// <param name="EquipSlot"></param>
    void EquipWeapon(PLAYER_HANDSTATE EquipSlot);

    /// <summary>
    /// 장착된 무기 반환 (없다면 nullptr)
    /// </summary>
    /// <param name="_HandState"></param>
    /// <returns></returns>
    Ptr<CWeaponScript> GetEquippedWeapon(PLAYER_HANDSTATE _HandState) const;
    
public:
    
    void SaveToLevelFile(FILE* _File) override {};
    void LoadFromLevelFile(FILE* _File) override {};
};
