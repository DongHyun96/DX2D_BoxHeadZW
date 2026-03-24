#include "pch.h"
#include "TimeMgr.h"

#include "FontMgr.h"
#include "GameEngine/01.Engine/Engine.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"

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

    // Prev 카운팅을 다시 현재카운팅으로 맞추기
    m_Prev = m_Current;

    // 누적 시간 계산
    m_Time += m_DeltaTime;
    
    ++m_FPS;

    // 1초마다 if 수행
    if (1.f < m_Time)
    {
        wchar_t buff[255];
        swprintf_s(buff, L"DT : %f ms | %d FPS", m_DeltaTime, m_FPS);
        SetWindowText(Engine::GetInst()->GetMainWndHwnd(), buff);

        m_FPS = 0;
        m_Time -= 1.f;
    }

    // Game Engine용 Time
    g_Global.EngineDT = m_DeltaTime;
    g_Global.EngineTime += m_DeltaTime;

    // Level 이 Pause 나 Stop 상태라면
    if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::PLAY)
    {
        g_Global.DeltaTime  = m_DeltaTime = 0.f;
        g_Global.Time       = 0.f;
    }
    // Level 이 Play 상태
    else
    {
        // Game Content 용 Time
        g_Global.DeltaTime  = m_DeltaTime;
        g_Global.Time      += m_DeltaTime;
    }
    
}

void TimeMgr::Render()
{
    // 이거 지금 1초 간 쌓인 FPS 출력 형태로 되어있음
    /*wchar_t buff[255];
    swprintf_s(buff, L"DT : %f ms | %d FPS", m_DeltaTime, m_FPS);
    FontMgr::GetInst()->DrawFont(buff, 10, 30, 24, FONT_RGBA(200, 20, 20, 255));*/
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
