#include "pch.h"
#include "CMummy.h"

#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/BackgroundTile/CBackgroundTile.h"
#include "Source/Scripts/CharacterScript/EnemyScript/EnemySpawnHandler/CEnemySpawnHandler.h"
#include "Source/Scripts/StatScript/CStatScript.h"

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
    
    m_Stat = GetOwner()->GetScriptComponent<CStatScript>().Get();
    
}

void CMummy::Tick()
{
    CEnemyScript::Tick();
}

void CMummy::OnFadeOutEnd()
{
    if (m_SpawnedByMummy)
    {
        m_SpawnedByMummy = false; // Mummy한테서 태어난 다른 Mummy는 죽기 직전에 여기서 마킹을 풀어주어야 정상적으로 동작함
        return;
    }
    
    // 주위에 다른 Mummy Spawn 처리
    
    
    // Available Cell 위치에 스폰 처리
    // 자기 자신 위치 x
    const Vec2 PlayerPos            = GM->GetPlayerObject()->Transform()->GetRelativePosXY();
    const CellCoord PlayerCellCoord = GM->GetBackgroundCellManager()->GetWorldPosToCellCoord(PlayerPos);
    const CellCoord MyCellCoord     = GM->GetBackgroundCellManager()->GetWorldPosToCellCoord(Transform()->GetRelativePosXY());
        
    vector<CellCoord> AvailableCells{};

    static const int Range = 2;
    for (int y = -Range; y < Range; ++y)
    {
        for (int x = -Range; x < Range; ++x)
        {
            if (x == 0 && y == 0) continue;
            
            const CellCoord CurCell = MyCellCoord + CellCoord(x, y);
            if (CurCell == PlayerCellCoord) continue;
            if (!GM->GetBackgroundCellManager()->IsCellAvailable(CurCell)) continue;
            
            AvailableCells.push_back(CurCell);
        }
    }
    
    if (AvailableCells.empty()) return;
    
    static thread_local mt19937 gen{ random_device{}() };
    shuffle(AvailableCells.begin(), AvailableCells.end(), gen);
    
    for (int i = 0; i < GetRandom(1, 2); ++i)
    {
        // PoolCount 때문에 제대로 Spawn 안될 수도 있음
        if (GameObject* Object = GM->GetEnemySpawnHandler()->SpawnEnemy(m_EnemyType, AvailableCells[i]))
        {
            Object->GetScriptComponent<CMummy>()->m_SpawnedByMummy = true;
            
        }
    }
        
}

