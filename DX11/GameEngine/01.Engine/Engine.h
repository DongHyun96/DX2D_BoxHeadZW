#pragma once
#include "framework.h"


// 디자인 패턴 -		프로그래밍을 할때 발생하는 다양한 설계 패턴

// iterator 패턴 -	자료구조 컨테이너를 설계할 때, 내부동작 원리는 감추고,
//					저장된 데이터를 쉽게 접근할 수 있게 하는 iterator 클래스를 같이 제공한다.

// 싱글턴 패턴 -		자료형(클래스)으로 생성시킬 수 있는 개체의 개수를 1개로 제한 

// Singleton



/// <summary>
/// 게임 프로그램의 최상위 관리자
/// </summary>
class Engine : public Singleton<Engine>
{
	SINGLE(Engine);

private:

	HINSTANCE	m_hInst{};
	HWND		m_hWnd{};
	Vec2		m_Resolution{};
	
private:
	
	FMOD::System*	m_FMODSystem{};	// FMOD 관리자
	
private:
	
	bool m_EditorMode{}; // Editor 모드인지 확인
	
public:
	HRESULT Init(HINSTANCE _hInst, UINT _Width, UINT _Height, bool _EditorMode);
	HRESULT Progress();

public:
	
	HINSTANCE GetHInstance() const { return m_hInst; }
	HWND GetMainWndHwnd() const { return m_hWnd; }
	FMOD::System* GetFMODSystem() const { return m_FMODSystem; }
	Vec2 GetResolution() const { return m_Resolution; }
	
	bool IsEditorMode() const { return m_EditorMode; }
	
};

// FMOD 관리자 매크로
#define FMOD_SYSTEM Engine::GetInst()->GetFMODSystem()
