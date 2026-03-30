#include "pch.h"
#include "CCharacterScript.h"

#include "Source/ScriptMgr.h"

CCharacterScript::CCharacterScript(enum SCRIPT_TYPE _ScriptType)
    : CScript(static_cast<int>(_ScriptType))
{
}

CCharacterScript::~CCharacterScript()
{
}

void CCharacterScript::Tick()
{
    Move();
    UpdateCurrentFacedDirection(); // TODO -> 피격중일 때에는 피격당하는 방향을 계속해서 바라보게끔 처리를 해주어야 함
}
