#include "pch.h"
#include "CTransform.h"

#include "GameEngine/02.Device/Device.h"
#include "GameEngine/02.Device/ConstBuffer/ConstBuffer.h"
#include "GameEngine/05.GameObject/GameObject.h"

CTransform::CTransform()
	: Component(COMPONENT_TYPE::TRANSFORM)
	, m_RelativeScale(Vec3(1.f, 1.f, 1.f))
	, m_Dir{}
	, m_IndependentScale(false)
{
	m_Dir[static_cast<UINT>(DIR::RIGHT)]	= Vec3(1.f, 0.f, 0.f); 
	m_Dir[static_cast<UINT>(DIR::UP)]		= Vec3(0.f, 1.f, 0.f); 
	m_Dir[static_cast<UINT>(DIR::FRONT)]	= Vec3(0.f, 0.f, 1.f); 
}

CTransform::~CTransform()
{
}

Vec3 CTransform::GetWorldScale() const
{
	Vec3 vWorldScale = m_RelativeScale;
	
	if (m_IndependentScale) return vWorldScale;
	
	Ptr<GameObject> pParent = GetOwner()->GetParent();
	
	while (pParent && pParent->Transform())
	{
		vWorldScale *= pParent->Transform()->GetRelativeScale();
		
		if (pParent->Transform()->m_IndependentScale) return vWorldScale;
		
		pParent = pParent->GetParent();	
	}
	
	return vWorldScale;
}

void CTransform::FinalTick()
{
	if (m_bUpdateZDepthToYCoordOnEveryTick)
		m_RelativePos.z = m_RelativePos.y;
	
	Matrix matPivot		= XMMatrixTranslation(m_Pivot.x, m_Pivot.y, m_Pivot.z);
	Matrix matInvPivot	= XMMatrixInverse(nullptr, matPivot);

	// SRT 순
	Matrix matTrans = XMMatrixTranslation(m_RelativePos.x, m_RelativePos.y, m_RelativePos.z);
	Matrix matScale = XMMatrixScaling(m_RelativeScale.x, m_RelativeScale.y, m_RelativeScale.z);
	Matrix matRot	= XMMatrixRotationRollPitchYaw(m_RelativeRot.x, m_RelativeRot.y, m_RelativeRot.z);
	
	m_MatWorld = matInvPivot * matScale * matRot * matTrans * matPivot;

	// 부모 오브젝트가 있었고, 부모 오브젝트의 TransformComponent가 있다면
	// 주의 (Transform GrandParent -> Non Transform Parent -> Transform Child (InValid)
	if (GetOwner()->GetParent() && GetOwner()->GetParent()->Transform())
	{
		// 부모 오브젝트 크기의 영향을 받지 않겠다.
		if (m_IndependentScale)
		{
			Vec3 ParentScale			= GetOwner()->GetParent()->Transform()->GetWorldScale();
			Matrix matParentScale		= XMMatrixScaling(ParentScale.x, ParentScale.y, ParentScale.z);
			Matrix matParentScaleInv	= XMMatrixInverse(nullptr, matParentScale); // 주의 : 부모의 Scale.Z가 모두 0보다는 커야 한다.
			
			m_MatWorld *= matParentScaleInv * GetOwner()->GetParent()->Transform()->GetWorldMatrix();
		}
		else m_MatWorld *= GetOwner()->GetParent()->Transform()->GetWorldMatrix();
	}
		
	// 최종 World Transform에서 방향 벡터 계산
	m_Dir[static_cast<UINT>(DIR::RIGHT)]	= m_MatWorld.Right(); 
	m_Dir[static_cast<UINT>(DIR::UP)]		= m_MatWorld.Up(); 
	m_Dir[static_cast<UINT>(DIR::FRONT)]	= m_MatWorld.Front();
	
	m_Dir[static_cast<UINT>(DIR::RIGHT)].Normalize();
	m_Dir[static_cast<UINT>(DIR::UP)].Normalize();
	m_Dir[static_cast<UINT>(DIR::FRONT)].Normalize();
	
}

