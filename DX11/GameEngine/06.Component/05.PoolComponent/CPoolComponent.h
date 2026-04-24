#pragma once

#include "GameEngine/04.Asset/09.Prefab/APrefab.h"

class CPoolComponent : public Component
{
    
    friend class PoolUI;
    
private:

    UINT                        m_PoolCount{};
    Ptr<APrefab>                m_PrefabToPool{}; // -> Layer Default GameObject는 Pooling 하지 말것 -> PoolComponent GameObject와 다른 Layer여야 Begin 처리 시에 Layer GameObject iterator가 정상 동작함
    deque<GameObject*>          m_SpawningPool{}; // 스폰 가능한 GameObject들 (IsActive가 꺼져있는 게임오브젝트들을 담는다)

private:
    
    bool m_AttachToSelfObject{}; // 자기자신이 PoolComponent임과 동시에, Pooling한 오브젝트를 본인의 자식으로 처리를 해야하면 체크 (주의, 본인과 다른 Layer를 쓰는 오브젝트여야한다)
    
public:

    CPoolComponent();
    CPoolComponent(const CPoolComponent& _Origin);
    virtual ~CPoolComponent() override;
    CLONE(CPoolComponent);
    
public:

    virtual void Init() override;
    virtual void Begin() override;
    virtual void FinalTick() override;
    
public:
    /// <summary>
    /// 프리팹에서 저장된 기본 Transform 위치정보에 Spawn 처리
    /// </summary>
    /// <returns> : 스폰된 GameObject, 제대로 Spawn되지 않았다면 nullptr </returns>
    GameObject* SpawnObject(bool _SetActiveHierarchy = true);

    /// <summary>
    /// 특정 위치에 GameObject 스폰 처리
    /// </summary>
    /// <returns> : 스폰된 GameObject, 제대로 Spawn되지 않았다면 nullptr </returns>
    GameObject* SpawnObject(const Vec3& _SpawnPosition, bool _SetActiveHierarchy = true);
    
    bool CanSpawnObject() const { return !m_SpawningPool.empty(); }
    
public:
    
    /// <summary>
    /// Pooler 오브젝트가 풀링한 오브젝트를 자식으로 두고 있는지의 여부 
    /// </summary>
    bool IsPooledObjectAttachedToPooler() const { return m_AttachToSelfObject; }
    
private:
    
    /// <summary>
    /// Pool에서 스폰된 Object들이 Deactivate 처리되었을 때(SetActive(false)) 처리되었을 때 호출됨
    /// 다시금 Pool에 돌아가는 처리
    /// </summary>
    void OnDeactivateActiveObject(GameObject* _GameObject)
    {
        m_SpawningPool.push_front(_GameObject);
    }

public:
    
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
    
};
