#include "pch.h"
#include "GameObject.h"

#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/03.Manager/07.TaskMgr/TaskMgr.h"
#include "GameEngine/04.Asset/04.Level/ALevel.h"
#include "GameEngine/04.Asset/09.Prefab/APrefab.h"
#include "GameEngine/06.Component/01.Transform/CTransform.h"
#include "GameEngine/06.Component/03.Collider2D/CColliderCircle.h"
#include "GameEngine/06.Component/03.Collider2D/CColliderPoint.h"
#include "GameEngine/06.Component/03.Collider2D/CColliderRect.h"
#include "Source/ScriptMgr.h"


GameObject::GameObject()
	: m_Components{}
	, m_Parent(nullptr)
	, m_ObjectDestroyed(false)
	, m_LayerIdx(-1)
{
}

GameObject::GameObject(const GameObject& _Origin)
	: Entity(_Origin)
	, m_LayerIdx(_Origin.m_LayerIdx) // 원본의 LayerIdx를 따르도록 처리
	, m_IsActive(_Origin.m_IsActive)
	, m_IsVisible(_Origin.m_IsVisible)
{
	// 원본 오브젝트와 동일한 세팅의 컴포넌트를 복사해서 나한테 넣어준다.
	for (UINT i = 0; i < static_cast<UINT>(COMPONENT_TYPE::END); ++i)
	{
		if (!_Origin.m_Components[i]) continue;
		AddComponent(_Origin.m_Components[i]->Clone());
	}

	// 원본 오브젝트와 동일한 스크립트를 복사해서 나한테 넣어준다.
	for (const Ptr<CScript>& Script : _Origin.m_vecScripts)
		AddComponent(Script->Clone());

	// 원본 오브젝트가 보유한 자식 오브젝트를 복사해서 나한테 붙여준다.
	for (const Ptr<GameObject>& Child : _Origin.m_vecChild)
		AddChild(Child->Clone());
}

GameObject::~GameObject()
{
}

void GameObject::Begin()
{
	for (const Ptr<CScript>& script : m_vecScripts)
		script->Begin();	
	
	for (const Ptr<Component>& component : m_Components)
		if (component) component->Begin();
	
	for (const Ptr<GameObject>& child : m_vecChild)
		child->Begin();
}

void GameObject::AfterLevelBegin()
{
	for (const Ptr<CScript>& script : m_vecScripts)
		script->AfterLevelBegin();
}

void GameObject::Tick()
{
	for (const Ptr<CScript>& script : m_vecScripts)
		script->Tick();
	
	for (const Ptr<GameObject>& child : m_vecChild)
		child->Tick();
}

void GameObject::FinalTick()
{
	for (const Ptr<Component>& component : m_Components)
		if (component) component->FinalTick();
	
	/*for (const Ptr<CScript>& script : m_vecScripts)
		if (script) script->FinalTick();*/

	RegisterLayer(); // 자신이 소속된 Layer에 자신을 알림

	vector<Ptr<GameObject>>::iterator iter = m_vecChild.begin();

	while (iter != m_vecChild.end())
	{
		Ptr<GameObject>& child = *iter;
		child->FinalTick();
		
		if (child->IsObjectDestroyed()) iter = m_vecChild.erase(iter);
		else ++iter;
	}
}

void GameObject::FinalTick_Editor()
{
	if (!m_IsActive) return;
	
	for (const Ptr<Component>& component : m_Components)
		if (component) component->FinalTick();

	vector<Ptr<GameObject>>::iterator iter = m_vecChild.begin();

	while (iter != m_vecChild.end())
	{
		Ptr<GameObject>& child = *iter;
		child->FinalTick();
		
		if (child->IsObjectDestroyed()) iter = m_vecChild.erase(iter);
		else ++iter;
	}
}

void GameObject::Render()
{
	if (!m_IsActive || !m_IsVisible) return;
	
	if (m_RenderCom)
	{
		// Transform 위치 정보를 constant buffer로 binding 처리한 뒤, MeshRenderer component가 그리기 처리
		Transform()->Binding();
		m_RenderCom->Render();	
	}
	
	for (const Ptr<GameObject>& child : m_vecChild)
		child->Render();
}

bool GameObject::RemoveComponent(COMPONENT_TYPE _Type)
{
	UINT TypeToIdx = static_cast<UINT>(_Type);

	// END 또는 Script일 경우
	if (TypeToIdx >= static_cast<UINT>(COMPONENT_TYPE::END)) return false;

	// 제거 대상이 없음
	if (!m_Components[TypeToIdx]) return false;

	// Rendering Component 였을 경우
	if (dynamic_cast<CRenderComponent*>(m_Components[TypeToIdx].Get()))
		m_RenderCom = nullptr;
	else if (dynamic_cast<CCollider2D*>(m_Components[TypeToIdx].Get())) // Collider Component 였을 경우
		m_Collider2D = nullptr;
	
	// 컴포넌트 제거
	m_Components[TypeToIdx] = nullptr;
	
	return true;
}

