#include "pch.h"
#include "TimeMgr.h"

#include "GameEngine/01.Engine/Engine.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/03.Manager/10.FontMgr/FontMgr.h"
#include <algorithm>

#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/CharacterScript/CharacterStat/PlayerStat/CPlayerStat.h"

TimeMgr::TimeMgr()
    /*: m_Frequency{}
    , m_Prev{}
    , m_Current{}
    , m_DeltaTime(0.f)
    , m_Time(0.f)*/
{
}

TimeMgr::~TimeMgr()
{
    
}

void TimeMgr::Init()
{
    // 1초 동안 가능한 카운팅 횟수
    QueryPerformanceFrequency(&m_Frequency);

    // 컴퓨터 부팅 이후로 센 총 카운트 횟수
    // 초기 카운트 & prev 초기화
    QueryPerformanceCounter(&m_Current);
    QueryPerformanceCounter(&m_Prev);
}

void TimeMgr::Tick()
{
    QueryPerformanceCounter(&m_Current);

    // 이전과 현재 카운팅 차이를 Frequency로 나눠서 1프레임동안 진행한 시간값을 구하기
    m_DeltaTime = static_cast<float>(m_Current.QuadPart - m_Prev.QuadPart) / static_cast<float>(m_Frequency.QuadPart);

    // [Fix] 릴리즈 모드 및 프레임 드랍 시 DT 폭주 방지 (최대 0.1초로 제한)
    m_DeltaTime = min(m_DeltaTime, 0.1f);

    // Prev 카운팅을 다시 현재카운팅으로 맞추기
    m_Prev = m_Current;

    // 누적 시간 계산
    m_Time += m_DeltaTime;

    m_FPSAccumTime += m_DeltaTime;
    ++m_FPSFrameCount;

    const float safeDeltaTime = (m_DeltaTime > 0.000001f) ? m_DeltaTime : 0.000001f;
    m_FPS = static_cast<UINT>(1.f / safeDeltaTime);

    wchar_t buff[255];
    swprintf_s(buff, L"DT : %.3f ms | %u FPS", m_DeltaTime * 1000.f, m_FPS);
    SetWindowText(Engine::GetInst()->GetMainWndHwnd(), buff);
    // DebugUtil::SetPermanentDebugLog("FPS", "FPS : " + to_string(m_FPS), DEF_COLOR_GREEN);
    

    if (m_FPSAccumTime >= 1.f)
    {
        m_FPSAccumTime -= 1.f;
        m_FPSFrameCount = 0;
    }

    // Game Engine용 Time
    g_Global.EngineDT = m_DeltaTime;
    g_Global.EngineTime += m_DeltaTime;

    // Level 이 Pause 나 Stop 상태라면
    if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::PLAY)
    {
        m_GameUnscaledDeltaTime = 0.f;
        m_GameScaledDeltaTime = 0.f;
        m_CurrentGameDeltaTime = 0.f;

        g_Global.DeltaTime = 0.f;
        g_Global.Time = 0.f;
    }
    // Level 이 Play 상태
    else
    {
        m_GameUnscaledDeltaTime = m_DeltaTime;
        m_GameScaledDeltaTime = m_GameUnscaledDeltaTime * m_TimeScale;
        m_CurrentGameDeltaTime = m_GameScaledDeltaTime;

        // Game Content 용 Time (TimeScale 적용)
        g_Global.DeltaTime = m_CurrentGameDeltaTime;
        g_Global.Time += m_CurrentGameDeltaTime;
    }
    
}

void TimeMgr::SetTimeScale(float _Scale)
{
    m_TimeScale = max(_Scale, 0.f);
    m_GameScaledDeltaTime = m_GameUnscaledDeltaTime * m_TimeScale;

    // 즉시 반영
    // m_CurrentGameDeltaTime = m_GameScaledDeltaTime;
    // g_Global.DeltaTime = m_CurrentGameDeltaTime;
}

float TimeMgr::PushGameDeltaTimeContext(bool _UseUnscaledDeltaTime)
{
    const float prevDeltaTime = m_CurrentGameDeltaTime;

    m_CurrentGameDeltaTime = _UseUnscaledDeltaTime ? m_GameUnscaledDeltaTime : m_GameScaledDeltaTime;
    g_Global.DeltaTime = m_CurrentGameDeltaTime;

    return prevDeltaTime;
}

void TimeMgr::PopGameDeltaTimeContext(float _PrevDeltaTime)
{
    m_CurrentGameDeltaTime = _PrevDeltaTime;
    g_Global.DeltaTime = m_CurrentGameDeltaTime;
}

void TimeMgr::Render()
{
    // 이거 지금 1초 간 쌓인 FPS 출력 형태로 되어있음
    static bool Flag{};
    if (Flag)
    {
        wchar_t buff[255];
        swprintf_s(buff, L"%d FPS", m_FPS);
        FontMgr::GetInst()->DrawFont(buff, 10, 30, 24, FONT_RGBA(200, 20, 20, 255));
    }
    
    if (KEY_TAP(KEY::F1)) Flag = !Flag;

    if (GM->GetPlayerObject())
    {
        Ptr<CPlayerStat> pStat = GM->GetPlayerObject()->GetScriptComponent<CPlayerStat>();
        
        if (pStat->IsDebugInvincible()) FontMgr::GetInst()->DrawFont(L"Player Invincible", 10, 60, 24, FONT_RGBA(200, 20, 20, 255));
        if (KEY_TAP(KEY::SPACE)) pStat->ToggleDebugInvincible();
    }
}

wstring TimeMgr::GetLocalTimeWString() const
{
    time_t now = chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    tm local_tm{};
    localtime_s(&local_tm, &now);

    wchar_t buf[64]{};
    wcsftime(buf, 64, L"%Y-%m-%d %H:%M:%S", &local_tm);
    return buf;
}
