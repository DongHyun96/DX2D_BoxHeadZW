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
    
    float m_AccTime = 0.f;
    bool  m_bPrevGameStart = false;

public:
    CIngameUIManager();
    CIngameUIManager(const CIngameUIManager& _Origin);
    virtual ~CIngameUIManager() override;
    CLONE(CIngameUIManager);
    
public:

    virtual void Begin() override;
    virtual void Tick() override;
    
public:
    
    const Ptr<AmmoCountUIArea>& GetAmmoCountUIArea() const { return m_AmmoCountUIArea; }
    const Ptr<RoundIndicators>& GetRoundIndicators() const { return m_RoundIndicators; }
    CText*           GetZombieAliveCount() const    { return m_ZombieAliveCount; }

    CCrossHair* GetCrossHair() const { return m_CrossHair; }
    void SetCrossHair(CCrossHair* _CrossHair) { m_CrossHair = _CrossHair; }
    
private:

    void InitMembers();
    
public:
    
    void SaveToLevelFile(FILE* _File) override {}
    void LoadFromLevelFile(FILE* _File) override {}
};
