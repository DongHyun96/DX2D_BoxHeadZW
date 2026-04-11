#include "pch.h"
#include "CRenderComponent.h"

#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/06.Component/01.Transform/CTransform.h"

CRenderComponent::CRenderComponent(COMPONENT_TYPE _Type)
    : Component(_Type)
{
}

CRenderComponent::CRenderComponent(const CRenderComponent& _Origin)
    : Component         (_Origin)
    , m_Mesh            (_Origin.m_Mesh)
    , m_SharedMaterial  (_Origin.m_SharedMaterial)
    , m_RenderOffset    (_Origin.m_RenderOffset)
    , m_RenderScale     (_Origin.m_RenderScale)
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
    if (!GetMesh() || !GetMaterial()) return;
    RenderMgr::GetInst()->AddGameObjectToRenderDomain(m_Material->GetDomain(), GetOwner());
}

void CRenderComponent::AfterLevelBegin()
{
}

void CRenderComponent::FinalTick()
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

void CRenderComponent::ApplyRenderTransformConst()
{
    if (!m_Material) return;
    // Render 전용 Offset 및 Scale 값 세팅
    m_Material->SetScalar(VEC4_3, Vec4(
        m_RenderOffset.x, m_RenderOffset.y,
        m_RenderScale.x,  m_RenderScale.y));
}

void CRenderComponent::DeregisterFromRenderDomain()
{
    if (!GetMesh() || !GetMaterial()) return;
    RenderMgr::GetInst()->RemoveGameObjectFromRenderDomain(m_Material->GetDomain(), GetOwner());
}

void CRenderComponent::SaveToLevelFile(FILE* _File)
{
    SaveAssetRef(_File, m_Mesh.Get());
    SaveAssetRef(_File, m_Material.Get());
    SaveAssetRef(_File, m_SharedMaterial.Get());
    
    fwrite(&m_RenderOffset, sizeof(Vec2), 1, _File);
    fwrite(&m_RenderScale, sizeof(Vec2), 1, _File);
}

void CRenderComponent::LoadFromLevelFile(FILE* _File)
{
    m_Mesh           = LoadAssetRef<AMesh>(_File);
    m_Material       = LoadAssetRef<AMaterial>(_File);
    m_SharedMaterial = LoadAssetRef<AMaterial>(_File);

    fread(&m_RenderOffset, sizeof(Vec2), 1, _File);
    fread(&m_RenderScale, sizeof(Vec2), 1, _File);
}

bool CRenderComponent::IsInViewRect(const Vec2& _ViewMin, const Vec2& _ViewMax)
{
    Vec2 vWorldPos   = ToVec2(Transform()->GetWorldPos());
    Vec2 vWorldScale = ToVec2(Transform()->GetWorldScale());

    // Render Offset과 Render Scale 적용
    // 최종 위치 = WorldPos + RenderOffset
    // 최종 크기 = WorldScale * RenderScale

    Vec2 vFinalPos   = vWorldPos + m_RenderOffset;
    Vec2 vFinalScale = vWorldScale * m_RenderScale;

    Vec2 vMin = vFinalPos - vFinalScale * 0.5f;
    Vec2 vMax = vFinalPos + vFinalScale * 0.5f;

    // AABB 충돌 체크
    if (_ViewMax.x < vMin.x || vMax.x < _ViewMin.x) return false;
    if (_ViewMax.y < vMin.y || vMax.y < _ViewMin.y) return false;

    return true;
}
