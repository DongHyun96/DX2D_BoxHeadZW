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
	
	GET_OTHER_COMPONENT(PoolComponent);
	
	
	class CCollider2D* GetCollider2D() const;

	
public:
	/// <summary>
	/// GameObject에 AddComponent 이후 Init 호출
	/// </summary>
	virtual void Init() {}
	
	virtual void Begin() {}
	virtual void FinalTick() = 0;
};
