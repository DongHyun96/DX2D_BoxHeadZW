#include "pch.h"
#include "CRoundHandler.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/CharacterScript/EnemyScript/EnemySpawnHandler/CEnemySpawnHandler.h"
#include "Source/Scripts/CharacterScript/PlayerScript/CPlayerScript.h"
#include "Source/Scripts/UIScript/InGameUIManager/CIngameUIManager.h"

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
    AddScriptParam(SCRIPT_PARAM::ROUND_INFO_VECTOR, &m_vecRoundInfo, L"Round Info Settings");
    AddScriptParam(SCRIPT_PARAM::SOUND, &m_RoundStartSound, L"RoundStart Sound");
}

void CRoundHandler::Begin()
{
}

void CRoundHandler::AfterLevelBegin()
{
    // Round 대기 or Start
    GM->GetIngameUIManager()->GetRoundIndicatorsRef().OnRoundWaitStart();
}

void CRoundHandler::Tick()
{
    if (!GM->GetIsGameStart()) return;
    HandleTransition();
    
    switch (m_RoundState)
    {
    case ROUND_STATE::GAME_OVER: DebugUtil::SetPermanentDebugLog("RoundState", "RoundState : GAMEOVER", DEF_COLOR_ORANGE);
        break;
    }
}

void CRoundHandler::HandleTransition()
{
    if (GM->GetPlayerObject()->GetScriptComponent<CPlayerScript>()->GetMainState() == PLAYER_MAINSTATE::DIE) return;
    
    switch (m_RoundState)
    {
    case ROUND_STATE::WAIT:
    {
        m_RoundWaitTimer -= DT;
        GM->GetIngameUIManager()->GetRoundIndicatorsRef().OnRoundWaiting(m_RoundWaitTimer);
        
        if (m_RoundWaitTimer > 0.f) return;
        
        m_RoundWaitTimer = s_RoundWaitTime;
        SetRoundState(ROUND_STATE::ROUND_GOING);
        return;
    }
    case ROUND_STATE::ROUND_GOING:
    {
        m_CurrentRoundTimer += DT;
        
        GM->GetIngameUIManager()->GetRoundIndicatorsRef().OnRounding();
        
        if (!m_ThisRoundAdditionalSpawnLeft.empty())
        {
            // 주기적으로 Spawn 처리

            if (m_CurrentRoundTimer < m_CurrentRoundInfo->AdditionalSpawnStartTime) return;


            m_AdditionalSpawnTimer += DT;
            
            // 적 추가 시간 지남 (랜덤하게 흩뿌림)
            if (m_AdditionalSpawnTimer > m_AdditionalSpawnInterval)
            {
                m_AdditionalSpawnTimer -= m_AdditionalSpawnInterval;
                m_AdditionalSpawnInterval = GetRandom(0.5f, 2.f);

                for (auto it = m_ThisRoundAdditionalSpawnLeft.begin(); it != m_ThisRoundAdditionalSpawnLeft.end();)
                {
                    // 0 ~ 남은 SpawnLeft Random 수만큼 이번 Interval에 Spawn 처리
                    const int spawnCount = GetRandom<int>(0, it->second);
                    for (int i = 0; i < spawnCount; ++i)
                        GM->GetEnemySpawnHandler()->SpawnEnemyOnRandomCell(it->first);
                    
                    it->second -= spawnCount;
                    
                    // 남은 Additional 스폰 수가 0 이하로 떨어졌으면 it 삭제
                    if (it->second <= 0) it = m_ThisRoundAdditionalSpawnLeft.erase(it);
                    else ++it;
                }
            }
        }
        else // 만약 추가하려는 적이 모두 다 소진되었고, 맵에 돌아다니는 좀비가 없으면 다음 라운드 진행
        {
            // Additional 모두 소진은 되었지만, 아직 살아있는 Enemy가 있는 중
            if (GM->GetEnemySpawnHandler()->GetAliveEnemyCount() > 0)
            {
                m_AllDieCheckTimer = 0.f;
                return;
            }
            
            // Additional 소진 & 살아있는 Enemy 없음 (Mummy 때문에 2초 정도 기다려보고 다음 Wait 상태로 진행)
            static const float ALL_DIE_CHECK_TIME = 2.f;
            m_AllDieCheckTimer += DT;
            
            if (m_AllDieCheckTimer < ALL_DIE_CHECK_TIME) return;

            // 모두 죽은 것 확인
            
            m_AllDieCheckTimer  = 0.f;
            m_CurrentRoundTimer = 0.f;
            SetRoundState(ROUND_STATE::WAIT);
        }
        
        
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
        m_RoundWaitTimer = s_RoundWaitTime;
        GM->GetIngameUIManager()->GetRoundIndicatorsRef().OnRoundWaitStart();
        return;
    }
        
    case ROUND_STATE::ROUND_GOING:
    {
        ++m_CurrentRoundIdx;
        m_CurrentRoundTimer = 0.f;
        
        // 첫 Spawn 처리 (RoundInfo가 있다면)
        // RoundInfo가 넘어가면 제일 어려운 단계 계속해서 수행
        const int RoundIndex = m_CurrentRoundIdx < m_vecRoundInfo.size() ? m_CurrentRoundIdx : m_vecRoundInfo.size() - 1; 
        
        RoundInfo& rInfo = m_vecRoundInfo[RoundIndex];

        for (const pair<const ENEMY_TYPE, pair<UINT, UINT>>& FirstSpawnCountMinMaxPair : rInfo.EachEnemyFirstSpawnCountMinMax)
        {
            const UINT SpawnCount = GetRandom(FirstSpawnCountMinMaxPair.second.first, FirstSpawnCountMinMaxPair.second.second);
            
            for (int i = 0; i < SpawnCount; ++i)
            {
                const UINT SpawnLocation = GetRandom(static_cast<UINT>(FIRST_SPAWN_LOC1), static_cast<UINT>(FIRST_SPAWN_LOC_END - 1));
                GM->GetEnemySpawnHandler()->SpawnEnemyOnFirstSpawnArea(FirstSpawnCountMinMaxPair.first, static_cast<FIRST_SPAWN_LOC>(SpawnLocation));
            }
        }

        // m_ThisRoundAdditionalSpawnLeft 초기화
        m_ThisRoundAdditionalSpawnLeft.clear();
        for (const pair<const ENEMY_TYPE, pair<UINT, UINT>>& AdditionalSpawnCountPair : rInfo.EachEnemyAdditionalSpawnCountMinMax)
            m_ThisRoundAdditionalSpawnLeft[AdditionalSpawnCountPair.first] = GetRandom(AdditionalSpawnCountPair.second.first, AdditionalSpawnCountPair.second.second);
        
        if (m_RoundStartSound) m_RoundStartSound->Play(1, 0.5f, false);

        m_CurrentRoundInfo        = &m_vecRoundInfo[RoundIndex];
        m_AdditionalSpawnInterval = GetRandom(1.f, 3.f);
        m_AdditionalSpawnTimer    = 0.f;
        m_AllDieCheckTimer        = 0.f;
        
        GM->GetIngameUIManager()->GetRoundIndicatorsRef().OnRoundStart(m_CurrentRoundIdx + 1);
        
        return;
    }
        
    case ROUND_STATE::GAME_OVER:
    {
        GM->GetIngameUIManager()->GetRoundIndicatorsRef().OnGameOver();
    }
        
    }
}