bool GameObject::RemoveScript(const Ptr<CScript>& _TargetScript)
{
	if (!_TargetScript) return false;
	
	for (auto iter = m_vecScripts.begin(); iter != m_vecScripts.end(); ++iter)
	{
		if (*iter == _TargetScript)
		{
			m_vecScripts.erase(iter);
			return true;
		}
	}
	
	return false;
}

bool GameObject::HasScript(SCRIPT_TYPE _ScriptType) const
{
	for (const Ptr<CScript>& script : m_vecScripts)
		if (script->GetScriptType() == _ScriptType) return true;
	return false;
}

bool GameObject::AddComponent(const Ptr<Component>& _Com)
{
	if (CRenderComponent* RenderComponent = dynamic_cast<CRenderComponent*>(_Com.Get()))
	{
		// 이미 렌더 컴포넌트가 존재
		if (m_RenderCom) return false;
		m_RenderCom = RenderComponent;
	}
	
	if (CCollider2D* Collider2DComponent = dynamic_cast<CCollider2D*>(_Com.Get()))
	{
		// Collision검사의 모호성 때문에, 하나의 오브젝트 당 하나의 Collider2D를 갖도록 함
		// 만일 다른 Collider Shape이 추가로 필요하다면, GameObject Child를 통해 처리할 것
		if (m_Collider2D) return false;
		m_Collider2D = Collider2DComponent;
	}
	
	// 입력으로 들어온 컴포넌트가 스크립트면, vector로 관리
	if (_Com->GetComponentType() == COMPONENT_TYPE::SCRIPT)
		m_vecScripts.push_back(dynamic_cast<CScript*>(_Com.Get()));
	
	else // 입력으로 들어온 컴포넌트가 스크립트가 아니면, 알맞은 배열 포인터로 가리킴
	{
		const UINT componentIdx = static_cast<UINT>(_Com->GetComponentType());
		if (m_Components[componentIdx]) return false;
		m_Components[componentIdx] = _Com;
	}
	
	_Com->m_Owner = this;
	_Com->Init();
	return true;
}

void GameObject::AddChild(const Ptr<GameObject>& _Child)
{
	if (_Child->GetParent() == this) return; // 이미 Child로 들어가져있는 오브젝트인 상황
	
	// 부모 오브젝트가 있는지 확인
	if (_Child->GetParent())
	{
		// 기존 부모 오브젝트와 관계를 해제한다.
		_Child->DisconnectWithParent();
	}
	else // 최상위 부모 오브젝트 였다면, Layer에서 최상위 부모 오브젝트에 등록되어있음
	{
		// 레벨 내부에 있던 오브젝트여야 Layer에서의 Parent GameObject에서 제거
		if (_Child->m_bInLayer)
			_Child->DeregisterAsParent(); // Layer에서 최상위 부모로 가리키던 포인터를 제거
	}

	m_vecChild.push_back(_Child); 
	_Child->m_Parent = this;
	
	if (!_Child->m_bInLayer) // 레벨 밖에 있던 오브젝트가(외부의) Level에 새로 합류한 상황
	{
		_Child->m_LayerIdx = this->m_LayerIdx; // Parent의 LayerIdx를 따르게끔 처리
		_Child->m_bInLayer = true; // 레벨 합류 처리

		// 부모가 될 오브젝트는 레벨 내부 소속인 경우 + 레벨이 Play 모드
		if (m_bInLayer && LevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::PLAY)
		{
			// Play중인 레벨 안에 있는 어떤 오브젝트의 자식으로서 레벨에 합류했기 때문에, Begin 호출
			_Child->Begin(); 
		}
	}

	// 부모오브젝트가 레벨 소속이면
	if (m_bInLayer)
	{
		// 현재 레벨에 변경이 발생했음을 알림
		if (LevelMgr::GetInst()->GetCurLevel())
			LevelMgr::GetInst()->GetCurLevel()->SetChanged();
	}
}

void GameObject::DisconnectWithParent()
{
	if (!m_Parent) return;

	// 레벨 소속인 경우, Changed true 처리 
	if (m_bInLayer) LevelMgr::GetInst()->GetCurLevel()->SetChanged();
	
	vector<Ptr<GameObject>>::iterator iter = m_Parent->m_vecChild.begin();
	for (; iter != m_Parent->m_vecChild.end(); ++iter)
	{
		if (*iter == this)
		{
			m_Parent->m_vecChild.erase(iter);
			m_Parent = nullptr;
			return;
		}
	}
	
	assert(nullptr);
}

