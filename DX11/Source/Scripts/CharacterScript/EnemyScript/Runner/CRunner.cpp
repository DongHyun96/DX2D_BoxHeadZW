#include "pch.h"
#include "CRunner.h"

#include "Source/ScriptMgr.h"

CRunner::CRunner()
    : CEnemyScript(SCRIPT_TYPE::RUNNER)
{
    m_AttackFlipbookCount = 16;
}

CRunner::~CRunner()
{
}

void CRunner::Begin()
{
    CEnemyScript::Begin();
}

void CRunner::Tick()
{
    CEnemyScript::Tick();
}
