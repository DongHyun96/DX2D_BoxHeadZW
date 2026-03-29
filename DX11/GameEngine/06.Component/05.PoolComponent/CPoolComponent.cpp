#include "pch.h"
#include "CPoolComponent.h"

#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"


CPoolComponent::CPoolComponent()
    : Component(COMPONENT_TYPE::POOL)
{
}

CPoolComponent::CPoolComponent(const CPoolComponent& _Origin)
    : Component(_Origin)
    , m_PoolCount(_Origin.m_PoolCount)
    , m_PrefabToPool(_Origin.m_PrefabToPool)
{
    // SpawningPool의 경우, Init() 및 LevelState Play일 경우에만 초기화 처리되도록 한다.
}

CPoolComponent::~CPoolComponent()
{
}

void CPoolComponent::Init()
{
    if (LevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::STOP) return; // PLAY 시에만 실질적인 Pooling 처리
    
    if (!m_PrefabToPool)
    {
        const wstring& OwnerName = GetOwner()->GetName();
        DebugUtil::AddDebugLog(L"[CPoolComponent::Init] : " + OwnerName + L" Pooling failed!, Set Prefab to this PoolingComponent");
        return;
    }
    
    // Pool 초기화
    for (UINT i = 0; i < m_PoolCount; ++i)
    {
        GameObject* CreatedObject = m_PrefabToPool->Instantiate();
        if (!CreatedObject)
        {
            const wstring& OwnerName = GetOwner()->GetName();
            DebugUtil::AddDebugLog(L"[CPoolComponent::Init] : " + OwnerName + L" Pooling failed!, ProtoObject not set to Prefab!");
            return;
        }
        
        // TODO : 여기서 Level에 추가를 하면, 음... 괜찮나? -> PLAY시에만 처리를 하니까 괜찮으려나
        LevelMgr::GetInst()->GetCurLevel()->AddObject(CreatedObject->GetLayerIdx(), CreatedObject);
        CreatedObject->SetActive(false);
    }
}

void CPoolComponent::Begin()
{
}

void CPoolComponent::FinalTick()
{
}

GameObject* CPoolComponent::Spawn()
{
    if (m_SpawningPool.empty()) return nullptr;

    return nullptr;
}

GameObject* CPoolComponent::SpawnObject(const Vec3& _SpawnPosition)
{
    return nullptr;
}

void CPoolComponent::SaveToLevelFile(FILE* _File)
{
    // Pool Count 저장
    fwrite(&m_PoolCount, sizeof(UINT), 1, _File);

    // Pooling해둘 GameObject의 APrefab 정보 저장
    SaveAssetRef(_File, m_PrefabToPool.Get());
    
    // Object의 경우, Prefab을 통해 초기화 처리를 한다
}

void CPoolComponent::LoadFromLevelFile(FILE* _File)
{
    // Pool Count 불러오기
    fread(&m_PoolCount, sizeof(UINT), 1, _File);

    // Pooling해둘 GameObject의 APrefab 정보 불러오기
    m_PrefabToPool = LoadAssetRef<APrefab>(_File);
}