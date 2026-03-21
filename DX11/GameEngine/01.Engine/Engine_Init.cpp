#include "pch.h"
#include "Engine.h"
#include "GameEngine/02.Device/Device.h"
#include "GameEngine/03.Manager/01.PathMgr/PathMgr.h"
#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/03.Manager/06.RenderMgr/RenderMgr.h"
#include "GameEngine/03.Manager/09.EditorMgr/EditorMgr.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HRESULT Engine::Init(HINSTANCE _hInst, UINT _Width, UINT _Height, bool _EditorMode)
{
    m_hInst         = _hInst;
    m_Resolution    = Vec2(_Width, _Height);
    m_EditorMode    = _EditorMode;
    
    // 생성시킬 윈도우(창) 옵션 설정
    WNDCLASSEXW wcex{};

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.lpszClassName  = L"MyGame";
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc; // 윈도우 프로시저 함수 포인터 등록
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = m_hInst;
    wcex.hIcon          = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_DX11));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = nullptr; // MAKEINTRESOURCEW(IDC_DX11);
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    RegisterClassExW(&wcex);
    
    // 등록한 윈도우 설정 데이터를 기반으로, 윈도우(창) 하나 생성
    // 커널 오브젝트 - OS가 관리하는 객체
    // 핸들         - 커널 오브젝트의 ID 개념
    // popUp 모드로 타이틀 창 제거 가능
    
    UINT Style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
    m_hWnd = CreateWindowW(L"MyGame", L"MyGame", /*WS_POPUP*/ /*WS_OVERLAPPEDWINDOW*/ Style, CW_USEDEFAULT, 0, CW_USEDEFAULT,
        0, nullptr, nullptr, m_hInst, nullptr);
    
    if (!m_hWnd) return E_FAIL;
    
    // 생성한 윈도우를 화면에 보이도록 설정
    ShowWindow(m_hWnd, true);
    UpdateWindow(m_hWnd);

    // 게임을 표시할 영역의 해상도가 원하는 크기가 되기 위해서
    // 실제 윈도우 크기는 몇이여야 하는지 계산
    RECT rt = {0, 0, static_cast<LONG>(m_Resolution.x), static_cast<LONG>(m_Resolution.y)};

    // 느낌표 두개는 bool type으로 맞추기 위함
    AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, !!GetMenu(m_hWnd));
    
    // 윈도우 크기 및 위치 변경
    SetWindowPos(m_hWnd, nullptr, 0, 0, rt.right - rt.left, rt.bottom - rt.top, 0);

    // 검은색으로 모든 픽셀 채우기
    /*HDC dc = GetDC(m_hWnd); // 입력으로 넣어준 윈도우에 그림을 그릴수 있게 해주는 도구
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0)); // 검은색 브러쉬 생성
    SelectObject(dc, hBrush); // dc에 검은색 brush를 전달
    Rectangle(dc, 0, 0, 1600, 900); // dc를 이용해서 윈도우에 검은색 사각형 그리기*/

    // GPU를 제어할 수 있는 DirectX11 함수를 사용하기 위한 매니저 생성
    // ID3D11Device, ID3D11DeviceContext
    if (FAILED(Device::GetInst()->Init(m_hWnd, m_Resolution)))
        return E_FAIL;

    PathMgr::GetInst()->Init(); // 실행파일과 같이 있는 Content 폴더의 경로를 찾아내기 위함
    TimeMgr::GetInst()->Init(); // 1프레임 동작하는데 걸리는 시간(DT) 계산
    KeyMgr::GetInst()->Init();  // 각각의 키의 상태를 계산 

    // 엔진 기본 에셋들 생성
    AssetMgr::GetInst()->Init();
    
    DebugUtil::AddDebugLog(L"[Engine Init] : AssetMgr INIT finished\n");
    
    // 레벨 매니저 초기화
    LevelMgr::GetInst()->Init();
    
    DebugUtil::AddDebugLog(L"[Engine Init] : LevelMgr INIT finished\n");
    
    // 렌더링 매니저 초기화
    RenderMgr::GetInst()->Init();
    
    DebugUtil::AddDebugLog(L"[Engine Init] : RenderMgr INIT finished\n");

    if (m_EditorMode)
    {
        EditorMgr::GetInst()->Init(); // Editor 매니저 초기화
        DebugUtil::AddDebugLog(L"[Engine Init] : EditorMgr INIT finished\n");
    }

    return S_OK; 
}
