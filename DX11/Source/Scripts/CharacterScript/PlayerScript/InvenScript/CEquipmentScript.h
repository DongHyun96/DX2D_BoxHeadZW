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
    /// </summary>
    /// <param name="EquipSlot"></param>
    /// <returns> 처음으로 Slot에 추가된 경우라면 return true (처음 Slot에 추가된 경우가 아니거나, Valid하지 않은 무기 종류가 들어온 경우 return false) </returns>
    bool EquipWeapon(PLAYER_HANDSTATE EquipSlot);

    /// <summary>
    /// 장착된 무기 반환 (없다면 nullptr)
    /// </summary>
    /// <param name="_HandState"></param>
    /// <returns></returns>
    Ptr<CWeaponScript> GetEquippedWeapon(PLAYER_HANDSTATE _HandState) const;
    
    const map<PLAYER_HANDSTATE, Ptr<CWeaponScript>>& GetEquippedWeapons() const { return m_mapEquippedWeapons; }

    /// <summary>
    /// 다음으로 해금할 종류의 SlotType 반환
    /// </summary>
    /// <returns> : 모두 해금하였다면 return PLAYER_HANDSTATE::END </returns>
    PLAYER_HANDSTATE GetNextWeaponTypeToUnlock() const;
    
public:
    
    void SaveToLevelFile(FILE* _File) override {};
    void LoadFromLevelFile(FILE* _File) override {};
};
