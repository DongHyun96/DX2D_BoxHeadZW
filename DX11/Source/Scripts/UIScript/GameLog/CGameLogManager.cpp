#include "pch.h"
#include "CGameLogManager.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/05.GameObject/GameObjectRefHolder.h"
#include "Source/ScriptMgr.h"
#include "Source/Scripts/UIScript/CText.h"

CGameLogManager::CGameLogManager()
    : CScript(SCRIPT_TYPE::GAMELOGMANAGER)
{
}

CGameLogManager::CGameLogManager(const CGameLogManager& _Origin)
    : CScript(_Origin)
      , m_vecGameLogs(_Origin.m_vecGameLogs)
// 나머지 멤버변수는 default 값 (AfterLevelGameObjectGuidTableInit에서 초기화 처리)
{
}

CGameLogManager::~CGameLogManager()
{
}

void CGameLogManager::Init()
{
    CScript::Init();

    // AddScriptParam(SCRIPT_PARAM::GAME_OBJ_REF_HOLDER, &(m_vecGameLogs[0]), L"GameRefHolderTest", false);
    AddScriptParam(SCRIPT_PARAM::VEC_GAME_OBJ_REF_HOLDER, &m_vecGameLogs, L"VecGameLogs", false);
}

void CGameLogManager::AfterLevelGameObjectGuidTableInit()
{
    for (int i = 0; i < m_vecGameLogs.size(); ++i)
    {
        m_vecGameLogs[i].LinkReferenceToGameObject(LevelMgr::GetInst()->GetCurLevel());

        GameObject* GameLogObj = m_vecGameLogs[i].GetGameObject();
        if (!GameLogObj)
        {
            DebugUtil::AddDebugLog(
                "[CGameLogManager::AfterLevelGameObjectGuidTableInit] : GameLogObj Reference linking failed");
            continue;
        }

        m_vecGameLogEachYPositions.push_back(GameLogObj->Transform()->GetRelativePosY());
        m_vecLogSequence.push_back(i);
    }
}

void CGameLogManager::Tick()
{
    HandleLogFadeOut();
    HandleLogLifeTimers();
    HandleLogQueuePositionsAndDefaultAlpha();
}

void CGameLogManager::AddGameLog(const wstring& _Log)
{
    // 새로운 첫 번째 로그로 맨 뒤 로그 이동시키기
    const int TargetIndex = m_vecLogSequence.back();
    m_vecLogSequence.erase(m_vecLogSequence.end() - 1);
    m_vecLogSequence.insert(m_vecLogSequence.begin(), TargetIndex);

    GameObject* TargetTextObj = m_vecGameLogs[TargetIndex].GetGameObject();
    CText* TargetText         = TargetTextObj->GetScriptComponent<CText>().Get();
    
    // 내용 setting하기
    TargetText->SetText(_Log);
    
    // Render Alpha값 기본 값으로 지정
    TargetText->SetAlpha(1.f);
    
    // 초기 위치 지정
    TargetTextObj->Transform()->SetRelativePosY(m_vecGameLogEachYPositions[0]);    
    
    // Maximum 3초의 Log lifeTime 처리
    ApplyNewLifeTimerToLog(TargetText, 3.f);
}

void CGameLogManager::HandleLogFadeOut()
{
    for (set<CText*>::iterator it = m_FadeOutLogs.begin(); it != m_FadeOutLogs.end();)
    {
        CText* LogText           = *it;
        const float CurrentAlpha = LogText->GetAlpha();

        if (CurrentAlpha < 0.05f) // Alpha가 충분히 작아서 Destination에 도달했다고 판단
        {
            LogText->SetAlpha(0.f);
            it = m_FadeOutLogs.erase(it);
            continue;
        }

        // Lerp Alpha color to 0.f
        float DestAlpha = Lerp(CurrentAlpha, 0.f, DT * 10.f);
        LogText->SetAlpha(DestAlpha);

        ++it;
    }
}

void CGameLogManager::HandleLogLifeTimers()
{
    for (map<CText*, float>::iterator it = m_LogLifeTimers.begin(); it != m_LogLifeTimers.end();)
    {
        pair<CText* const, float>& TextLifeTimePair = *it;
        TextLifeTimePair.second                     -= DT;

        // 수명이 다한 Log -> FadeOut 처리를 위해 FadeOutLogs에 넣기
        // & LifeTimers 더 이상 잴 필요 없기 때문에 제거
        if (TextLifeTimePair.second <= 0.f)
        {
            m_FadeOutLogs.insert(TextLifeTimePair.first);
            it = m_LogLifeTimers.erase(it);
        }
        else ++it;
    }
}

void CGameLogManager::HandleLogQueuePositionsAndDefaultAlpha()
{
    for (int i = 1; i < m_vecLogSequence.size(); ++i)
    {
        const int Index           = m_vecLogSequence[i];
        CTransform* TextTransform = m_vecGameLogs[Index].GetGameObject()->Transform().Get();

        const float CurrentPosY = TextTransform->GetRelativePosY();
        const float DestPosY    = Lerp(CurrentPosY, m_vecGameLogEachYPositions[i], 20.f * DT);
        
        TextTransform->SetRelativePosY(DestPosY);
    }
    
    // 마지막에서 두 번 째 -> 로그는 보이는데, FadeOut 중이 아니라면 FadeOut 처리
    const int LastPrevIndex          = m_vecLogSequence[m_vecLogSequence.size() - 2];
    GameObjectRefHolder& LastPrevLog = m_vecGameLogs[LastPrevIndex];
    CText* LastPrevText              = LastPrevLog.GetGameObject()->GetScriptComponent<CText>().Get();
    
    if (LastPrevText->GetAlpha() > 0.f) // 아직 보이는 중이고
    {
        if (!m_FadeOutLogs.contains(LastPrevText)) // Start fade 처리가 안된 상황
        {
            m_LogLifeTimers.erase(LastPrevText);
            m_FadeOutLogs.insert(LastPrevText); // FadeOut 효과 처리 시작하기
        }
    }
}

void CGameLogManager::ApplyNewLifeTimerToLog(CText* _Log, float _TotalLifeTime)
{
    m_LogLifeTimers.insert(make_pair(_Log, _TotalLifeTime));
    m_FadeOutLogs.erase(_Log);
}

void CGameLogManager::SaveToLevelFile(FILE* _File)
{
    const int Count = m_vecGameLogs.size();
    fwrite(&Count, sizeof(int), 1, _File);

    for (GameObjectRefHolder& RefHolder : m_vecGameLogs)
        RefHolder.SaveToLevelFile(_File);
}

void CGameLogManager::LoadFromLevelFile(FILE* _File)
{
    int Count{};
    fread(&Count, sizeof(int), 1, _File);

    for (int i = 0; i < Count; ++i)
    {
        GameObjectRefHolder RefHolder{};
        RefHolder.LoadFromLevelFile(_File);
        m_vecGameLogs.push_back(move(RefHolder));
    }
}
