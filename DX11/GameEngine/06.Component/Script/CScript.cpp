#include "pch.h"
#include "CScript.h"

#include "GameEngine/04.Asset/09.Prefab/APrefab.h"
#include "GameEngine/03.Manager/07.TaskMgr/TaskMgr.h"
#include "GameEngine/05.GameObject/GameObject.h"

CScript::CScript(int _ScriptType)
    : Component(COMPONENT_TYPE::SCRIPT)
    , m_ScriptType(_ScriptType)
{
}

CScript::CScript(SCRIPT_TYPE _ScriptType)
    : Component(COMPONENT_TYPE::SCRIPT)
    , m_ScriptType(static_cast<int>(_ScriptType))
{
}

CScript::CScript(const CScript& _Origin)
    : Component(_Origin)
    , m_ScriptType(_Origin.m_ScriptType)
{
}

CScript::~CScript()
{
}

void CScript::Destroy()
{
    GetOwner()->Destroy();
}
