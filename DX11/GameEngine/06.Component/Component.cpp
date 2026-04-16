#include "pch.h"
#include "Component.h"

#include "GameEngine/05.GameObject/GameObject.h"

#define GET_OTHER_COMPONENT_BODY(COM_NAME) C##COM_NAME* Component::COM_NAME() const { return GetOwner()->COM_NAME().Get(); }

Component::Component(COMPONENT_TYPE _Type)
	: m_Type(_Type)
{
}

Component::Component(const Component& _Origin)
	: Entity(_Origin)
	, m_Type(_Origin.m_Type)
	, m_Owner(nullptr)
{
}

Component::~Component()
{
}

GET_OTHER_COMPONENT_BODY(Transform)
GET_OTHER_COMPONENT_BODY(Camera)
GET_OTHER_COMPONENT_BODY(ColliderRect)
GET_OTHER_COMPONENT_BODY(ColliderCircle)
GET_OTHER_COMPONENT_BODY(ColliderPoint)
GET_OTHER_COMPONENT_BODY(Light2D)
	
GET_OTHER_COMPONENT_BODY(MeshRender)
GET_OTHER_COMPONENT_BODY(BillboardRender)
GET_OTHER_COMPONENT_BODY(SpriteRender)
GET_OTHER_COMPONENT_BODY(FlipbookRender)
GET_OTHER_COMPONENT_BODY(TileRender)
GET_OTHER_COMPONENT_BODY(ParticleRender)
GET_OTHER_COMPONENT_BODY(PoolComponent)

CCollider2D* Component::GetCollider2D() const
{
	return m_Owner->GetCollider2D().Get();
}

CRenderComponent* Component::GetRenderCom() const
{
	return m_Owner->GetRenderCom().Get();	
}


