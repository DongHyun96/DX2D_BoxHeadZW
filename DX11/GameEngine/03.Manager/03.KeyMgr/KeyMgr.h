#pragma once

/// <summary>
/// Key_State를 감지할 key 종류들
/// </summary>
enum class KEY
{
	Q,
	W,
	E,
	R,
	A,
	S,
	D,
	F,
	Z,
	X,
	C,
	V,
	
	MLB, // 마우스 왼쪽 클릭
	MRB, // 마우스 오른쪽 클릭
	MMB,
	
	TILDE, NUM_1, NUM_2, NUM_3, NUM_4, NUM_5, NUM_6,
	NUM_7, NUM_8, NUM_9,
	
	LEFT,
	RIGHT,
	UP,
	DOWN,
	
	ENTER,
	ALT,
	CTRL,
	LSHIFT,
	RSHIFT,
	SPACE,
	ESC,
	
	HOME,
	END,

	F1, F2, F3, F4, F5,
	F6, F7, F8, F9,
	
	KEY_END
};

enum KEY_STATE
{
	NONE,
	TAP,
	PRESSED,
	RELEASED,
};

struct KeyInfo
{
	KEY_STATE	State{};   // 키의 현재 상태
	bool		Pressed{};		// 이전에 눌린적이 있는지 아닌지
};

class KeyMgr : public Singleton<KeyMgr>
{
	
	SINGLE(KeyMgr);

private:

	vector<KeyInfo> m_vecKeys{};	
	
	Vec2 m_MousePos{};		// 이번 프레임 마우스 좌표 (창 기준 좌표)
	Vec2 m_MousePrevPos{};	// 이전 프레임 마우스 좌표
	Vec2 m_MouseDir{};		// 이번 프레임 마우스 이동 방향

	bool m_WheelChanged{};
	int m_Wheel{}; // 1(WheelUp), 0, -1(WheelDown)
	
	bool m_Active{};
	
public:
	
	void Init();
	
public:

	void Tick();
	
public:
	
	KEY_STATE GetKeyState(KEY _Key) const { return m_vecKeys[static_cast<UINT>(_Key)].State; }
	
	GET(Vec2, MousePos);
	GET(Vec2, MousePrevPos);
	GET(Vec2, MouseDir);

public:
	
	/// <summary>
	/// 현재 마우스 Client 좌표를 인게임 월드 좌표(2D Orthographic)로 변환
	/// </summary>
	Vec3 GetMouseWorldPos() const;

	/// <summary>
	/// 지정한 뷰포트 사각형(로컬좌표 기준)으로 마우스 월드좌표 계산
	/// </summary>
	/// <param name="_LocalPos"> : 뷰포트 내부 로컬 좌표 </param>
	/// <param name="_ViewportSize"> : 뷰포트 크기 </param>
	/// <returns> : </returns>
	Vec3 GetMouseWorldPosByViewport(const Vec2& _LocalPos, const Vec2& _ViewportSize) const;
	
public:
	
	int GetMouseWheel() const { return m_Wheel; }
	void SetMouseWheel(int _Wheel)
	{
		m_WheelChanged = true;
		m_Wheel = _Wheel / abs(_Wheel);
	}
	
	GET_SET(bool, Active)
	
};

#define KEY_CHECK(key, state) (KeyMgr::GetInst()->GetKeyState(key) == state)

#define KEY_TAP(key)		KEY_CHECK(key, KEY_STATE::TAP)
#define KEY_PRESSED(key)	KEY_CHECK(key, KEY_STATE::PRESSED)
#define KEY_RELEASED(key)	KEY_CHECK(key, KEY_STATE::RELEASED)
#define KEY_NONE(key)		KEY_CHECK(key, KEY_STATE::NONE)
