#pragma once
#include <vector>
#include <functional>

#include "GameEngine/04.Asset/Entity.h"
#include "GameEngine/06.Component/Script/CScript.h"
#include "Header/components.h"
#include "Module/Ptr.h"

#define GET_COMPONENT(COM_NAME, COM_TYPE) Ptr<C##COM_NAME> COM_NAME() const {return dynamic_cast<C##COM_NAME*>(m_Components[static_cast<UINT>(COMPONENT_TYPE::COM_TYPE)].Get()); }

enum class DT_CONTEXT_TYPE;
class CMeshRender;

class GameObject : public Entity
{
	
	friend class TaskMgr;
	friend class Layer;
	friend class Menu;
	friend class Outliner;
	friend class APrefab;

private:
	
	GUID m_GUID{};
	
private:

	// Text Object인지 여부 (Rendering 처리를 따로 해주어야 해서 체킹함)
	bool m_bIsTextObject{};
	
private:

	// SetActive(false) 시, 한 번은 다음 FinalTick()을 호출받아 vecAllObjects에 들어가 있어야 제대로 된 충돌검사 및 CallBack호출이 이루어지기 때문에
	// SetActive(false) 조작 시, Pending 처리로 FinalTick 한번은 호출처리한다.
	bool m_ObjectMarkedDeactivated{};
	bool m_IsActive  = true;
	bool m_IsVisible = true;
	
	vector<function<void(GameObject*)>> 	m_vecDelegateOnActivate{};		// SetActive true 처리될 때 CallBack 처리
	vector<function<void(GameObject*)>> 	m_vecDelegateOnDeactivate{};	// SetActive false 처리될 때 CallBack 처리
	map<DWORD_PTR, function<void()>>		m_mapDelegateOnDestroy{};		// Destroy 처리될 때 Callback 처리 -> GameObjectRefHolder에게 Destroy 되었다고 알림 처리할 때 사용 및 Script한테 Destroy 알리는 등 처리
	map<CScript*, function<void()>>			m_mapDelegateOnRemoveScript{};	// RemoveScript 호출 시, Callback 처리
	
private: // TimeScale에 따른 DT Context 관련

	DT_CONTEXT_TYPE m_DTContextType{};
	// bool m_IgnoreGlobalTimeScale{}; // true면 TimeScale을 무시한 DeltaTime 사용 (PLAY 중엔 E_DT와 동일)
	
private:

	Ptr<Component>			m_Components[static_cast<UINT>(COMPONENT_TYPE::END)]{};
	vector<Ptr<CScript>>	m_vecScripts{};

	Ptr<CCollider2D>		m_Collider2D{};
	Ptr<CRenderComponent>	m_RenderCom{};

	// 종속 관계에서 Parent가 Child를 스마트포인터로 소유 / Child는 일반 포인터로 부모를 가리킴으로 순환 참조 방지
	// 자식이 사라져도 부모 객체는 살아있음
	GameObject*				m_Parent{};
	vector<Ptr<GameObject>> m_vecChild{};

	// GameObject 본인이 속한 Layer Index
	// -1인 경우, 어떤 레이어에도 속하지 않는다 == 레벨안에 있지 않은 오브젝트
	// 실질적으로 Level 구조에서 사용하는 LayerIdx 변수
	int						m_LayerIdx = -1;
	bool					m_bInLayer{}; // 레이어에 현재 속해 있는지 체크
	
	bool					m_ObjectDestroyed{}; // 곧 사라질 GameObject

private:
	
	CPoolComponent*			m_OwnerPoolComponent{}; // ObjectPooling 처리로 생성된 GameObject들의 Pool
	
public:

	GameObject();
	GameObject(const GameObject& _Origin);
	
	virtual ~GameObject() override;

public:
	
	GameObject* Clone() const { return new GameObject(*this); };
	
public:
	
