#include "pch.h"
#include "CVampire.h"

#include "Source/ScriptMgr.h"

CVampire::CVampire()
    : CEnemyScript(SCRIPT_TYPE::VAMPIRE)
{
}

CVampire::~CVampire()
{
}

void CVampire::Begin()
{
    CEnemyScript::Begin();
}

void CVampire::Tick()
{
}
