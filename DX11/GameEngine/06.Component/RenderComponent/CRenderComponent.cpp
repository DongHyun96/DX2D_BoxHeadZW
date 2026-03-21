#include "pch.h"
#include "CRenderComponent.h"

#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"

CRenderComponent::CRenderComponent(COMPONENT_TYPE _Type)
    : Component(_Type)
{
}

CRenderComponent::CRenderComponent(const CRenderComponent& _Origin)
    : Component(_Origin)
    , m_Mesh(_Origin.m_Mesh)
    , m_SharedMaterial(_Origin.m_SharedMaterial)
{
    // 원본 렌더컴포넌트가 공유재질(에셋 매니저로부터 관리되는)을 사용하고 있다면
    if (_Origin.m_Material == _Origin.m_SharedMaterial)
        m_Material = m_SharedMaterial;
    
    // 원본의 동적재질이 존재하고, 현재 사용중인 재질이 동적 재질로 설정된 경우
    else if (_Origin.m_DynamicMaterial && _Origin.m_Material == _Origin.m_DynamicMaterial) 
        CreateDynamicMaterial();
}

CRenderComponent::~CRenderComponent()
{
}

Ptr<AMaterial> CRenderComponent::ReturnToSharedMaterial()
{
    m_Material = m_SharedMaterial;
    return m_Material;
}

void CRenderComponent::Init()
{
    CreateMaterial();
}

void CRenderComponent::Begin()
{
}

Ptr<AMaterial> CRenderComponent::CreateDynamicMaterial()
{
    // 동적 재질 생성은 반드시 레벨이 Play 모드일 경우에만 사용 가능한 기능
    assert(LevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::PLAY);
    
    if (!m_DynamicMaterial) m_Material = m_DynamicMaterial = m_SharedMaterial->Clone(); // 처음 생성할 때
    else                    m_Material = m_DynamicMaterial; // 기존에 생성한 DynamicMaterial이 있을 때
    
    return m_Material;
}

void CRenderComponent::PrintIrrelevantDebugLog() const
{
    DebugUtil::AddDebugLog(L"[RenderComponent::CreateMaterial] Above not found msg is irrelevant");
}

void CRenderComponent::SaveToLevelFile(FILE* _File)
{
    SaveAssetRef(_File, m_Mesh.Get());
    SaveAssetRef(_File, m_Material.Get());
    SaveAssetRef(_File, m_SharedMaterial.Get());
}

void CRenderComponent::LoadFromLevelFile(FILE* _File)
{
    m_Mesh           = LoadAssetRef<AMesh>(_File);
    m_Material       = LoadAssetRef<AMaterial>(_File);
    m_SharedMaterial = LoadAssetRef<AMaterial>(_File);
}