	/// <summary>
	/// <para> 현재 Level의 GUID 초기화 처리 이후, 바로 호출될 함수 </para>
	/// <para> GO 레퍼런스를 다시 재연결해야 하는 Component(Script 포함)에 대해, 다시 재연결 처리를 이 시점에서 한다 </para>
	/// </summary>
	void AfterLevelGameObjectGuidTableInit();
	
	void Begin();
	void AfterLevelBegin();
	void Tick();

	/// <summary> 매 프레임마다 Tick 이후에 뒷 수습작업 수행 </summary>
	void FinalTick();
	
	/// <summary> Editor 전용 GameObject FinalTick </summary>
	void FinalTick_Editor();
	
	void Render();
	
public:
	
	/// <summary>
	/// Type에 대응되는 Component 제거 
	/// </summary>
	/// <returns> : 제거할 대상 Component가 없거나, TYPE END일 경우 return false </returns>
	bool RemoveComponent(COMPONENT_TYPE _Type);

	
	bool AddComponent(const Ptr<Component>& _Com);
	Ptr<Component> GetComponent(COMPONENT_TYPE _Type) { return m_Components[static_cast<UINT>(_Type)]; }

	void AddDeactivateDelegate(const function<void(GameObject*)>& _Delegate) { m_vecDelegateOnDeactivate.push_back(_Delegate); }
	// void RemoveDeactivateDelegate(const function<void()>& _Delegate) // 이건 functional 특성 상 wrapper이기 때문에 직접 비교가 불가능 -> 특정 요소를 찝어서 remove처리 불가능
	void AddActivateDelegate(const function<void(GameObject*)>& _Delegate) { m_vecDelegateOnActivate.push_back(_Delegate); }
	
	void AddDestroyDelegate(DWORD_PTR _Delegator, const function<void()>& _Delegate) { m_mapDelegateOnDestroy.insert(make_pair(_Delegator, _Delegate)); }
	void AddRemoveScriptDelegate(CScript* _ScriptTarget, const function<void()>& _Delegate) { m_mapDelegateOnRemoveScript.insert(make_pair(_ScriptTarget, _Delegate)); }
	
public:
	
	/// <summary>
	/// TargetScript에 대응되는 Script 제거 
	/// </summary>
	/// <returns> : 제거할 대상이 없을 경우 return false </returns>
	bool RemoveScript(const Ptr<CScript>& _TargetScript);

	/// <summary>
	/// T Pointer 형으로 dynamic casting 처리가능한 첫 번째 Script 반환 
	/// </summary>
	/// <returns> : 해당 Script Type이 없다면 return nullptr </returns>
	template<typename T>
	Ptr<T> GetScriptComponent() const;

	/// <summary>
	/// 해당하는 Type의 Script 반환
	/// </summary>
	/// <returns> : 해당 Script Type이 없다면 nullptr 반환 </returns>
	CScript* GetScriptComponent(SCRIPT_TYPE _ScriptType) const;
	
	bool HasScript(SCRIPT_TYPE _ScriptType) const;
	
	
	const vector<Ptr<CScript>>& GetScripts() const { return m_vecScripts; }

public:
	
	void AddChild(const Ptr<GameObject>& _Child);
	void DisconnectWithParent();
	void DeregisterAsRootParent();
	void RegisterAsParent();
	
	int GetChildIdx(const Ptr<GameObject>& _Child);
	void MoveChildOrder(const Ptr<GameObject>& _Child, int _Dir);

public:
	
	GameObject* GetParent() const { return m_Parent; }
	
	Ptr<GameObject> GetChild(UINT _Idx) const { return m_vecChild[_Idx]; }
	Ptr<GameObject> GetChildByName(const wstring& _ObjectName) const;
	const vector<Ptr<GameObject>>& GetChildren() const { return m_vecChild; }

public:
	
	bool IsObjectDestroyed() const { return m_ObjectDestroyed; }
	
	bool GetActive() const { return m_IsActive; }
	
