#include "pch.h"
#include "CCharacterScript.h"

#include "Source/ScriptMgr.h"

CCharacterScript::CCharacterScript()
    : CScript(SCRIPT_TYPE::CHARACTERSCRIPT)
{
}

CCharacterScript::CCharacterScript(int _ScriptType)
    : CScript(_ScriptType)
{
}

CCharacterScript::~CCharacterScript()
{
}

void CCharacterScript::Tick()
{
    Move();
}
