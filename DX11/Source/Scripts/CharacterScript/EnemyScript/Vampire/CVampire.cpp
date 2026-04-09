#include "pch.h"
#include "CVampire.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/BackgroundTile/CBackgroundTile.h"
#include "Source/Scripts/StatScript/CStatScript.h"


const float CVampire::s_SelfHealStartTime   = 4.f;
const float CVampire::s_HealPerSec          = 40.f;
const Vec2 CVampire::s_TeleportStartTimeMinMax = {3.f, 10.f};

CVampire::CVampire()
    : CEnemyScript(SCRIPT_TYPE::VAMPIRE)
    , m_TeleportStartTimeLimit(GetRandom(s_TeleportStartTimeMinMax.x, s_TeleportStartTimeMinMax.y))
{
}

CVampire::~CVampire()
{
}

void CVampire::Begin()
{
    CEnemyScript::Begin();
    m_Stat = GetOwner()->GetScriptComponent<CStatScript>().Get();
    m_EnemyScript = GetOwner()->GetScriptComponent<CEnemyScript>().Get();
}

void CVampire::Tick()
{
    CEnemyScript::Tick();
    HandleSelfHeal();
    HandleTeleport();
}

void CVampire::OnTakeDamage(GameObject* _DamageCauser)
{
    // Damage Timer 초기화
    m_AfterLastDamagedTimer = 0.f;
}

void CVampire::SetCurrentWalkType(ENEMY_WALK_TYPE _WalkType)
{
    CEnemyScript::SetCurrentWalkType(_WalkType);
    
    // 여기 들어왔다는 건 이미 Walk 상태일 때임
    if (GetCurrentWalkType() == ENEMY_WALK_TYPE::CELL_PATH)
        InitTeleportTimer();
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

void CVampire::HandleTeleport()
{
    if (m_EnemyScript->GetMainState() != ENEMY_MAINSTATE::WALK || m_EnemyScript->GetCurrentWalkType() != ENEMY_WALK_TYPE::CELL_PATH) return;
    
    // Walk 상태이고, CellPath Type Walk 상태 지속 시간 체크
    m_TeleportTimer += DT;
    
    if (m_TeleportTimer > m_TeleportStartTimeLimit)
    {
        // 50%의 확률로 Teleport 처리
        if (GetRandom(0.f, 1.f) < 0.5f) // 여기는 실패 -> 다시 돌아가기
        {
            // 다시 Teleport 시간 측정
            InitTeleportTimer();
            return;
        }

        const Vec2 PlayerPos = GM->GetPlayerObject()->Transform()->GetRelativePosXY();
        const CellCoord PlayerCellCoord = GM->GetBackgroundCellManager()->GetWorldPosToCellCoord(PlayerPos);
        
        // Player 와 완전히 인접한 Cell은 거르고(해당 위치 + 8칸 = 9칸) 도넛 모양으로 사각형의 영역안의 Available한 Cell 영역에 Teleport 처리
        vector<CellCoord> AvailableCells{};

        for (int y = -8; y < 8; ++y)
        {
            if (-1 <= y && y <= 1) continue;
            for (int x = -8; x < 8; ++x)
            {
                if (-1 <= x && x <= 1) continue;
                const CellCoord CurCell = PlayerCellCoord + CellCoord(x, y);
                if (GM->GetBackgroundCellManager()->IsCellAvailable(CurCell))
                    AvailableCells.push_back(CurCell);
            }
        }

        // Available한 Cell이 없으면 다시 Teleport 시간 측정
        if (AvailableCells.empty())
        {
            // 다시 Teleport 시간 측정
            InitTeleportTimer();
            return;
        }

        // 실제로 Teleport 처리
        int RandIdx = GetRandom(0, static_cast<int>(AvailableCells.size() - 1));
        const Vec2 TeleportPos = GM->GetBackgroundCellManager()->GetCellCoordToWorldPos(AvailableCells[RandIdx]);
        Transform()->SetRelativePos(ToVec3(TeleportPos, TeleportPos.y));
        
        // TargetObject 다시 재설정 처리
        m_EnemyScript->SetTargetObject(nullptr);
        
        // TODO : Teleport effect & sound
        
        // 다시 Teleport 시간 측정
        InitTeleportTimer();
        return;        
    }
            
}
