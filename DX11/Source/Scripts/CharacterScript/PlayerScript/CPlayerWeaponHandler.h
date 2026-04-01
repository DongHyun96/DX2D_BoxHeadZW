#pragma once
/// <summary>
/// 하수, 중수, 고수 순으로 버프 처리
/// </summary>
enum class WEAPON_MASTERY
{
    BEGINNER,
    INTERMEDIATE,
    MASTER
};

struct WeaponMasteryData
{
    WEAPON_MASTERY  CurrentMasteryState{};
    float           EXP{};
};

struct WeaponMasteryBuff
{
    float FireRPM{}; // 60초당 몇발 발사 가능한지
    float DamageAmountPerRound{};
};

class CPlayerWeaponHandler : public CScript
{
private: // TODO : 수치는 추후 조정할 것
    
    // 총기별 각종 상수 수치 관련
    
    // 각 무기별 MasteryEXP Max값
    const map<PLAYER_HANDSTATE, vector<float>> EACH_WEAPON_MASTERY_EXP_MAX =  
    {
        {PLAYER_HANDSTATE::PISTOL,  {500.f, 1000.f, 1500.f}},    
        {PLAYER_HANDSTATE::UZI,     {500.f, 1000.f, 1500.f}},    
        {PLAYER_HANDSTATE::SHOTGUN, {500.f, 1000.f, 1500.f}},    
        {PLAYER_HANDSTATE::MINIGUN, {500.f, 1000.f, 1500.f}},    
        {PLAYER_HANDSTATE::ROCKET,  {500.f, 1000.f, 1500.f}}  
    };
    
    // 1 kill 당 보상 EXP 량
    const map<ENEMY_TYPE, float> REWARD_EXP_PER_KILL = 
    {
        {ENEMY_TYPE::ZOMBIE,    20.f},   
        {ENEMY_TYPE::MUMMY,     25.f},   
        {ENEMY_TYPE::VAMPIRE,   30.f},           
        {ENEMY_TYPE::RUNNER,    35.f},
        {ENEMY_TYPE::DEVIL,     50.f}, 
    };
    
    // 각 무기별 버프처리
    const map<PLAYER_HANDSTATE, vector<WeaponMasteryBuff>> EACH_WEAPON_MASTERY_BUFF = 
    {
        {
            PLAYER_HANDSTATE::PISTOL,
            {
                WeaponMasteryBuff(150.f, 30.f),
                WeaponMasteryBuff(200.f, 40.f),
                WeaponMasteryBuff(400.f, 50.f)
            }
        },   
        
        { 
            PLAYER_HANDSTATE::UZI,
            {
                WeaponMasteryBuff(400.f, 30.f),
                WeaponMasteryBuff(600.f, 40.f),
                WeaponMasteryBuff(800.f, 50.f)
            }
        },  
        
        { 
            PLAYER_HANDSTATE::SHOTGUN,
            {
                WeaponMasteryBuff(120.f, 30.f),
                WeaponMasteryBuff(180.f, 40.f),
                WeaponMasteryBuff(300.f, 50.f)
            }
        },  
        
        { 
            PLAYER_HANDSTATE::MINIGUN,
            {
                WeaponMasteryBuff(1000.f, 30.f),
                WeaponMasteryBuff(1200.f, 40.f),
                WeaponMasteryBuff(1500.f, 50.f)
            }
        },  
        
        { 
            PLAYER_HANDSTATE::ROCKET,
            {
                WeaponMasteryBuff(75.f, 30.f),
                WeaponMasteryBuff(100.f, 40.f),
                WeaponMasteryBuff(150.f, 50.f)
            }
        },  
    };

    
private:

    using MuzzleOffsets = array<Vec2, static_cast<int>(EDIRECTION::END)>;
    map<PLAYER_HANDSTATE, MuzzleOffsets> m_mapEachMuzzleOffsets =  // 각 무기별, Muzzle Offset 값들 (WorldPos 기준 좌표에서의 Offset)
    {
        {PLAYER_HANDSTATE::PISTOL,  {}},
        {PLAYER_HANDSTATE::UZI,     {}},
        {PLAYER_HANDSTATE::SHOTGUN, {}},
        {PLAYER_HANDSTATE::MINIGUN, {}},
        {PLAYER_HANDSTATE::ROCKET,  {}}
    };
    
private:
    
    map<PLAYER_HANDSTATE, WeaponMasteryData> m_mapCurrentMastery{};

private:
    
    class CPlayerScript* m_PlayerMainScript{};
    class CEquipmentScript* m_EquipmentScript{};
    
private:

    // 이전 Tick에 쐈는지 체크
    bool m_LastTickFired{};
    
public:
    
    CPlayerWeaponHandler();
    virtual ~CPlayerWeaponHandler() override;
    
    CLONE(CPlayerWeaponHandler)
    
public:

    virtual void Init() override;
    virtual void Begin() override;
    virtual void Tick() override;
    
private:
    
    void TickSwapWeapon();
    void TickFireWeapon();
    
public:
    
    /// <summary>
    /// Player의 HandState와 현재 바라보는 Direction에 따른 MuzzleOffset값 반환
    /// </summary>
    /// <returns></returns>
    const Vec2& GetCurrentMuzzleOffset();
    
public:
    
    void SaveToLevelFile(FILE* _File) override;
    void LoadFromLevelFile(FILE* _File) override;
};
