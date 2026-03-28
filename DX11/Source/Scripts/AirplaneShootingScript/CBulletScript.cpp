#include "pch.h"
#include "CBulletScript.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/05.GameObject/GameObject.h"
#include "Source/ScriptMgr.h"

CBulletScript::CBulletScript()
    : CScript(static_cast<int>(SCRIPT_TYPE::BULLETSCRIPT))
{
}

CBulletScript::~CBulletScript()
{
}

void CBulletScript::Begin()
{
    if (!GetCollider2D()) return;

    ADD_DYNAMIC_BEGIN_OVERLAP(CBulletScript::BeginOverlap);
    ADD_DYNAMIC_OVERLAP(CBulletScript::Overlap);
    ADD_DYNAMIC_END_OVERLAP(CBulletScript::EndOverlap);
    
    /*Collider2D()->AddDynamicBeginOverlap(this, static_cast<COLLISION_EVENT>(&CBulletScript::BeginOverlap));
    Collider2D()->AddDynamicOverlap     (this, static_cast<COLLISION_EVENT>(&CBulletScript::Overlap));
    Collider2D()->AddDynamicEndOverlap  (this, static_cast<COLLISION_EVENT>(&CBulletScript::EndOverlap));*/
}

void CBulletScript::Tick()
{
    if (!GetOwner()->GetActive()) return;

    m_TimeAfterSpawned += DT;
    
    // 추적 대상이 있다면
    if (IsValid(m_Target))
    {
        Vec3 vTargetPos = m_Target->Transform()->GetWorldPos();
        Vec3 vPos = Transform()->GetRelativePos();
        
        Vec3 vDirToTarget = vTargetPos - vPos;
        vDirToTarget.Normalize();
        
        vPos += vDirToTarget * 1000.f * DT;
        Transform()->SetRelativePos(vPos);
    }
    else
    {
        Vec3 Pos = Transform()->GetRelativePos();
    
        Pos += Transform()->GetDir(DIR::RIGHT) * m_Speed * DT;
        Transform()->SetRelativePos(Pos);
    }
    
    if (m_TimeAfterSpawned > 10.f)
    {
        m_TimeAfterSpawned = 0.f;
        GetOwner()->SetActive(false);
    }
    
    // Border check
    /*Vec2 Resol = Device::GetInst()->GetRenderResolution();
    if (Pos.x < -Resol.x || Pos.x > Resol.x || Pos.y < -Resol.y || Pos.y > Resol.y)
        m_Owner->SetIsActive(false);*/
}

void CBulletScript::BeginOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider)
{
    GetCollider2D()->SetColor(Vec4(1.f, 0.f, 0.f, 1.f));
    GetOwner()->SetActive(false);
}

void CBulletScript::Overlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider)
{
}

void CBulletScript::EndOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider)
{
    GetCollider2D()->SetColor(Vec4(0.f, 1.f, 0.f, 1.f));
}

void CBulletScript::SaveToLevelFile(FILE* _File)
{
    fwrite(&m_Speed, sizeof(float), 1, _File);
}

void CBulletScript::LoadFromLevelFile(FILE* _File)
{
    fread(&m_Speed, sizeof(float), 1, _File);
}

bool CBulletScript::Fire(const Vec3& _StartPosition, const Vec3& _FireDirection, float _Speed)
{
    if (GetOwner()->GetActive()) return false;

    GetOwner()->SetActive(true);
    Transform()->SetRelativePos(_StartPosition);

    float zRot = atan2f(_FireDirection.y, _FireDirection.x);
    Transform()->SetRelativeRot(Vec3(0.f, 0.f, zRot));
    
    m_Speed = _Speed;
    m_TimeAfterSpawned = 0.f;
    return true;
}
