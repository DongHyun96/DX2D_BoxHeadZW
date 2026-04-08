#include "pch.h"
#include "CVampire.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "Source/ScriptMgr.h"
#include "Source/Scripts/StatScript/CStatScript.h"


const float CVampire::s_SelfHealStartTime   = 4.f;
const float CVampire::s_HealPerSec          = 40.f;

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
    m_Stat = GetOwner()->GetScriptComponent<CStatScript>().Get();
}

void CVampire::Tick()
{
    CEnemyScript::Tick();
    HandleSelfHeal();
    
    DebugUtil::SetPermanentDebugLog("Vampire HP", "Vamp HP : " + to_string(m_Stat->GetHP()), DEF_COLOR_GREEN);
}

void CVampire::OnTakeDamage()
{
    // Damage Timer 초기화
    m_AfterLastDamagedTimer = 0.f;
}

void CVampire::HandleSelfHeal()
{
    m_AfterLastDamagedTimer += DT;
    
    if (m_AfterLastDamagedTimer > s_SelfHealStartTime) // 자가 치유 가능 시간
    {
        m_AfterLastDamagedTimer = s_SelfHealStartTime + 1.f;
        
        // 자가 치유가 필요하면 처리
        if (!m_Stat->IsDead() && !m_Stat->IsFullHP())
            m_Stat->ApplyHeal(DT * s_HealPerSec);
    }
}
