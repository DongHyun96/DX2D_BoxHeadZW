#include "pch.h"
#include "CMonsterScript.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/05.GameObject/GameObject.h"
#include "GameEngine/06.Component/03.Collider2D/CCollider2D.h"
#include "Source/ScriptMgr.h"

CMonsterScript::CMonsterScript()
    : CScript(static_cast<int>(SCRIPT_TYPE::MONSTERSCRIPT))
{
}

CMonsterScript::~CMonsterScript()
{
}

void CMonsterScript::Begin()
{
    ADD_DYNAMIC_BEGIN_OVERLAP(CMonsterScript::BeginOverlap);
    ADD_DYNAMIC_OVERLAP(CMonsterScript::Overlap);
    ADD_DYNAMIC_END_OVERLAP(CMonsterScript::EndOverlap);
}

void CMonsterScript::Tick()
{
}

void CMonsterScript::BeginOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider)
{
    if (!GetOwner()->GetRenderCom() || !GetOwner()->GetRenderCom()->GetMaterial())
        return;

    // 가리키고 있던 원본 재질을 복사생성해서 자신만의 재질을 만들고, 그것을 반환
    Ptr<AMaterial> pDynamicMtrl = GetOwner()->GetRenderCom()->CreateDynamicMaterial();
    pDynamicMtrl->SetScalar(VEC4_0, Vec4(2.f, 1.f, 1.f, 1.f));
}

void CMonsterScript::Overlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider)
{
    if (KEY_TAP(KEY::C))
        Destroy();
}

void CMonsterScript::EndOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider)
{
    if (!GetOwner()->GetRenderCom() || !GetOwner()->GetRenderCom()->GetMaterial())
        return;

    // 현재 사용중인 
    Ptr<AMaterial> pMtrl = GetOwner()->GetRenderCom()->ReturnToSharedMaterial();
    pMtrl->SetScalar(VEC4_0, Vec4(1.f, 1.f, 1.f, 1.f));
}
