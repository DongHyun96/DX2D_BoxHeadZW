#pragma once

class CGameLogManager : public CScript
{
private:

    vector<GameObjectRefHolder>     m_vecGameLogs{};
    vector<float>                   m_vecGameLogEachYPositions{};   // GameLog 각 위치의 초기 Y Position 값
    vector<int>                     m_vecLogSequence{};         // 현재 Log들의 순서 (차례로 밑에서부터 위로)

private:
    
    map<class CText*, float>    m_LogLifeTimers{}; // Log Spawn된 이후, FadeOut 처리되기 이전까지의 수명처리 담당
    set<CText*>                 m_FadeOutLogs{}; // FadeOut 처리시킬 로그들
    
public:
    
    CGameLogManager();
    CGameLogManager(const CGameLogManager& _Origin);
    virtual ~CGameLogManager() override;

    CLONE(CGameLogManager);
    
public:

    virtual void Init() override;
    virtual void AfterLevelGameObjectGuidTableInit() override;
    virtual void Tick() override;

public:
    
    /// <summary>
    /// GameLog 추가
    /// </summary>
    void AddGameLog(const wstring& _Log);
    
private:

    /// <summary>
    /// Fade Out Start 처리된 Log들 FadeOut 처리 
    /// </summary>
    void HandleLogFadeOut();

    /// <summary>
    /// <para> TextBlockLifeTimers에 들어온 log Text 메시지들의 수명 관리 처리 </para>
    /// <para> 수명이 다 된 Log의 경우, FadeOut 처리까지 담당 후 TextBlockLifeTimers 맵에서 제거 </para>
    /// </summary>
    void HandleLogLifeTimers();
    
    /// <summary>
    /// Queue 방식으로 처리되는 로그들에 대한 Effect Handling
    /// </summary>
    void HandleLogQueuePositionsAndDefaultAlpha();

    /// <summary>
    /// Log 메시지가 사라지기까지 걸리는 시간 지정 (이전에 지정된 시간이 있었거나 사라지고 있거나, 사라졌었다면 다시금 수명 재조정 
    /// </summary>
    void ApplyNewLifeTimerToLog(CText* _Log, float _TotalLifeTime);
    
public:
    void SaveToLevelFile(FILE* _File) override;
    void LoadFromLevelFile(FILE* _File) override;
};
