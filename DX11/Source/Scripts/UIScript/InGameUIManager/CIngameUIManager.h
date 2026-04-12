#pragma once

enum class UI_NAVIGATION
{
    LOBBY,
    PLAYING,
    END
};

struct AmmoCountUIArea
{
    class CText*                            MainAmmoCount{};
    CText*                                  GrenadeCount{};
    CText*                                  AirStrikeCount{};

    map<PLAYER_HANDSTATE, GameObject*>      WeaponIcons{}; 
    GameObject*                             RoundAmmoIcon{};
    
    map<PLAYER_STRUCTURE_TYPE, GameObject*> StructureIcons{};
    
    class CProgressBar*                     HPBar{};
};

struct RoundIndicators
{
    CText* RoundText{};
    CText* RoundWaitText{};
    CText* RoundWaitTimeText{};
};

class CIngameUIManager : public CScript
{
private:

    AmmoCountUIArea m_AmmoCountUIArea{};
    RoundIndicators m_RoundIndicators{};
    CText*          m_ZombieAliveCount{};
    
public:
    CIngameUIManager();
    virtual ~CIngameUIManager() override;
    CLONE(CIngameUIManager);
    
public:

    virtual void Begin() override;
    virtual void Tick() override;
    
public:
    
    AmmoCountUIArea& GetAmmoCountUIAreaRef()        { return m_AmmoCountUIArea; }
    RoundIndicators& GetRoundIndicatorsRef()        { return m_RoundIndicators; }
    CText*           GetZombieAliveCount() const    { return m_ZombieAliveCount; }

private:

    void InitMembers();
    
public:
    
    void SaveToLevelFile(FILE* _File) override {}
    void LoadFromLevelFile(FILE* _File) override {}
};
