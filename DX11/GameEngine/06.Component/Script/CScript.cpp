#include "pch.h"
#include "CScript.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/04.Asset/09.Prefab/APrefab.h"
#include "GameEngine/03.Manager/07.TaskMgr/TaskMgr.h"
#include "GameEngine/04.Asset/04.Level/ALevel.h"
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

bool CScript::RegisterEditingTickEnabled()
{
    // 만일 현재 Level이 Owner의 Level이고, Stop 상황이 아니라면 EditingTickEnabled 처리 x
    if (LevelMgr::GetInst()->GetCurLevel() == GetOwner()->GetOwnerLevel())
        if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::STOP) return false;
    
    m_bUseEditingTick = true;
    
    GetOwner()->GetOwnerLevel()->AddEditingTickEnabledGameObject(GetOwner());
    
    // DT Context
    GetOwner()->SetDTContextType(DT_CONTEXT_TYPE::IMPLICIT_ENGINE_DT);
    return true;
}

void CScript::DeRegisterEditingTickEnabled()
{
    m_bUseEditingTick = false;
    
    GetOwner()->GetOwnerLevel()->RemoveEditingTickEnabledGameObject(GetOwner());
    
    // DT Context default로 처리
    GetOwner()->SetDTContextType(DT_CONTEXT_TYPE::DEFAULT);
}

void CScript::RequestLevelToRetrySave()
{
    if (!GetOwner() || !GetOwner()->GetOwnerLevel()) return;
    GetOwner()->GetOwnerLevel()->RequestRetrySave();
}

void CScript::Destroy()
{
    GetOwner()->Destroy();
}
