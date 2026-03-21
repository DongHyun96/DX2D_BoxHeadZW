#pragma once

class TimeMgr : public Singleton<TimeMgr>
{
    
    SINGLE(TimeMgr);
    
private:
    
    LARGE_INTEGER   m_Frequency{};
    LARGE_INTEGER   m_Prev{};
    LARGE_INTEGER   m_Current{};

    float           m_DeltaTime{}; // 1 프레임 간격 시간
    float           m_Time{}; // 게임이 켜진 이후로 흐른 시간(누적시간)
    
    UINT            m_FPS{};
    
public:
    
    void Init();
    void Tick();
    
public:

    float GetDeltaTime() const { return m_DeltaTime; }
    float GetTime() const { return g_Global.Time; }
    
    float GetEngineDT() const { return g_Global.EngineDT; }
    float GetEngineTime() const { return g_Global.EngineTime; }

    /// <summary>
    /// 현재 시각 구하기(현실 시간)
    /// </summary>
    wstring GetLocalTimeWString() const;
    
    
};

#define DT      TimeMgr::GetInst()->GetDeltaTime()
#define TIME    TimeMgr::GetInst()->GetTime()

#define E_DT    TimeMgr::GetInst()->GetEngineDT()
#define E_TIME  TimeMgr::GetInst()->GetEngineTime()