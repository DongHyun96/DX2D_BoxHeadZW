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
	GameObjectRefHolder(const GameObjectRefHolder& _Origin);
	GameObjectRefHolder(GameObject* _Object);
	GameObjectRefHolder(GUID _Guid);
	~GameObjectRefHolder();
	
	CLONE(GameObjectRefHolder);
	

public:
	
	void SetGameObject(GameObject* _GameObject);
	
	GameObject* GetGameObject() const { return m_GameObject; }

	bool operator==(const GameObjectRefHolder& _Other) const
	{
		return _Other.m_RefGUID == m_RefGUID;
	}
	
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


