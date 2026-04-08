#include "pch.h"
#include "CTurretMGEffectPooler.h"

#include "Source/ScriptMgr.h"

CTurretMGEffectPooler::CTurretMGEffectPooler()
    : CScript(SCRIPT_TYPE::AIRSTRIKE)
{
}

CTurretMGEffectPooler::~CTurretMGEffectPooler()
{
}

void CTurretMGEffectPooler::Begin()
{
    CScript::Begin();
}

void CTurretMGEffectPooler::Tick()
{
}
