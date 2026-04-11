#include "pch.h"
#include "CRoundHandler.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/CharacterScript/EnemyScript/EnemySpawnHandler/CEnemySpawnHandler.h"

const float CRoundHandler::s_RoundWaitTime = 15.f;

CRoundHandler::CRoundHandler()
    : CScript(SCRIPT_TYPE::ROUNDHANDLER)
{
}

CRoundHandler::~CRoundHandler()
{
}

void CRoundHandler::Init()
{
    // Init이 먼저 불리고, LoadFromLevelFile이 그 다음 순   
}

void CRoundHandler::Tick()
{
    HandleTransition();
}

void CRoundHandler::HandleTransition()
{
    switch (m_RoundState)
    {
    case ROUND_STATE::WAIT:
    {
        m_RoundWaitTimer += DT;
        
        if (m_RoundWaitTimer < s_RoundWaitTime) return;
        
        m_RoundWaitTimer = 0.f;
        SetRoundState(ROUND_STATE::ROUND_GOING);
        return;
    }
    case ROUND_STATE::ROUND_GOING:
    {
        m_CurrentRoundTimer += DT;
        
        if (m_CurrentRoundTimer < )
        
        return;
    }
        
    case ROUND_STATE::GAME_OVER:
    {
        
        return;
    }
        
    }
}

void CRoundHandler::SetRoundState(ROUND_STATE _RoundState)
{
    m_RoundState = _RoundState;
    
    switch (m_RoundState)
    {
    case ROUND_STATE::WAIT:
    {
        m_RoundWaitTimer = 0.f;
        return;
    }
        
    case ROUND_STATE::ROUND_GOING:
    {
        ++m_CurrentRoundIdx;
        m_CurrentRoundTimer = 0.f;
        
        // 첫 Spawn 처리 (RoundInfo가 있다면)
        if (m_CurrentRoundIdx < m_vecRoundInfo.size())
        {
            const RoundInfo& rInfo = m_vecRoundInfo[m_CurrentRoundIdx];

            for (const pair<const ENEMY_TYPE, pair<unsigned, unsigned>>& FirstSpawnCountMinMaxPair : rInfo.EachEnemyFirstSpawnCountMinMax)
            {
                const UINT SpawnCount = GetRandom(FirstSpawnCountMinMaxPair.second.first, FirstSpawnCountMinMaxPair.second.second);
                
                for (int i = 0; i < SpawnCount; ++i)
                {
                    const UINT SpawnLocation = GetRandom(static_cast<UINT>(FIRST_SPAWN_LOC1), static_cast<UINT>(FIRST_SPAWN_LOC_END - 1));
                    GM->GetEnemySpawnHandler()->SpawnEnemyOnFirstSpawnArea(FirstSpawnCountMinMaxPair.first, static_cast<FIRST_SPAWN_LOC>(SpawnLocation));
                }
            }
        }
        else // 없다면 통일된 최대 난이도로 스폰처리
        {
            
        }
        
        return;
    }
        
    case ROUND_STATE::GAME_OVER:
    {
        
    }
        
    }
}

void CRoundHandler::SaveToLevelFile(FILE* _File)
{
    UINT RoundInfoCount = m_vecRoundInfo.size();
    fwrite(&RoundInfoCount, sizeof(UINT), 1, _File);
    
    for (const auto& RoundInfo : m_vecRoundInfo)
        fwrite(&RoundInfo, sizeof(RoundInfo), 1, _File);
}

void CRoundHandler::LoadFromLevelFile(FILE* _File)
{
    // TODO : 저장하고 주석 풀기
    
    /*UINT RoundInfoCount{};
    fread(&RoundInfoCount, sizeof(UINT), 1, _File);
    
    m_vecRoundInfo.resize(RoundInfoCount);
    for (auto& RoundInfo : m_vecRoundInfo)
        fread(&RoundInfo, sizeof(RoundInfo), 1, _File);*/
}
