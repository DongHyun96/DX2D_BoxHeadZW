#pragma once

class TimeMgr : public Singleton<TimeMgr>
{
    
    SINGLE(TimeMgr);
    
private:
    
    LARGE_INTEGER   m_Frequency{};
    LARGE_INTEGER   m_Prev{};
    LARGE_INTEGER   m_Current{};

    float           m_DeltaTime{}; // 1 프레임 간격 시간 (Engine DT)
    float           m_Time{}; // 게임이 켜진 이후로 흐른 시간(누적시간)
    float           m_TimeScale{1.f};
    float           m_GameUnscaledDeltaTime{}; // 게임 로직용 Unscaled DT (PLAY일 때 EngineDT)
    float           m_GameScaledDeltaTime{};   // 게임 로직용 TimeScale 적용 DT
    float           m_CurrentGameDeltaTime{};  // 현재 컨텍스트에서 DT가 반환할 값
    
    UINT            m_FPS{};
    UINT            m_FPSDisplay{};
    
    float           m_FPSAccumTime{};
    UINT            m_FPSFrameCount{};
    
public:
    
    void Init();
    void Tick();
    void Render();
    
public:

    float GetDeltaTime() const { return m_CurrentGameDeltaTime; }
    float GetTime() const { return g_Global.Time; }
    
    float GetEngineDT() const { return g_Global.EngineDT; }
    float GetEngineTime() const { return g_Global.EngineTime; }
    
    void SetTimeScale(float _Scale);
    float GetTimeScale() const { return m_TimeScale; }
    float GetScaledDeltaTime() const { return m_GameScaledDeltaTime; }
    float GetUnscaledDeltaTime() const { return m_GameUnscaledDeltaTime; }

    // GameObject Tick/FinalTick 컨텍스트에서만 사용할 것
    float PushGameDeltaTimeContext(bool _UseUnscaledDeltaTime);
    void PopGameDeltaTimeContext(float _PrevDeltaTime);

    /// <summary>
    /// 현재 시각 구하기(현실 시간)
    /// </summary>
    wstring GetLocalTimeWString() const;
    
};

#define DT      TimeMgr::GetInst()->GetDeltaTime()
#define TIME    TimeMgr::GetInst()->GetTime()

#define E_DT    TimeMgr::GetInst()->GetEngineDT()
#define E_TIME  TimeMgr::GetInst()->GetEngineTime()
