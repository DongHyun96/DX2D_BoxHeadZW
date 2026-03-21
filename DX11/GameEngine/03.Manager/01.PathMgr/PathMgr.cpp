#include "pch.h"
#include "PathMgr.h"

PathMgr::PathMgr()
    : m_ContentPath{}
{
}

PathMgr::~PathMgr()
{
}

void PathMgr::Init()
{
    InitPath(L"\\Content\\", m_ContentPath);
    InitPath(L"\\CollisionTable\\", m_CollisionTablePath);
}

void PathMgr::InitPath(const wstring& _TargetFolder, wchar_t* _InitingPath)
{
    // 실행파일이 있는 Bin 폴더 경로를 찾아낸다.
    // 디버깅 모드에서도 똑같이 동작하게 하기 위해서, 프로젝트 구성설정, 디버깅 탭에 작업 디렉터리를 실행파일 경로로 설정해준다.
    GetCurrentDirectory(PATH_CHAR_SIZE, _InitingPath);

    // exe파일 경로의 이전 경로명까지 문자열 줄이기 처리
    int Len = wcslen(_InitingPath);
    for (int i = Len - 1; i >= 0; --i)
    {
        if (_InitingPath[i] == L'\\')
        {
            _InitingPath[i] = '\0';
            break;
        }
    }

    // 폴더 경로 붙이기
    wcscat_s(_InitingPath, PATH_CHAR_SIZE, _TargetFolder.c_str()); 
}
