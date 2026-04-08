#include "pch.h"

#include <ctime>

#include "GameEngine/01.Engine/Engine.h"
#include "GameEngine/03.Manager/01.PathMgr/PathMgr.h"
#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/03.Manager/09.EditorMgr/EditorMgr.h"
#include "GameEngine/04.Asset/Entity.h"

#include "ImGui/imgui_impl_win32.h"
#include "Source/AStar/AStarPathFinder.h"

#define MAX_LOADSTRING 100


HINSTANCE hInst{};

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);

INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// SAL 주석
// HInstance - 프로세스 ID
int APIENTRY wWinMain
(
    _In_     HINSTANCE hInstance,
    _In_opt_ HINSTANCE, 
    _In_     LPWSTR,    
    _In_     int    
)
{
    
    GetRandom(10, 20);
    GetRandom(25, 10);
    
    srand(time(nullptr));
    
    // 메모리 누수 감지
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    // _CrtSetBreakAlloc(138); // 누수 발생지점 중단점 걸어주는 기능

    hInst = hInstance;

    // Field node 먼저 초기화 처리를 해주어야 해서, GetInst() 한 번 호출 처리함
    AStarPathFinder::GetInst();
    
    // Engine 초기화, 최상위 관리자
    // ReleaseMode일 경우, EditorMode false 로 처리할 것
    if (FAILED(Engine::GetInst()->Init(hInstance, RESOL_X, RESOL_Y, true)))
        return 0;

    // LevelMgr::GetInst()->LoadTestLevel();
    ChangeLevel(L"Level\\NightSceneLevel.lv", true);
    // ChangeLevelState(LEVEL_STATE::PLAY);
    // CreateTestLevel();
    
    // 단축키 테이블 정보 핸들 받음
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DX11));
    
    MSG msg{};
    
    while (true)
    {
        // PM_REMOVE -> 확인한 메세지를 GetMessage함수처럼 WQ에서 제거하는 옵션
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT) break;
            
            // 단축키 관련 내용이면 TranslateAccelerator 함수에서 처리
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                // 단축키 관련된 이벤트가 아니면 TranslateMessage, DispatchMessage 함수를 이용해서 처리
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            
            continue;
        }
        // 메세지 큐에 메세지가 없었다.
        if (FAILED(Engine::GetInst()->Progress())) break; // Game 1프레임 실행
    }

    /*wstring ContentPath = CONTENT_PATH; ContentPath +=L"Level\\TestLevel.lv";
    LevelMgr::GetInst()->GetCurLevel()->Save(ContentPath);*/
    
    return (int) msg.wParam;
}


// extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

/// <summary>
/// 프로시저 함수
/// 윈도우에 발생한 사전(이벤트, 메세지)를 처리해주는 함수
/// DispatchMessage함수 내부에서 호출됨 
/// </summary>
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;
    
    switch (message)
    {
    // 1. Alt + 키 조합 시 발생하는 경고음 방지
    case WM_SYSCHAR: return DefWindowProc(hWnd, message, wParam, lParam);
        // 시스템 키(Alt 조합 등)가 눌렸을 때 문자 입력을 무시하고 바로 리턴x.
        // 이렇게 하면 DefWindowProc으로 메시지가 넘어가지 않아 윈도우 경고음 소리 x
        
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) return 0; // Alt 키 단독 입력 무시 
        
        // 윈도우 기본 처리기로 넘겨서 X버튼(SC_CLOSE), 창 이동 등이 작동하게 함
        return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_MOUSEWHEEL:
    {
        // -120이면 내림
        // 120이면 올림
        KeyMgr::GetInst()->SetMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam));
    }
        break;
    case WM_LBUTTONDOWN:
        // PostQuitMessage(0); // 윈도우 종료함수
        // 메세지 큐에 WM_QUIT이 들어감
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0); // 윈도우 종료함수
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
