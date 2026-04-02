#include "pch.h"
#include "CCharacterStat.h"

#include "Source/ScriptMgr.h"
#include "Source/Scripts/CharacterScript/CCharacterAnimHandler.h"
#include "Source/Scripts/CharacterScript/CCharacterScript.h"

CCharacterStat::CCharacterStat()
    : CStatScript(SCRIPT_TYPE::CHARACTERSTAT)
{
}

CCharacterStat::~CCharacterStat()
{
}

CCharacterStat::CCharacterStat(SCRIPT_TYPE _ScriptType)
    : CStatScript(_ScriptType)
{
}

bool CCharacterStat::TakeDamage(float _DamageAmount, const Vec2& _DamageSourcePos)
{
    if (!CStatScript::TakeDamage(_DamageAmount, _DamageSourcePos)) return false;
    
    // PushedOut 이후 Dead 처리가 이루어져야 함
    if (const Ptr<CCharacterScript>& MainCharacterScript = GetOwner()->GetScriptComponent<CCharacterScript>())
    {
        Vec2 PushedOutFaceDirection = _DamageSourcePos - ToVec2(Transform()->GetWorldPos());
        PushedOutFaceDirection.Normalize();
        
        // PushedOut의 경우, PushedOut Animation(정확히는 밀린 장면 Stop 시간 얼마나 보여줄지) time rewind 처리를 여기서 해주어야 한다
        MainCharacterScript->RewindPushedOut(PushedOutFaceDirection);
        GetOwner()->GetScriptComponent<CCharacterAnimHandler>()->RewindPushedOut(PushedOutFaceDirection);
    }
    
    return true;
}
