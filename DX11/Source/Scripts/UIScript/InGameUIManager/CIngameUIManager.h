#pragma once
#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "Source/Scripts/UIScript/CText.h"

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
    
public:

    void UpdateToGun(PLAYER_HANDSTATE _HandState, int _MainAmmoCount)
    {

        for (auto weapon_icon : WeaponIcons)
            weapon_icon.second->SetActive(false);
        
        for (auto structure_icon : StructureIcons)
            structure_icon.second->SetActive(false);
        
        WeaponIcons[_HandState]->SetActive(true);
        RoundAmmoIcon->SetActive(true);
        
        if (_HandState == PLAYER_HANDSTATE::PISTOL)
        {
            MainAmmoCount->SetText(L"INF");
            MainAmmoCount->SetColor(DEF_COLOR_WHITE);
        }
        else
        {
            MainAmmoCount->SetText(to_wstring(_MainAmmoCount));
            MainAmmoCount->SetColor((_MainAmmoCount <= 0) ? Vec4(1.f, 0.f, 0.f, 1.f) : DEF_COLOR_WHITE);
        }
    }
    
    void UpdateCurrentAmmoCount(int _AmmoCount)
    {
        MainAmmoCount->SetText(to_wstring(_AmmoCount));
        MainAmmoCount->SetColor((_AmmoCount <= 0) ? Vec4(1.f, 0.f, 0.f, 1.f) : DEF_COLOR_WHITE);
    }
    
    void UpdateToStructure(PLAYER_STRUCTURE_TYPE _Type, int _Count)
    {
        for (auto weapon_icon : WeaponIcons)
            weapon_icon.second->SetActive(false);
        
        for (auto structure_icon : StructureIcons)
            structure_icon.second->SetActive(false);
        
        RoundAmmoIcon->SetActive(false);
        
        StructureIcons[_Type]->SetActive(true);
        
        MainAmmoCount->SetText(to_wstring(_Count));
    }
};

struct RoundIndicators
{
    CText* RoundText{}; // Round 1 Round 2
    CText* RoundWaitText{}; // Waiting for next round
    CText* RoundWaitTimeText{}; // time text

private:
    
    float RoundNumberTimer{};
    int RoundTextPhase = 0;
    
public:
    
    void OnRoundWaitStart()
    {
        RoundText->SetAlpha(0.f);
        
        RoundWaitText->Transform()->SetRelativePosY(150.f); // 85
        RoundWaitTimeText->Transform()->SetRelativePosY(-35.f); // 35
        RoundWaitText->SetAlpha(0.f);
        RoundWaitTimeText->SetAlpha(0.f);
        
        RoundNumberTimer = 0.f;
    }
    void OnRoundWaiting(float RoundWaitTime)
    {
        {
            float Y = RoundWaitText->Transform()->GetRelativePosY();
            Y = Lerp(Y, 85.f, DT * 20.f);
            RoundWaitText->Transform()->SetRelativePosY(Y); // 85
        }
        {
            float Y = RoundWaitTimeText->Transform()->GetRelativePosY();
            Y = Lerp(Y, 35.f, DT * 20.f);
            RoundWaitTimeText->Transform()->SetRelativePosY(Y); // 35
        }
        {
            float Alpha = RoundWaitText->GetAlpha();
            Alpha = Lerp(Alpha, 1.f, DT * 10.f);
            RoundWaitText->SetAlpha(Alpha);
            RoundWaitTimeText->SetAlpha(Alpha);
        }

        int RoundWaitTimeToInt = static_cast<int>(RoundWaitTime);
        RoundWaitTimeText->SetText(to_wstring(RoundWaitTimeToInt + 1));
    }
    
    void OnRoundStart(int RoundNumber)
    {
        RoundText->SetText(L"Round  " + to_wstring(RoundNumber));
        RoundText->SetAlpha(0.f);
    
        RoundWaitText->SetAlpha(0.f);
        RoundWaitTimeText->SetAlpha(0.f);

        // 새 라운드 시작 시 타이머와 상태를 초기화합니다.
        RoundNumberTimer = 0.f;
        RoundTextPhase   = 0; 
    }
    void OnRounding()
    {
        // 1단계: 서서히 나타나기 (Fade In)
        if (RoundTextPhase == 0)
        {
            float Alpha = RoundText->GetAlpha();
            Alpha = Lerp(Alpha, 1.f, DT * 15.f);
            RoundText->SetAlpha(Alpha);
        
            // 완전히 나타나면 다음 단계(대기)로 넘어감
            if (Alpha > 0.99f) // 자연스러운 연출을 위해 0.99 사용
            {
                RoundText->SetAlpha(1.f);
                RoundTextPhase = 1; 
            }
        }
        // 2단계: 2초간 대기 (Wait)
        else if (RoundTextPhase == 1)
        {
            RoundNumberTimer += DT;
        
            // 2초가 지나면 다음 단계(사라지기)로 넘어감
            if (RoundNumberTimer > 2.f)
            {
                RoundTextPhase = 2; 
            }
        }
        // 3단계: 서서히 사라지기 (Fade Out)
        else if (RoundTextPhase == 2)
        {
            float Alpha = RoundText->GetAlpha();
            Alpha = Lerp(Alpha, 0.f, DT * 15.f); // 사라지는 속도 조절 가능
            RoundText->SetAlpha(Alpha);

            // 완전히 사라지면 연산 종료
            if (Alpha < 0.01f)
            {
                RoundText->SetAlpha(0.f);
                RoundTextPhase = 3; // 3이 되면 더 이상 Lerp 연산을 하지 않음
            }
        }
    }
    
    
    void OnGameOver()
    {
        RoundNumberTimer = 0.f;
        RoundText->SetAlpha(1.f);
        RoundWaitText->SetAlpha(0.f); // Waiting for next round
        RoundWaitTimeText->SetAlpha(0.f); // time text
        
        RoundText->SetText(L"Game Over");
    }
};

class CIngameUIManager : public CScript
{
private:

    AmmoCountUIArea m_AmmoCountUIArea{};
    RoundIndicators m_RoundIndicators{};
    CText*          m_ZombieAliveCount{};
    
    class CCrossHair* m_CrossHair{};
    
    float m_AccTime = 0.f;
    bool  m_bPrevGameStart = false;

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

    CCrossHair* GetCrossHair() const { return m_CrossHair; }
    void SetCrossHair(CCrossHair* _CrossHair) { m_CrossHair = _CrossHair; }
    
private:

    void InitMembers();
    
public:
    
    void SaveToLevelFile(FILE* _File) override {}
    void LoadFromLevelFile(FILE* _File) override {}
};
