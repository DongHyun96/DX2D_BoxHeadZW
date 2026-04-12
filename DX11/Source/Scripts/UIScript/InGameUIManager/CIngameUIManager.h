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
    GameObject*                             BulletIcon{};
    
    map<PLAYER_STRUCTURE_TYPE, GameObject*> StructureIcons{};
};

class CIngameUIManager : public CScript
{
private:

    AmmoCountUIArea m_AmmoCountUIArea{};
    
public:
    CIngameUIManager();
    virtual ~CIngameUIManager() override;
    CLONE(CIngameUIManager);
    
public:

    virtual void Begin() override;
    virtual void Tick() override;

public:
    
    void SaveToLevelFile(FILE* _File) override {}
    void LoadFromLevelFile(FILE* _File) override {}
};
