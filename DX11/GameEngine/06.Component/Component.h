#pragma once

#include "GameEngine/04.Asset/Entity.h"

#define GET_OTHER_COMPONENT(COM_NAME) class C##COM_NAME* COM_NAME() const

class GameObject;

class Component : public Entity
{

	friend class GameObject;

private:

	const COMPONENT_TYPE m_Type; // 컴포넌트 타입

	/*컴포넌트를 소유한 게임오브젝트, Ptr를 사용하지 않은 이유는 순환참조가 일어나기 때문이다
	자신의 소유주를 가리키는 멤버변수 포인터는 raw pointer를 사용*/
	GameObject* m_Owner{};

public:
	
	Component(COMPONENT_TYPE _Type);
	Component(const Component& _Origin);	
	
	virtual ~Component() override;

public:
	
	virtual Component* Clone() const = 0;
	
	virtual void SaveToLevelFile(FILE* _File) = 0;
	virtual void LoadFromLevelFile(FILE* _File) = 0;
	
public:

	COMPONENT_TYPE GetComponentType() const { return m_Type; }
	GameObject* GetOwner() const { return m_Owner; }
	
protected:

	void SetOwner(GameObject* _Owner) { m_Owner = _Owner; }

public:
	
    GET_OTHER_COMPONENT(Transform);
    GET_OTHER_COMPONENT(Camera);
    GET_OTHER_COMPONENT(ColliderRect);
    GET_OTHER_COMPONENT(ColliderCircle);
    GET_OTHER_COMPONENT(ColliderPoint);
    GET_OTHER_COMPONENT(Light2D);
	
    GET_OTHER_COMPONENT(MeshRender);
    GET_OTHER_COMPONENT(BillboardRender);
    GET_OTHER_COMPONENT(SpriteRender);
    GET_OTHER_COMPONENT(FlipbookRender);
	GET_OTHER_COMPONENT(TileRender);
	GET_OTHER_COMPONENT(ParticleRender);
	
	GET_OTHER_COMPONENT(PoolComponent);
	
	
	
	class CCollider2D* GetCollider2D() const;
	class CRenderComponent* GetRenderCom() const;

	
public:
	/// <summary>
	/// GameObject에 AddComponent 이후 Init 호출
	/// </summary>
	virtual void Init() {}

	/// <summary>
	/// <para> 현재 Level의 GUID 테이블이 초기화된 이후로 바로 호출됨(Begin 시점 이전) </para>
	/// <para> GameObject 레퍼런스 재연결 처리를 이 함수 시점에서 처리 -> 필요하면 override 할 것 </para>
	/// </summary>
	virtual void AfterLevelGameObjectGuidTableInit() {}
	
	virtual void Begin() {}
	
	/// <summary>
	/// Level 시작 시 Begin 처리가 모두 끝난 뒤 처리할 내용이 있다면 해당함수 override
	/// </summary>
	virtual void AfterLevelBegin() {}
	
	virtual void FinalTick() = 0;
};
