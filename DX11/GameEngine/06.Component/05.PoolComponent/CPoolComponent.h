#pragma once

// TODO : PoolComponent UI 만들기
class CPoolComponent : public Component
{
    
    friend class PoolUI;
    
private:

    UINT                    m_PoolCount{};
    Ptr<APrefab>            m_PrefabToPool{};
    queue<Ptr<GameObject>>  m_SpawningPool{}; // 스폰 가능한 GameObject들 (IsActive가 꺼져있는 게임오브젝트들을 담는다)
    
public:

    CPoolComponent();
    CPoolComponent(const CPoolComponent& _Origin);
    virtual ~CPoolComponent() override;
    CLONE(CPoolComponent)
    
public:

    virtual void Init() override;
    virtual void Begin() override;
    virtual void FinalTick() override;
    
public:
    /// <summary>
    /// 프리팹에서 저장된 기본 Transform 위치정보에 Spawn 처리
    /// </summary>
    /// <returns> : 스폰된 GameObject, 제대로 Spawn되지 않았다면 nullptr </returns>
    GameObject* SpawnObject();

    /// <summary>
    /// 특정 위치에 GameObject 스폰 처리
    /// </summary>
    /// <returns> : 스폰된 GameObject, 제대로 Spawn되지 않았다면 nullptr </returns>
    GameObject* SpawnObject(const Vec3& _SpawnPosition);
    
private:
    
    /// <summary>
    /// Pool에서 스폰된 Object들이 Deactivate 처리되었을 때(SetActive(false)) 처리되었을 때 호출됨
    /// 다시금 Pool에 돌아가는 처리
    /// </summary>
    void OnDeactivateActiveObject(const Ptr<GameObject>& _GameObject) { m_SpawningPool.push(_GameObject); }

public:
    
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
    
};
