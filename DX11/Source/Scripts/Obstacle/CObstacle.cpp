#include "pch.h"
#include "CObstacle.h"

#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "Source/ScriptMgr.h"

CObstacle::CObstacle()
    : CScript(SCRIPT_TYPE::OBSTACLE)
{
}

CObstacle::~CObstacle()
{
}

void CObstacle::Begin()
{
    // Obstacle의 경우, z값은 y값으로 고정처리되어 계속 이어진다 (BeginPlay시에 z값 한 번 설정하고 더 이상 건드릴 필요 없다)
    Transform()->SetRelativePosZ(Transform()->GetRelativePosY());
    
    ADD_DYNAMIC_BEGIN_OVERLAP(CObstacle::BeginOverlap);
    ADD_DYNAMIC_OVERLAP(CObstacle::Overlap);
    ADD_DYNAMIC_END_OVERLAP(CObstacle::EndOverlap);
}

void CObstacle::Tick()
{
}

void CObstacle::SaveToLevelFile(FILE* _File)
{
}

void CObstacle::LoadFromLevelFile(FILE* _File)
{
}

void CObstacle::BeginOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider)
{
    const ALevel* CurLevel = LevelMgr::GetInst()->GetCurLevel().Get(); 
    if (_OtherCollider->GetOwner()->GetLayerIdx() == CurLevel->GetLayerIndexByLayerName(L"Character"))
    {
        DebugUtil::AddDebugLog(_OtherCollider->GetOwner()->GetName() + L" BeginOverlap");
        // Character의 경우, blocking 처리
        _OtherCollider->Transform()->UpdateTransformToPrevRelativePos();
    }
}

void CObstacle::Overlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider)
{
    /*wstring temp = _OtherCollider->GetOwner()->GetName();
    DebugUtil::SetPermanentDebugLog("TempKey", string(temp.begin(), temp.end()).c_str(), Vec4(1.f, 1.f, 0.f ,1.f));*/
    const ALevel* CurLevel = LevelMgr::GetInst()->GetCurLevel().Get(); 
    if (_OtherCollider->GetOwner()->GetLayerIdx() == CurLevel->GetLayerIndexByLayerName(L"Character"))
    {
        DebugUtil::AddDebugLog(_OtherCollider->GetOwner()->GetName() + L" BeginOverlap");
        // Character의 경우, blocking 처리
        _OtherCollider->Transform()->UpdateTransformToPrevRelativePos();
    }
}

void CObstacle::EndOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider)
{
    DebugUtil::AddDebugLog(_OtherCollider->GetOwner()->GetName() + L" EndOverlap");    
}
