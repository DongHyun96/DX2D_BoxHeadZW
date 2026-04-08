#include "pch.h"
#include "CMummy.h"

#include "Source/ScriptMgr.h"

CMummy::CMummy()
    : CEnemyScript(SCRIPT_TYPE::MUMMY)
{
}

CMummy::~CMummy()
{
}

void CMummy::Begin()
{
    CEnemyScript::Begin();
}

void CMummy::Tick()
{
    CEnemyScript::Tick();
}