	/// <summary>
	/// Active 상태 전환 
	/// </summary>
	/// <param name="_Active"></param>
	/// <param name="_SetActiveHierarchy"> : 포함된 자식 오브젝트 까지도 Active 상태를 수정할건지 (false면 자기자신만 수정) </param>
	void SetActive(bool _Active, bool _SetActiveHierarchy = true);

	bool GetObjectMarkedDeactivated() const { return m_ObjectMarkedDeactivated; }
	bool GetVisible() const { return m_IsVisible; }
	
	
	void SetVisible(bool _Visible) { m_IsVisible = _Visible; }

	void SetDTContextType(DT_CONTEXT_TYPE _ContextType) { m_DTContextType = _ContextType; }
	
public:
	
	void SetIsTextObject(bool _IsTextObject) { m_bIsTextObject = _IsTextObject; }
	bool GetIsTextObject() const { return m_bIsTextObject; }

public:
	
	GUID GetGUID();
	
private:
	
	GUID& GetGUIDRef();
	
public:
	
	int GetLayerIdx() const { return m_LayerIdx; }
	bool SetLayerIdx(int _LayerIdx);
	
public:

	// 특정 컴포넌트를 다운캐스팅해서 바로 리턴
	GET_COMPONENT(Transform,		TRANSFORM)
	GET_COMPONENT(MeshRender,		MESH_RENDER)
	GET_COMPONENT(BillboardRender,	BILLBOARD_RENDER);
	GET_COMPONENT(Camera,			CAMERA);
	
	GET_COMPONENT(ColliderRect,		COLLIDER2D_RECT);
	GET_COMPONENT(ColliderCircle,	COLLIDER2D_CIRCLE);
	GET_COMPONENT(ColliderPoint,	COLLIDER2D_POINT);
	
	GET_COMPONENT(SpriteRender,		SPRITE_RENDER)
	GET_COMPONENT(FlipbookRender,	FLIPBOOK_RENDER)
	GET_COMPONENT(TileRender,		TILE_RENDER)
	GET_COMPONENT(ParticleRender,   PARTICLE_RENDER)
	GET_COMPONENT(Light2D,			LIGHT2D)
	GET_COMPONENT(PoolComponent,    POOL)
	
public:
	
	Ptr<CCollider2D> GetCollider2D() const { return m_Collider2D; }
	Ptr<CRenderComponent> GetRenderCom() const { return m_RenderCom; }
	
	/*Ptr<CTransform>  Transform()  {return dynamic_cast<CTransform*>(m_Components[static_cast<UINT>(COMPONENT_TYPE::TRANSFORM)].Get()); }
	Ptr<CMeshRender> MeshRender() {return dynamic_cast<CMeshRender*>(m_Components[static_cast<UINT>(COMPONENT_TYPE::MESHRENDER)].Get()); }
	Ptr<CCamera> Camera() {return dynamic_cast<CCamera*>(m_Components[static_cast<UINT>(COMPONENT_TYPE::CAMERA)].Get()); }*/

	/// <summary>
	/// Level Asset 저장 시, GameObject 저장 기능 
	/// </summary>
	void SaveToLevelFile(FILE* _File);

	void LoadFromLevelFile(FILE* _File);
	
public:
	
	void RegisterLayer();
	
	void Destroy();
	
public:
	
	void SetOwnerPoolComponent(CPoolComponent* _OwnerPoolComponent) { this->m_OwnerPoolComponent = _OwnerPoolComponent; }
	CPoolComponent* GetOwnerPoolComponent() const { return m_OwnerPoolComponent; }
	
};

template <typename T>
Ptr<T> GameObject::GetScriptComponent() const
{
	for (const Ptr<CScript>& script : m_vecScripts)
		if (Ptr<T> casted = dynamic_cast<T*>(script.Get())) 
			return casted;
	
	return nullptr;
}

bool IsValid(Ptr<GameObject>& _Object);
bool IsValid(const Ptr<GameObject>& _Object);
bool IsValid(GameObject* _Object);

