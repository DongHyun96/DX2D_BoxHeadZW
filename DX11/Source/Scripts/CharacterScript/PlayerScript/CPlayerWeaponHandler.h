#pragma once


class CPlayerWeaponHandler : public CScript
{
private:

    using MuzzleOffsets = array<Vec2, static_cast<int>(EDIRECTION::END)>;
    map<PLAYER_HANDSTATE, MuzzleOffsets> m_mapEachMuzzleOffsets =  // 각 무기별, Muzzle Offset 값들
    {
        {PLAYER_HANDSTATE::PISTOL,  {}},
        {PLAYER_HANDSTATE::UZI,     {}},
        {PLAYER_HANDSTATE::SHOTGUN, {}},
        {PLAYER_HANDSTATE::MINIGUN, {}},
        {PLAYER_HANDSTATE::ROCKET,  {}}
    };
    
    class CPlayerScript* m_PlayerMainScript{};
    class CEquipmentScript* m_EquipmentScript{};
    
public:
    
    CPlayerWeaponHandler();
    virtual ~CPlayerWeaponHandler() override;
    
    CLONE(CPlayerWeaponHandler)

    // TODO : 여기 무조건 위에꺼로 수정할 것!!!
    // CPlayerWeaponHandler* Clone() const override { return const_cast<CPlayerWeaponHandler*>(this); }
    
public:

    virtual void Init() override;
    virtual void Begin() override;
    virtual void Tick() override;
    
private:
    
    void TickSwapWeapon();
    void TickFireWeapon();
    
public:
    
    void SaveToLevelFile(FILE* _File) override;
    void LoadFromLevelFile(FILE* _File) override;
};