void CRoundHandler::SaveToLevelFile(FILE* _File)
{
    UINT RoundInfoCount = m_vecRoundInfo.size();
    fwrite(&RoundInfoCount, sizeof(UINT), 1, _File);
    
    for (const auto& info : m_vecRoundInfo)
    {
        // First Spawn Settings
        UINT FirstSpawnCount = info.EachEnemyFirstSpawnCountMinMax.size();
        fwrite(&FirstSpawnCount, sizeof(UINT), 1, _File);
        for (const auto& [type, counts] : info.EachEnemyFirstSpawnCountMinMax)
        {
            // Enum 캐스팅 및 pair 분할 저장
            UINT enumValue = static_cast<UINT>(type);
            fwrite(&enumValue, sizeof(UINT), 1, _File);
            fwrite(&counts.first, sizeof(UINT), 1, _File);
            fwrite(&counts.second, sizeof(UINT), 1, _File);
        }

        // Additional Spawn Settings
        fwrite(&info.AdditionalSpawnStartTime, sizeof(float), 1, _File);
        
        UINT AdditionalSpawnCount = info.EachEnemyAdditionalSpawnCountMinMax.size();
        fwrite(&AdditionalSpawnCount, sizeof(UINT), 1, _File);
        for (const auto& [type, counts] : info.EachEnemyAdditionalSpawnCountMinMax)
        {
            UINT enumValue = static_cast<UINT>(type);
            fwrite(&enumValue, sizeof(UINT), 1, _File);
            fwrite(&counts.first, sizeof(UINT), 1, _File);
            fwrite(&counts.second, sizeof(UINT), 1, _File);
        }
    }
    
    SaveAssetRef(_File, m_RoundStartSound.Get());
}

void CRoundHandler::LoadFromLevelFile(FILE* _File)
{
    UINT RoundInfoCount = 0;
    fread(&RoundInfoCount, sizeof(UINT), 1, _File);
    
    m_vecRoundInfo.clear();
    m_vecRoundInfo.resize(RoundInfoCount);

    for (auto& info : m_vecRoundInfo)
    {
        // First Spawn Settings
        UINT FirstSpawnCount = 0;
        fread(&FirstSpawnCount, sizeof(UINT), 1, _File);
        info.EachEnemyFirstSpawnCountMinMax.clear();
        for (UINT i = 0; i < FirstSpawnCount; ++i)
        {
            UINT enumValue = 0;
            UINT countMin = 0, countMax = 0;
            
            fread(&enumValue, sizeof(UINT), 1, _File);
            fread(&countMin, sizeof(UINT), 1, _File);
            fread(&countMax, sizeof(UINT), 1, _File);
            
            info.EachEnemyFirstSpawnCountMinMax[static_cast<ENEMY_TYPE>(enumValue)] = {countMin, countMax};
        }

        // Additional Spawn Settings
        fread(&info.AdditionalSpawnStartTime, sizeof(float), 1, _File);
        
        UINT AdditionalSpawnCount = 0;
        fread(&AdditionalSpawnCount, sizeof(UINT), 1, _File);
        info.EachEnemyAdditionalSpawnCountMinMax.clear();
        for (UINT i = 0; i < AdditionalSpawnCount; ++i)
        {
            UINT enumValue = 0;
            UINT countMin = 0, countMax = 0;
            
            fread(&enumValue, sizeof(UINT), 1, _File);
            fread(&countMin, sizeof(UINT), 1, _File);
            fread(&countMax, sizeof(UINT), 1, _File);
            
            info.EachEnemyAdditionalSpawnCountMinMax[static_cast<ENEMY_TYPE>(enumValue)] = {countMin, countMax};
        }
    }
    
    m_RoundStartSound = LoadAssetRef<ASound>(_File);
    
}