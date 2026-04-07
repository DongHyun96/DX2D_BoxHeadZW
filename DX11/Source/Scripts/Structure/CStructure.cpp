#include "pch.h"
#include "CStructure.h"

#include "Source/ScriptMgr.h"
#include "Source/Scripts/CharacterScript/CCharacterScript.h"
#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"

set<CellCoord> CStructure::s_InstalledCoord{};

CStructure::CStructure()
    : CScript(SCRIPT_TYPE::STRUCTURE)
{
}

CStructure::CStructure(const CStructure& _Origin)
    : CScript(_Origin)
    , m_InstallSound(_Origin.m_InstallSound)
    , m_IsPreviewObject(_Origin.m_IsPreviewObject)
{
}

CStructure::~CStructure()
{
}

CStructure::CStructure(SCRIPT_TYPE _ScriptType)
    : CScript(_ScriptType)
{
}

void CStructure::Init()
{
    AddScriptParam(SCRIPT_PARAM::SOUND, &m_InstallSound, L"InstallSound");
}

void CStructure::Begin()
{
    ADD_DYNAMIC_BEGIN_OVERLAP(CStructure::BodyColliderBeginOverlap);
    ADD_DYNAMIC_OVERLAP(CStructure::BodyColliderOverlap);
}

void CStructure::Tick()
{
}

void CStructure::BodyColliderBeginOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider)
{
    // Character Movement로 해당 Collider에 부딪힌 경우
    if (BlockCharacterCollider(_OtherCollider)) return;
    
    // 나머지는 Enemy Attack Area -> TakeDamage 처리는 각자의 Projectile 및 AttackArea Overlap에서 처리할 것
}

void CStructure::BodyColliderOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider)
{
    if (BlockCharacterCollider(_OtherCollider)) return;
}

bool CStructure::BlockCharacterCollider(CCollider2D* _OtherCollider)
{
    // 프리뷰 오브젝트에 캐릭터가 blocking되면 안됨
    if (m_IsPreviewObject) return false;
    
    // 캐릭터가 아닌 다른 GameObject
    if (!_OtherCollider->GetOwner()->GetScriptComponent<CCharacterScript>()) return false;

    _OtherCollider->Transform()->UpdateTransformToPrevRelativePos();
    return true;
}

void CStructure::SaveToLevelFile(FILE* _File)
{
    SaveAssetRef(_File, m_InstallSound.Get());
}

void CStructure::LoadFromLevelFile(FILE* _File)
{
    m_InstallSound = LoadAssetRef<ASound>(_File).Get();
}