void CTransform::Binding()
{
	g_Trans.matWorld = m_MatWorld;
	
	// 전역변수에 들어있는 오브젝트 위치 정보를 상수버퍼로 복사
	Device::GetInst()->GetCB(CB_TYPE::TRANSFORM)->SetData(&g_Trans);
	Device::GetInst()->GetCB(CB_TYPE::TRANSFORM)->Binding();
}

void CTransform::SaveToLevelFile(FILE* _File)
{
	fwrite(&m_bUpdateZDepthToYCoordOnEveryTick, sizeof(bool), 1, _File);
	fwrite(&m_RelativePos  		, sizeof(Vec3), 1, _File);
	fwrite(&m_RelativeScale		, sizeof(Vec3), 1, _File);
	fwrite(&m_RelativeRot  		, sizeof(Vec3), 1, _File);
	fwrite(&m_Pivot				, sizeof(Vec3), 1, _File);
	fwrite(&m_IndependentScale  , sizeof(bool), 1, _File);
}

void CTransform::LoadFromLevelFile(FILE* _File)
{
	fread(&m_bUpdateZDepthToYCoordOnEveryTick, sizeof(bool), 1, _File);
	fread(m_RelativePos     	, sizeof(Vec3), 1, _File);
	fread(m_RelativeScale   	, sizeof(Vec3), 1, _File);
	fread(m_RelativeRot     	, sizeof(Vec3), 1, _File);
	fread(&m_Pivot				, sizeof(Vec3), 1, _File);
	fread(&m_IndependentScale	, sizeof(bool), 1, _File);
}

void CTransform::SetRelativePosFromWorldPos(const Vec3& _DesiredWorldPos)
{
	m_RelativePos = CalculateRelativePosFromWorldPos(_DesiredWorldPos);
}

Vec3 CTransform::CalculateRelativePosFromWorldPos(const Vec3& _DesiredWorldPos)
{
	if (!GetOwner()->GetParent() || !GetOwner()->GetParent()->Transform()) return _DesiredWorldPos;

	CTransform* ParentTransform = GetOwner()->GetParent()->Transform().Get();
	
	Matrix ParentEffect = ParentTransform->GetWorldMatrix();
	
	if (m_IndependentScale)
	{
		Vec3 ParentScale = ParentTransform->GetWorldScale();
		Matrix invParentScale = XMMatrixScaling
		(
		   (fabsf(ParentScale.x) > FLT_EPSILON) ? 1.f / ParentScale.x : 0.f,
		   (fabsf(ParentScale.y) > FLT_EPSILON) ? 1.f / ParentScale.y : 0.f,
		   (fabsf(ParentScale.z) > FLT_EPSILON) ? 1.f / ParentScale.z : 0.f
		);
		ParentEffect = invParentScale * ParentEffect;
	}
	
	// 목표 월드좌표 -> 부모공간 좌표
	Matrix invParent = XMMatrixInverse(nullptr, ParentEffect);
	Vec3 TargetInParent = Vec3::Transform(_DesiredWorldPos, invParent);
	
	// local에서 translation만 제외한 오프셋 계산 : invPivot * S * R * pivot

	Matrix MatPivot    = XMMatrixTranslation(m_Pivot.x, m_Pivot.y, m_Pivot.z);
	Matrix MatInvPivot = XMMatrixInverse(nullptr, MatPivot);
	Matrix NoTrans     = MatInvPivot
					   * XMMatrixScaling(m_RelativeScale.x, m_RelativeScale.y, m_RelativeScale.z)
					   * XMMatrixRotationRollPitchYaw(m_RelativeRot.x, m_RelativeRot.y, m_RelativeRot.z)
					   * MatPivot;

	Vec3 OriginOffsetInParent = Vec3::Transform(Vec3::Zero, NoTrans);
	return TargetInParent - OriginOffsetInParent;
}