void GameObject::DeregisterAsParent()
{
	Ptr<ALevel> pCurLevel = LevelMgr::GetInst()->GetCurLevel();
	Layer* pLayer = pCurLevel->GetLayer(m_LayerIdx); // Layer의 경우 스마트포인터로 가리키면 삭제당함
	pLayer->DeregisterAsParent(this);
}

void GameObject::RegisterAsParent()
{
	// if (m_bInLayer) return;
	LevelMgr::GetInst()->GetCurLevel()->GetLayer(m_LayerIdx)->AddObject(this);
}

void GameObject::SetActive(bool _Active, bool _SetActiveHierarchy)
{
	m_IsActive = _Active;
	
	if (!_Active)
	{
		// 여기서 한 Tick에 대해서는 처리를 해주어여 함 -> 바로 Active false를 하는 순간 VecAllObject에 들어가질 않아버림
		// 최종 TaskMgr에서 해당 MarkedDeactivated된 GameObject들에 대해 false로 풀어줌으로써 다음 Tick부터 FinalTick 호출이 안되게끔 처리를 한다
		m_ObjectMarkedDeactivated = true;
		
		// PoolingObject의 OnDeactivate 대리자 호출
		for (const function<void(const Ptr<GameObject>&)>& OnDeactivate : m_vecDelegateOnDeactivate)
			OnDeactivate(this);
	}

	// 자식까지도 Active 상태 업데이트 처리
	if (_SetActiveHierarchy)
	{
		for (const Ptr<GameObject>& Child : m_vecChild)
			Child->SetActive(_Active, _SetActiveHierarchy);
	}
}

bool GameObject::SetLayerIdx(int _LayerIdx)
{
	// 현재 레벨이 Stop 상태가 아니라면, Layer 변경 불가
	if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::STOP) return false;

	// 프리팹 프로토타입 오브젝트의 경우, 예외적으로 LayerIdx를 수정가능하게끔 한다.
	if (APrefab::IsPrefabPrototypeObject(this))
	{
		m_LayerIdx = _LayerIdx;
		return true;
	}
	
	// 현재 레벨에 배치되지 않은 GameObject의 경우, LayerIdx 설정을 setter로 설정할 수 없게끔 조치
	if (!m_bInLayer) return false;

	// 최상위 부모가 아닌 경우, LayerIdx를 바꿀 수 없다 -> 자식은 무조건 Parent의 LayerIdx를 따른다 (TODO : 이거도 맞나...충돌처리를 자식은 다른 Layer로 빼서 할 수도 있지 않나)
	if (m_Parent) return false;

	int prevLayer = m_LayerIdx;
	m_LayerIdx = _LayerIdx;
	
	//  바뀐 LayerIdx에 대해 실질적으로 현재 Level에서 바뀐 Layer에 넣어주어야 함
	Ptr<ALevel> CurLevel = LevelMgr::GetInst()->GetCurLevel();
	CurLevel->GetLayer(prevLayer)->DeregisterAsParent(this);
	CurLevel->AddObject(m_LayerIdx, this);
	
	return true;
}

void GameObject::RegisterLayer()
{
	Ptr<ALevel> pCurLevel = LevelMgr::GetInst()->GetCurLevel();
	Layer* pLayer		  = pCurLevel->GetLayer(m_LayerIdx);
	
	pLayer->RegisterObjectToAllObjects(this);
}

void GameObject::Destroy()
{
	if (m_ObjectDestroyed) return; // 이미 삭제 요청이 들어갔었던 Object
    
	TaskInfo info = {};
    
	info.Type       = TASK_TYPE::DESTROY_OBJECT;
	info.Param_0    = reinterpret_cast<DWORD_PTR>(this);
	TaskMgr::GetInst()->AddTask(info);
}

