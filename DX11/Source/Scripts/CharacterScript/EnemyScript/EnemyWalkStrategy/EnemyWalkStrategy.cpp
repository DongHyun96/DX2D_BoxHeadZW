#include "pch.h"
#include "EnemyWalkStrategy.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "Source/AStar/AStarPathFinder.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/BackgroundTile/CBackgroundTile.h"
#include "Source/Scripts/CharacterScript/EnemyScript/CEnemyScript.h"
#include "Source/Scripts/StatScript/CStatScript.h"
#include "Source/Scripts/Structure/CStructure.h"


GameObject* EnemyWalkStrategy::FindNearestTargetFromAllObjects(CEnemyScript* _Enemy)
{
    const Vec3 EnemyPos     = _Enemy->Transform()->GetRelativePos();
    const Vec2 PlayerPos    = GM->GetPlayerObject()->Transform()->GetWorldPos2D();
        
    float MinDist               = Vec2::DistanceSquared(ToVec2(EnemyPos), PlayerPos);
    GameObject* TargetSelected  = GM->GetPlayerObject();
        
    // Player 위치, CStructure의 위치 중 가장 근접한 위치를 찾음
    for (CStructure* Structure : CStructure::GetInstalledStructures())
    {
        const Vec2 StructurePos = Structure->Transform()->GetWorldPos2D();
        float CurDist = Vec2::DistanceSquared(StructurePos, ToVec2(EnemyPos));
            
        if (CurDist < MinDist)
        {
            MinDist = CurDist;
            TargetSelected = Structure->GetOwner();
        }
    }
    
    return TargetSelected;
}

void EnemyWalkThroughCellPathStrategy::UseWalkStrategy(CEnemyScript* _Enemy)
{
    Vec3 EnemyPos = _Enemy->Transform()->GetRelativePos();
    
    // 이미 해당 이동방법으로 이동을 끝낸 상황이거나, 이동하려했던 Target이 죽은 상황
    if (_Enemy->m_CellPath.empty() || !IsValid(_Enemy->m_TargetObject))
    {
        // 새로운 Target 찾기
        GameObject* TargetSelected = FindNearestTargetFromAllObjects(_Enemy);
        if (!TargetSelected) return; // 맵에 고를 Target이 없는 상황

        // Target을 향한 새로운 경로 지정
        const CellCoord CurrentCellCoord    = GM->GetBackgroundCellManager()->GetWorldPosToCellCoord(ToVec2(EnemyPos));
        const CellCoord destCellCoord       = GM->GetBackgroundCellManager()->GetWorldPosToCellCoord(TargetSelected->Transform()->GetWorldPos2D());
        
        AStarPathFinder::GetInst()->GetPath(CurrentCellCoord, destCellCoord, _Enemy->m_CellPath);
        
        // Target 지정
        _Enemy->m_TargetObject = TargetSelected;
    }

    // 새로운 경로를 받았음에도 empty일 경우가 있음 (이때는 처리 x)
    // 플레이어가 죽었고, 설치물이 모두 파괴되었을 때 여기로 들어옴 (거의 들어올 일 없음)
    if (_Enemy->m_CellPath.empty())
    {
        return;
    }
    
    _Enemy->m_Velocity = Vec3::Zero;
    
    const Vec2 Destination = GM->GetBackgroundCellManager()->GetCellCoordToWorldPos(_Enemy->m_CellPath.top()); 
    const Vec2 Direction = Destination - ToVec2(EnemyPos);
    
    const float DistToDest = Direction.Length();
    const float MoveDistThisFrame = _Enemy->m_MoveSpeedBase * _Enemy->m_MoveSpeedFactor * DT;
    
    // 이번 이동 거리보다 남은 거리가 작다면 도착한 것으로 판정
    if (DistToDest <= MoveDistThisFrame) 
    {
        _Enemy->Transform()->SetRelativePos(ToVec3(Destination, Destination.y)); // Dest로 위치보정 처리
        _Enemy->m_CellPath.pop(); 
        return;        
    }
    
    _Enemy->m_Velocity = ToVec3(Direction / (DistToDest == 0.f ? FLT_EPSILON : DistToDest) ) *  _Enemy->m_MoveSpeedBase * _Enemy->m_MoveSpeedFactor;
    EnemyPos += _Enemy->m_Velocity * DT;
    
    _Enemy->Transform()->SetRelativePos(EnemyPos);
}

void EnemyWalkStraightStrategy::UseWalkStrategy(CEnemyScript* _Enemy)
{
    // Target Object가 valid하지 않은 상황, 새로운 TargetObject 찾아서 지정
    if (!IsValid(_Enemy->m_TargetObject))
    {
        GameObject* TargetSelected = FindNearestTargetFromAllObjects(_Enemy); // 여기를 사실, 전체 물체탐색을 할게 아니라, perception에 들어온 Target에 대한 setting으로 변경처리를 해주어야 더 좋을 듯 -> 이걸 한 번 처리를 함
        _Enemy->m_TargetObject = TargetSelected;
    }

    Vec2 EnemyPos = _Enemy->Transform()->GetRelativePosXY();
    _Enemy->m_Velocity = Vec3::Zero;
    
    Vec2 Direction = _Enemy->m_TargetObject->Transform()->GetRelativePosXY() - EnemyPos;
    Direction.Normalize();
    
    _Enemy->m_Velocity = ToVec3(Direction * _Enemy->m_MoveSpeedBase * _Enemy->m_MoveSpeedFactor);
    
    const Vec3 NewPos = _Enemy->Transform()->GetRelativePos() + _Enemy->m_Velocity * DT;
    _Enemy->Transform()->SetRelativePos(NewPos);
}
