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
}
