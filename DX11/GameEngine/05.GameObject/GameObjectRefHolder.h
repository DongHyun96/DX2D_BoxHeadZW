#pragma once

/// <summary>
/// GameObject Reference 저장 및 불러오기가 필요한 변수에 대해서, 이 클래스를 사용할 것
/// </summary>
class GameObjectRefHolder
{
private:
	
	GameObject* m_GameObject{}; // 실질적인 원본 reference 객체
	GUID		m_RefGUID{};	// Reference GameObject의 GUID
	
public:

	GameObjectRefHolder();
	~GameObjectRefHolder();

	/// <summary>
	/// <para> **기존에는 GUID만 복사받고 실질적인 래퍼런스 연결은 AfterGUIDTableInit 시점에 GORefHolder를 사용하는 쪽에서 LinkReferenceToGameObject를 통해 나머지 연결 처리를 해주었음** </para>
	/// <para> **위와 같이 GO 포인터 값을 직접 연결하지 않은 이유는, Level 복사 시 RefHolder와 GO 중 어떤 게 먼저 생성될지 모르기 때문 -> Level이 모두 로드된 이후** </para>
	/// <para> **Table이 명확히 잡히면, Table을 통해 생성된 GO를 찾아 래퍼런스를 연결하는 식 (지금도 그렇게 처리를 하고 있긴 함)** </para>
	/// <para> **위의 상황에서 Reliability 측면에서 GameObject 포인터(래퍼런스 연결) 까지는 복사받지 않고 있었음 (만약 GO 래퍼런스까지 복사받는다면, 해당 GO는 원본 Level(Stop상태에서의 Editing 전용 원본)에서의 GO)** </para>
	/// <para> **해당 GO는 원본 Level(Stop 상태에서의 Editing 전용 원본 SharedLevel)에서의 원본 GO라 원본 데이터를 건드릴 수 있는 일말의 Vulnerable한 상황이 있다고 판단** </para>
	/// <para> <개편 사항> </para>
	/// <para> **위의 복사처리는 레벨 복사 측면에서만 고려한 복사 처리 -> Editing 상황에서도 Duplicate 기능을 통해 RefHolder 자체도 복사처리가 되어야 하는 상황이 존재** </para>
	/// <para> **복사생성 시, GUID와 GO 래퍼런스 연결까지도 모두 연결 처리하도록 수정** </para>
	/// <para> **Level Play로 SharedLevel을 복사받을 때, GO복사처리 이후, 특정 동작을 하지 않고 바로 GUIDTable을 Init하고 RefHolder의 GO 래퍼런스 연결을 처리하고 있음** </para>
	/// <para> **Level 복사 과정에서 잠깐 원본 GO에 연결을 하지만, 새롭게 생성한 Level의 GUID Table이 Init된 이후, GO 래퍼런스 연결을 제대로 새로운 GO로 바로 처리를 하기 때문에 문제가 되지는 않음** </para>
	/// <para> **개편 사항에서의 단점은 Level 복사 과정에서, 어쨋든 원본 GO 래퍼런스 연결이 잠깐 일어남 -> but 문제가 되지는 않음 </para>
	/// </summary>
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

private:
	
	/// <summary>
	/// 래퍼런스로 들고 있었던 GameObject의 소멸자에서 또는 Destroy 되었을 때, Callback Delegate 호출 처리
	/// 래퍼런스 연결 끊기 처리
	/// </summary>
	void OnGameObjectDestroyOrDelete();
	
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


