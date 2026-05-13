#pragma once
#include "GameEngine/04.Asset/Asset.h"
#include "GameEngine/05.GameObject/GameObject.h"
#include "Layer/Layer.h"

/*
 * LayerNames
 * 
 * 
 */

class ALevel : public Asset
{

    friend class LevelUI;

private:
    
    // CurLevel의 LevelStop 상황과 LevelPlay 상태에서의 GUID 값은 모두 동일
    // Level이 복제되어, 동일한 Guid를 가진 GameObject들이 막 생성이 되었을 때, Begin 호출 이전에 해당 테이블 초기화 처리됨
    // 주의 : Stop 상태의 원본 Level에서의 GameObject만이 GuidTable에 등록되어 처리됨
    // Play 시작되었을 때, 새로이 생성된 오브젝트들에 대해서는 테이블에 추가되지 않는다
    // 애초에 목적이 Stop 상황 Editing 상황 시의 GameObjectReference와 Play 상황에서의 GameObjectReference 싱크를 맞추기 위함이기 때문
    unordered_map<GUID, GameObject*, GUIDHasher> m_GuidTable{};

private:

    // Editing 환경에서(LEVELSTATE_STOP), Tick 함수를 돌려봐야 하는 GameObject들 등록해서 사용할 것
    // 주의할 점 : Editing 환경에서의 수정된 값으로 Level Play 진입 시, 초기 값이 아닌 수정된 값으로 시작될 수 있는 점 주의할 것
    set<GameObject*> m_setEditingTickEnabledGameObject{};
    
private:
    
    Layer   m_arrLayer[MAX_LAYER]{};        // 하나의 레벨안에 총 32개의 레이어가 존재
    UINT    m_CollisionMatrix[MAX_LAYER]{}; // 어떤 레이어와, 어떤 레이어가 충돌검사를 진행할지 마킹한 데이터 테이블
    bool    m_Changed{};                    // 레벨 안에 오브젝트들의 상태가 변경됐는지 확인하는 변수 (오브젝트 추가, 삭제, 또는 오브젝트끼리의 계층관계가 변경 등)

private:
    
    /// <summary>
    /// 레벨 Begin 시, 현재 LayerName과 Idx 저장 (UnNamed Layer의 경우 저장 x)
    /// 주의 : 중복된 LayerName이 존재하면 안된다. -> 중복 안되게끔 사전 처리 모두 해놓음
    /// </summary>
    map<wstring, UINT> m_mapLayerNameIndex{};

private: // 이 레벨에서 사용될 FirstMainCamera와 UICamera 정보 (CCamera 컴포넌트 Init에서 자동 세팅 처리되게끔 처리함)
    
    CCamera* m_FirstMainCamera{};
    CCamera* m_UICamera{};

private:

    // Save 도중, 다시 Save 처리를 해야하는 상황이 생겼을 때 true
    bool m_bRetrySave{};
    
public:
    
    ALevel();
    ALevel(const ALevel& _Origin);
    virtual ~ALevel() override;
    
public:
    
    ALevel* Clone() const { return new ALevel(*this); } // 복사생성자를 사용한 새로운 레벨 생성
    
    void AddObject(int _LayerIdx, const Ptr<GameObject>& _Object);
    void Deregister();
    
    void AddEditingTickEnabledGameObject(GameObject* _GameObject);
    void RemoveEditingTickEnabledGameObject(GameObject* _GameObject);

    
    /// <summary>
    /// Level 처음 시작 시 호출 
    /// </summary>
    void Begin();

    /// <summary>
    /// Level 시작 처리 이후 호출 
    /// </summary>
    void AfterLevelBegin();

    /// <summary>
    /// Editing 환경에서의 Tick 함수 처리 -> 등록받은 GameObject에 대해서만 Editing 환경에서의 EditingTick 함수로
    /// Tick Test를 해볼 수 있게끔 처리한다 (주의할 점 : Editing 환경에서의 수정된 값으로 Level Play 진입 시, 초기 값이 아닌 수정된 값으로 시작될 수 있는 점 주의할 것)
    /// </summary>
    void EditingTick();
    
    void Tick();
    void FinalTick();

public:
    /// <summary>
    /// 이미 해당 layer 이름이 이 Level에 존재하는지 체크 (자기 자신 Idx는 제외) 
    /// </summary>
    bool IsLayerNameDuplicated(const wstring& _LayerName, int _SelfIdx);

    /// <summary>
    /// LayerName으로 LayerIndex 찾기
    /// </summary>
    /// <param name="_LayerName"></param>
    /// <returns> : 만약 해당하는 이름이 없다면 return -1 </returns>
    int GetLayerIndexByLayerName(const wstring& _LayerName) const;
    
public:
    
    void CheckCollisionLayer(UINT _LayerIdx1, UINT _LayerIdx2);
    void CheckCollisionLayer(const wstring& _LayerName1, const wstring& _LayerName2);
    
    UINT* GetCollisionMatrix() { return m_CollisionMatrix; }

    void RequestRetrySave() { m_bRetrySave = true; }
    
    virtual HRESULT Save(const wstring& _FilePath) override;
    virtual HRESULT Load(const wstring& _FilePath) override;
    
public:
    
    Layer* GetLayer(UINT _Idx)
    {
        assert(0 <= _Idx && _Idx < MAX_LAYER);
        return &(m_arrLayer[_Idx]);
    }

    Ptr<GameObject> FindObjectByName(const wstring& _Name);
    GameObject* GetObjectByGUID(const GUID& _GUID);

    /// <summary>
    /// 해당 GameObject 객체가 이 Level에 존재하는지 확인
    /// </summary>
    bool IsObjectInLevel(const Ptr<GameObject>& _Object);

    /// <summary>
    /// <para> Level의 변경점이 있는지 확인 </para>
    /// <para> 변경점을 확인하면서, 동시에 변경점이 있었다고 체크된 상태였다면, 해당상태 다시 되돌려둠 </para>
    /// </summary>
    bool HasChanged()
    {
        bool Changed = m_Changed;
        m_Changed = false;
        return Changed;
    }

    /// <summary>
    /// 레벨에 변경사항이 있다면(ex 오브젝트 추가, 삭제, Hierarchy의 변화 등) 변화 여부 true로 기록 처리
    /// </summary>
    void SetChanged() { m_Changed = true; }
    
public:
    
    /// <summary>
    /// 레벨 첫 시작 시, 첫 MainCamera로 지정될 MainCamera 지정 (Editor 기능 내에서 호출할 함수) 
    /// </summary>
    bool SetFirstMainCamera(CCamera* _Camera);
    bool SetUICamera(CCamera* _Camera);
    CCamera* GetUICamera() const { return m_UICamera; }

public:
    
    /// <summary>
    /// 이 Leve에 있는 모든 GO(Child 오브젝트 포함)를 순회하며, GameObject의 GUID를 통해, Level의 GUIDTable 초기화 
    /// </summary>
    void InitGuidTable();
    
private:
    
    /// <summary>
    /// GameObject Guid 테이블 초기화 이후 업데이트된 Guid 테이블을 이용하여, GameObject 레퍼런스를 다시 잡아줌 
    /// </summary>
    void AfterInitGuidTable();
    
};
