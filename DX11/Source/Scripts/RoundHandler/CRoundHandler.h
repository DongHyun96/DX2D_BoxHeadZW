#pragma once
#include "GameEngine/04.Asset/10.Sound/ASound.h"

struct RoundInfo
{
    map<ENEMY_TYPE, pair<UINT, UINT>> EachEnemyFirstSpawnCountMinMax =
    {
        {ENEMY_TYPE::ZOMBIE,    {0, 0}},
        {ENEMY_TYPE::MUMMY,     {0, 0}},
        {ENEMY_TYPE::VAMPIRE,   {0, 0}},
        {ENEMY_TYPE::RUNNER,    {0, 0}},
        {ENEMY_TYPE::DEVIL,     {0, 0}},    
    };
    
    map<ENEMY_TYPE, pair<UINT, UINT>> EachEnemyAdditionalSpawnCountMinMax =
    {
        {ENEMY_TYPE::ZOMBIE,    {0, 0}},
        {ENEMY_TYPE::MUMMY,     {0, 0}},
        {ENEMY_TYPE::VAMPIRE,   {0, 0}},
        {ENEMY_TYPE::RUNNER,    {0, 0}},
        {ENEMY_TYPE::DEVIL,     {0, 0}},    
    };
    
    float AdditionalSpawnStartTime{};
    
};


class CRoundHandler : public CScript
{
private:
    
    Ptr<ASound> m_RoundStartSound{};
    
private:

    ROUND_STATE         m_RoundState{};
    
    vector<RoundInfo>   m_vecRoundInfo{}; // 들어가져 있는 Info대로 라운드를 진행하다가, 더 이상 라운드 정보가 없는 라운드는 난이도 최대치로 continue
    UINT                m_CurrentRoundIdx = -1;
    
    RoundInfo*          m_CurrentRoundInfo{};

private:

    // 이번 라운드 Additional 스폰 처리 남은 Enemy 수
    map<ENEMY_TYPE, UINT> m_ThisRoundAdditionalSpawnLeft{};
    
private:
    
    static const float  s_RoundWaitTime;
    float               m_RoundWaitTimer = s_RoundWaitTime;
    
    
    float m_AdditionalSpawnInterval{};
    
    float m_CurrentRoundTimer{}; // 현재 Round 진행 시간 측정
    float m_AdditionalSpawnTimer{};
    float m_AllDieCheckTimer{};

public:
    
    CRoundHandler();
    virtual ~CRoundHandler() override;
    CLONE(CRoundHandler);
    
public:
    
    virtual void Init() override;
    virtual void Tick() override;

private:
    
    void HandleTransition();
    
public:
    
    void SetRoundState(ROUND_STATE _RoundState);
    ROUND_STATE GetRoundState() const { return m_RoundState; }
    
public:
    
    void SaveToLevelFile(FILE* _File) override;
    void LoadFromLevelFile(FILE* _File) override;
};
