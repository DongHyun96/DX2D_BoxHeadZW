#include "pch.h"
#include "KeyMgr.h"

#include "GameEngine/01.Engine/Engine.h"
#include "GameEngine/02.Device/Device.h"
#include "GameEngine/03.Manager/06.RenderMgr/RenderMgr.h"

UINT g_KeyIndex[static_cast<UINT>(KEY::KEY_END)] = 
{
    'Q', 'W', 'E', 'R',
    
    'A', 'S', 'D', 'F',
    'Z', 'X', 'C', 'V',
    
    VK_LBUTTON,
    VK_RBUTTON,
    VK_MBUTTON,
    
    '1', '2', '3', '4', '5', '6', 
    
    VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN,
    
    VK_RETURN,
    VK_MENU,
    
    VK_CONTROL, VK_LSHIFT, VK_RSHIFT,
    
    VK_SPACE,
    VK_ESCAPE,
    VK_HOME, VK_END,
    
    VK_F1, VK_F2, VK_F3, VK_F4,
    VK_F5, VK_F6, VK_F7, VK_F8, VK_F9,
};

KeyMgr::KeyMgr()
{
}

KeyMgr::~KeyMgr()
{
}

void KeyMgr::Init()
{
    // 키 등록
    m_vecKeys.resize(static_cast<UINT>(KEY::KEY_END));
}

void KeyMgr::Tick()
{
    // GetFocus() : 현재 포커싱 중인 윈도우 핸들 반환
    // 현재 ImGui에 Focus가 잡혔을 때
    if (GetFocus() != Engine::GetInst()->GetMainWndHwnd() || !m_Active)
    {
        for (UINT i = 0; i < static_cast<UINT>(KEY::KEY_END); ++i)
        {
            GetAsyncKeyState(g_KeyIndex[i]); // 누적 입력처리 해소
            
            if (m_vecKeys[i].State == TAP || m_vecKeys[i].State == PRESSED)
                m_vecKeys[i].State = RELEASED;
            else m_vecKeys[i].State = NONE;

            m_vecKeys[i].Pressed = false;
        }
        
        return;
    }
    
    for (UINT i = 0; i < m_vecKeys.size(); i++)
    {
        // if (GetAsyncKeyState(g_KeyIndex[i] == 0x8000) 지금 시점에서 눌렸는지 확인 0x8000 (short 16비트의 맨 앞자리가 1일 때, 현 시점에 눌렸는지 판정)
        // 단점 : 게임 프레임이 떨어질수록 키 씹힘이 생길 수가 있다
        if (GetAsyncKeyState(g_KeyIndex[i]))
        {
            m_vecKeys[i].State = (m_vecKeys[i].Pressed) ? PRESSED : TAP; 
            m_vecKeys[i].Pressed = true;
        }
        else // 지금 안눌려있다
        {
            m_vecKeys[i].State = (m_vecKeys[i].Pressed) ? RELEASED : NONE;
            m_vecKeys[i].Pressed = false;
        }
    }
    
    // 마우스 좌표 계산
    POINT pt{};
    GetCursorPos(&pt); // 화면 기준 마우스 좌표
    ScreenToClient(Engine::GetInst()->GetMainWndHwnd(), &pt); // 우리 윈도우 기준으로 마우스좌표 변경
    // 좌상단(0, 0), 우하단(ResolWidth, ResolHeight) -> WinAPI 좌표로 찍힘

    m_MousePrevPos = m_MousePos;
    m_MousePos = Vec2(static_cast<float>(pt.x), static_cast<float>(pt.y));

    // 마우스 진행 방향
    m_MouseDir = m_MousePos - m_MousePrevPos;
    m_MouseDir.Normalize();
    
    if (m_WheelChanged) m_WheelChanged = false;
    else m_Wheel = 0;
}

Vec3 KeyMgr::GetMouseWorldPos() const
{
    const Ptr<CCamera> pPOVCam = RenderMgr::GetInst()->GetPOVCam();
    if (!pPOVCam) return Vec3::Zero;

    // 현재 프로젝트는 2D Orthographic 기준으로 마우스 월드 좌표를 사용한다.
    // if (pPOVCam->GetType() == PROJ_TYPE::ORTHOGRAPHIC) return Vec3::Zero;
            

    const Vec2 vRenderRes = Device::GetInst()->GetRenderResolution();
    if (vRenderRes.x <= 0.f || vRenderRes.y <= 0.f)
        return Vec3::Zero;

    // Client(0~해상도) -> NDC(-1~1)
    const float ndcX = (m_MousePos.x / vRenderRes.x) * 2.f - 1.f;
    const float ndcY = 1.f - (m_MousePos.y / vRenderRes.y) * 2.f;

    const float worldW = pPOVCam->GetWidth() * pPOVCam->GetOrthoScale();
    const float worldH = (pPOVCam->GetWidth() / pPOVCam->GetAspectRatio()) * pPOVCam->GetOrthoScale();

    Vec3 vWorld = pPOVCam->Transform()->GetWorldPos();
    vWorld += pPOVCam->Transform()->GetDir(DIR::RIGHT) * (ndcX * worldW * 0.5f);
    vWorld += pPOVCam->Transform()->GetDir(DIR::UP) * (ndcY * worldH * 0.5f);
    vWorld.z = 0.f;

    return vWorld;
}

Vec3 KeyMgr::GetMouseWorldPosByViewport(const Vec2& _LocalPos, const Vec2& _ViewportSize) const
{
    const Ptr<CCamera> pPOVCam = RenderMgr::GetInst()->GetPOVCam();
    if (!pPOVCam) return Vec3::Zero;

    if (_ViewportSize.x <= 0.f || _ViewportSize.y <= 0.f)
        return Vec3::Zero;

    const float ndcX = (_LocalPos.x / _ViewportSize.x) * 2.f - 1.f;
    const float ndcY = 1.f - (_LocalPos.y / _ViewportSize.y) * 2.f;

    const float worldW = pPOVCam->GetWidth() * pPOVCam->GetOrthoScale();
    const float worldH = (pPOVCam->GetWidth() / pPOVCam->GetAspectRatio()) * pPOVCam->GetOrthoScale();

    Vec3 vWorld = pPOVCam->Transform()->GetWorldPos();
    vWorld += pPOVCam->Transform()->GetDir(DIR::RIGHT) * (ndcX * worldW * 0.5f);
    vWorld += pPOVCam->Transform()->GetDir(DIR::UP) * (ndcY * worldH * 0.5f);
    vWorld.z = 0.f;

    return vWorld;
}
