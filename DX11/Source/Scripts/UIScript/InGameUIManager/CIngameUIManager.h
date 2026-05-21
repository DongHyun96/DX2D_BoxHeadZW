#pragma once
#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "Source/Scripts/UIScript/CText.h"
#include "Source/Scripts/UIScript/HUD/HUD.h"

enum class UI_NAVIGATION
{
    LOBBY,
    PLAYING,
    END
};

class CIngameUIManager : public CScript
{
private:

    Ptr<AmmoCountUIArea> m_AmmoCountUIArea{};
    Ptr<RoundIndicators> m_RoundIndicators{};
    CText*               m_ZombieAliveCount{};
    
    class CCrossHair* m_CrossHair{};

    class CGameLogManager* m_GameLogManager{};

public:
    CIngameUIManager();
    virtual ~CIngameUIManager() override;
    CLONE(CIngameUIManager);
    
public:

    virtual void Begin() override;
    virtual void Tick() override;

public:
    
    void SetZombieAliveCountText(int _AliveCount);
    
public:
    
    const Ptr<AmmoCountUIArea>& GetAmmoCountUIArea() const { return m_AmmoCountUIArea; }
    const Ptr<RoundIndicators>& GetRoundIndicators() const { return m_RoundIndicators; }

    CCrossHair* GetCrossHair() const { return m_CrossHair; }
    void SetCrossHair(CCrossHair* _CrossHair) { m_CrossHair = _CrossHair; }
    
private:

    void InitMembers();
    void InitPlayerHUDMembers(const Ptr<GameObject>& PlayerHUD);
    void InitAnimationGroupMembers(const Ptr<GameObject>& AnimationGroups);
    
public:
    
    void SaveToLevelFile(FILE* _File) override {}
    void LoadFromLevelFile(FILE* _File) override {}
};
