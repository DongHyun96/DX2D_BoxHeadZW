#pragma once

/// <summary>
/// GameObject Reference 저장 및 불러오기가 필요한 변수에 대해서, 이 클래스를 사용할 것
/// </summary>
class GameObjectRefHolder
{
private:
	
	GameObject* m_GameObject{}; // 실질적인 원본 reference 객체
	GUID		m_RefGUID{};	// Reference GameObject의 GUID
	
	function<void(GameObject*)> m_DelegateOnGameObjectDestroyed{}; // 래퍼런스로 들고 있었던 게임오브젝트가 Delete되었을 때, 이 GameObjectRefHolder를 사용중이었던 객체에게 알림용 Delegate
	
public:

	GameObjectRefHolder();
	~GameObjectRefHolder();
	GameObjectRefHolder(const GameObjectRefHolder& _Origin);
	GameObjectRefHolder(GameObjectRefHolder&& _Origin) noexcept;
	
	GameObjectRefHolder& operator=(const GameObjectRefHolder& _Other);
	GameObjectRefHolder& operator=(GameObjectRefHolder&& _Other) noexcept;
	
	GameObjectRefHolder* Clone() const { return new GameObjectRefHolder(*this);  }
	
public:
	
	void SetGameObject(GameObject* _GameObject);
	
	GameObject* GetGameObject() const { return m_GameObject; }
	GUID GetRefGUID() const { return m_RefGUID; }

	bool operator==(const GameObjectRefHolder& _Other) const
	{
		return _Other.m_RefGUID == m_RefGUID;
	}

public:
	
	void SetReferenceObjDestroyDelegate(const function<void(GameObject*)>& _Func) { m_DelegateOnGameObjectDestroyed = _Func; }
	
private:
	
	/// <summary>
	/// 래퍼런스로 들고 있었던 GameObject가 Destroy되었을 때, Callback 받는 함수
	/// 래퍼런스 연결을 끊고 nullptr로 처리 & 추가로 해당 Reference를 들고 있었던 다른 객체에게 Destroy되었다고 알림(만약 Destroy 관련 구독 처리가 되어 있다면)
	/// 주의 : Client 요청 처리로 GameObject::Destroy() 호출이 들어간 경우에만 Delegate 호출 처리됨
	/// Level 삭제 시 Destroy 요청 없이 GO 삭제 처리되는 경우에, m_DelegateOnGameObjectDestroyed 델리게이트 호출에서 의도치 않은 상황이 발생해서 이렇게 함 
	/// </summary>
	void OnGameObjectDestroyed();
	
	/// <summary>
	/// 래퍼런스로 들고 있었던 GameObject의 소멸자에서 Delegate 호출 처리
	/// GO 래퍼런스 포인터의 DanglingPointer 문제를 막기 위함
	/// </summary>
	void OnGameObjectDelete();
	
public:
	
	/// <summary>
	/// GameObject를 저장하는 것이 아닌, 원본 Reference의 GUID를 저장한다 (만약 원본 객체가 없었다면, 없는 객체의 Null GUID가 저장됨) 
	/// </summary>
	void SaveToLevelFile(FILE* _File);

	/// <summary>
	/// 이 Load의 경우, GUID Load 처리까지만 담당한다 
	/// </summary>
	void LoadFromLevelFile(FILE* _File);

	/// <summary>
	/// 실질적인 오브젝트 레퍼런스 레벨에 배치된 오브젝트와 연결 처리
	/// </summary>
	void LinkReferenceToGameObject(const Ptr<ALevel>& _Level);
	
};


