#include "pch.h"
#include "CPoolComponent.h"

#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/03.Manager/07.TaskMgr/TaskMgr.h"
#include "Source/Manager/GameManager.h"


CPoolComponent::CPoolComponent()
    : Component(COMPONENT_TYPE::POOL)
{
}

CPoolComponent::CPoolComponent(const CPoolComponent& _Origin)
    : Component(_Origin)
    , m_PoolCount(_Origin.m_PoolCount)
    , m_PrefabToPool(_Origin.m_PrefabToPool)
    , m_AttachToSelfObject(_Origin.m_AttachToSelfObject)
{
    // SpawningPool의 경우, Init() 및 LevelState Play일 경우에만 초기화 처리되도록 한다.
}

CPoolComponent::~CPoolComponent()
{
}

void CPoolComponent::Init()
{
}

void CPoolComponent::Begin()
{
    if (!m_PrefabToPool)
    {
        const wstring& OwnerName = GetOwner()->GetName();
        DebugUtil::AddDebugLog(L"[CPoolComponent::Begin] : " + OwnerName + L" Pooling failed!, Set Prefab to this PoolingComponent");
        return;
    }
    
    // Pool 초기화
    for (UINT i = 0; i < m_PoolCount; ++i)
    {
        GameObject* CreatedObject = m_PrefabToPool->Instantiate();

        const wstring& OwnerName = GetOwner()->GetName();
        
        if (!CreatedObject)
        {
            DebugUtil::AddDebugLog(L"[CPoolComponent::Begin] : " + OwnerName + L" Pooling failed!, ProtoObject not set to Prefab!");
            return;
        }
        
        if (!CreatedObject->Transform())
        {
            DebugUtil::AddDebugLog(L"[CPoolComponent::Begin] : " + OwnerName + L" Pooling failed!, ProtoObject must have Transform component!");
            return;                        
        }
        
        const wstring Name = CreatedObject->GetName() + L"(PooledObject" + to_wstring(i) + L")";
        CreatedObject->SetName(Name);

        // Owner PoolComponent 등록
        CreatedObject->SetOwnerPoolComponent(this);

        // 여기서 Layer의 모든 GameObject를 순회할 때, 동일한 Layer에 집어넣는 경우라면 문제가 될 수 있음 -> iterator 순회하는 벡터에 새로운 요소를 추가해서 UB로 빠지는 중
        LevelMgr::GetInst()->GetCurLevel()->AddObject(CreatedObject->GetLayerIdx(), CreatedObject);
        
        if (m_AttachToSelfObject) GetOwner()->AddChild(CreatedObject);
        
        CreatedObject->SetActive(false);
        
        // SetActive(false) Callback 처리이기 때문에 SetActive(false) 이후로 두어야한다
        CreatedObject->AddDeactivateDelegate(bind(&CPoolComponent::OnDeactivateActiveObject, this, placeholders::_1));
        m_SpawningPool.push(CreatedObject);
    }
}

void CPoolComponent::FinalTick()
{
    // Nothing to do
}

GameObject* CPoolComponent::SpawnObject(bool _SetActiveHierarchy)
{
    if (m_SpawningPool.empty())
    {
        DebugUtil::AddDebugLog(L"[CPoolComponent::SpawnObject] : " + GetOwner()->GetName() + L"'s Maximum pool count reached!");
        return nullptr;
    }
    
    TaskInfo info{};

    // Transform 정보를 Tick에서 한번 업데이트 처리를 해주어야, 올바른 위치에 스폰 처리되기 때문에, TaskMgr를 통해 Active를 켜준다
    Ptr<GameObject> gObject = m_SpawningPool.front(); m_SpawningPool.pop();
    
    info.Type       = TASK_TYPE::SPAWN_POOLED_OBJECT;
    info.Param_0    = reinterpret_cast<DWORD_PTR>(gObject.Get());
    info.Param_1    = static_cast<DWORD_PTR>(_SetActiveHierarchy); // 부모와 자식 모두 한번에 켤건지 체킹
    TaskMgr::GetInst()->AddTask(info);
    
    return gObject.Get();
}

GameObject* CPoolComponent::SpawnObject(const Vec3& _SpawnPosition, bool _SetActiveHierarchy)
{
    if (m_SpawningPool.empty())
    {
        DebugUtil::AddDebugLog(L"[CPoolComponentSpawnObject] : " + GetOwner()->GetName() + L"'s Maximum pool count reached!");
        return nullptr;
    }
    
    TaskInfo info{};

    Ptr<GameObject> gObject = m_SpawningPool.front(); m_SpawningPool.pop();
    gObject->Transform()->SetRelativePos(_SpawnPosition);
    
    info.Type       = TASK_TYPE::SPAWN_POOLED_OBJECT;
    info.Param_0    = reinterpret_cast<DWORD_PTR>(gObject.Get());
    info.Param_1    = static_cast<DWORD_PTR>(_SetActiveHierarchy);
    TaskMgr::GetInst()->AddTask(info);
    
    return gObject.Get();
}

void CPoolComponent::SaveToLevelFile(FILE* _File)
{
    // Pool Count 저장
    fwrite(&m_PoolCount, sizeof(UINT), 1, _File);

    // Pooling해둘 GameObject의 APrefab 정보 저장
    SaveAssetRef(_File, m_PrefabToPool.Get());
    
    // Object의 경우, Prefab을 통해 초기화 처리를 한다
    
    fwrite(&m_AttachToSelfObject, sizeof(bool), 1, _File);
}

void CPoolComponent::LoadFromLevelFile(FILE* _File)
{
    // Pool Count 불러오기
    fread(&m_PoolCount, sizeof(UINT), 1, _File);

    // Pooling해둘 GameObject의 APrefab 정보 불러오기
    m_PrefabToPool = LoadAssetRef<APrefab>(_File);
    
    fread(&m_AttachToSelfObject, sizeof(bool), 1, _File);
}