void GameObject::SaveToLevelFile(FILE* _File)
{
	// 이름
	SaveWString(_File, GetName());
	
	// 컴포넌트
	for (UINT i = 0; i < static_cast<UINT>(COMPONENT_TYPE::END); ++i)
	{
		if (!m_Components[i]) continue; // 가지고 있는 Component만 저장

		// 컴포넌트 타입
		fwrite(&i, sizeof(UINT), 1, _File);
		
		// 컴포넌트 내용
		m_Components[i]->SaveToLevelFile(_File);
	}

	// 컴포넌트 끝 마킹 처리
	UINT ComEnd = static_cast<UINT>(COMPONENT_TYPE::END);
	fwrite(&ComEnd, sizeof(UINT), 1, _File);
	
	// Script
	size_t ScriptCount = m_vecScripts.size();
	fwrite(&ScriptCount, sizeof(size_t), 1, _File);
	
	for (const auto& Script : m_vecScripts)
	{
		wstring ScriptName = ScriptMgr::GetScriptName(Script.Get());
		SaveWString(_File, ScriptName);
		
		Script->SaveToLevelFile(_File);
	}
	
	// ChildObject
	size_t ChildCount = m_vecChild.size();
	fwrite(&ChildCount, sizeof(size_t), 1, _File);
	for (const Ptr<GameObject>& Child : m_vecChild)
		Child->SaveToLevelFile(_File);
	
	// Layer Idx 저장
	fwrite(&m_LayerIdx, sizeof(UINT), 1, _File);
	
	// Active, Visible 상태 저장
	fwrite(&m_IsActive, sizeof(bool), 1, _File);
	fwrite(&m_IsVisible, sizeof(bool), 1, _File);
}

void GameObject::LoadFromLevelFile(FILE* _File)
{
	// 이름
	SetName(LoadWString(_File));
	
	if (GetName() == L"Cliff1")
	{
		int a = 0;
	}
	
	// 컴포넌트
	UINT ComType{};
	
	while (true)
	{
		fread(&ComType, sizeof(UINT), 1, _File);
		if (ComType == static_cast<UINT>(COMPONENT_TYPE::END)) break;
		// if (ComType == 13) break;

		Ptr<Component> pComponent{};
		
		switch (static_cast<COMPONENT_TYPE>(ComType))
		{
		case COMPONENT_TYPE::TRANSFORM:			pComponent = new CTransform;		break;
		case COMPONENT_TYPE::CAMERA:			pComponent = new CCamera;			break;
			
		case COMPONENT_TYPE::COLLIDER2D_RECT:	pComponent = new CColliderRect;		break;
		case COMPONENT_TYPE::COLLIDER2D_CIRCLE:	pComponent = new CColliderCircle;	break;
		case COMPONENT_TYPE::COLLIDER2D_POINT:	pComponent = new CColliderPoint;	break;
			
		case COMPONENT_TYPE::LIGHT2D:			pComponent = new CLight2D;			break;
		case COMPONENT_TYPE::LIGHT3D:			break;
		case COMPONENT_TYPE::MESH_RENDER:		pComponent = new CMeshRender;		break;
		case COMPONENT_TYPE::BILLBOARD_RENDER:	pComponent = new CBillboardRender;	break;
		case COMPONENT_TYPE::FLIPBOOK_RENDER:	pComponent = new CFlipbookRender;	break;
		case COMPONENT_TYPE::PARTICLE_RENDER:	break;
		case COMPONENT_TYPE::SPRITE_RENDER: 	pComponent = new CSpriteRender;		break;
		case COMPONENT_TYPE::TILE_RENDER:		pComponent = new CTileRender;		break;
		case COMPONENT_TYPE::POOL:				pComponent = new CPoolComponent;	break;
		}

		AddComponent(pComponent);
		pComponent->LoadFromLevelFile(_File);
	}
	
	// 스크립트
	size_t ScriptCount{};
	fread(&ScriptCount, sizeof(size_t), 1, _File);
	
	for (size_t i = 0; i < ScriptCount; ++i)
	{
		wstring ScriptName = LoadWString(_File);
		Ptr<CScript> pScript = ScriptMgr::GetScript(ScriptName);
		
		if (!pScript)
		{
			wstring DebugMsg = GetName() + L"'s " + ScriptName + L" failed to load. Prolly Script class name changed";
			DebugUtil::AddDebugLog(L"[GameObject::LoadFromLevelFile] : " + DebugMsg, DEF_COLOR_RED, 10.f);
			continue;
		}
		
		AddComponent(pScript.Get());
		pScript->LoadFromLevelFile(_File);
	}
	
	// ChildObject 복구
	size_t ChildCount{};
	fread(&ChildCount, sizeof(size_t), 1, _File);
	
	for (size_t i = 0; i < ChildCount; ++i)
	{
		Ptr<GameObject> ChildObject = new GameObject;
		AddChild(ChildObject);
		ChildObject->LoadFromLevelFile(_File);
	}
	
	// LayerIdx 복구 
	fread(&m_LayerIdx, sizeof(UINT), 1, _File);
	
	// Active, Visible 상태 복구
	fread(&m_IsActive, sizeof(bool), 1, _File);
	fread(&m_IsVisible, sizeof(bool), 1, _File);
}
