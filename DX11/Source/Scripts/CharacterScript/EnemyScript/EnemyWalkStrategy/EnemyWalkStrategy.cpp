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

GameObject* EnemyWalkStrategy::GetRandomTargetFromAllObjects(CEnemyScript* _Enemy)
{
    if (GetRandom(0.f, 1.f) < 0.4f) return GM->GetPlayerObject();
    
    if (CStructure::GetInstalledStructures().empty()) return nullptr;
    if (CStructure* script = CStructure::GetInstalledStructures().getRandom())
    {
        return script->GetOwner();
    }
}

void EnemyWalkThroughCellPathStrategy::UseWalkStrategy(CEnemyScript* _Enemy)
{
    _Enemy->m_Velocity = Vec3::Zero;
    
    CBackgroundTile* const BackgroundCellManager = GM->GetBackgroundCellManager();
    Vec3 EnemyPos = _Enemy->Transform()->GetRelativePos();
    
    // 이미 해당 이동방법으로 이동을 끝낸 상황이거나, 이동하려했던 Target이 죽은 상황
    if (_Enemy->m_CellPath.empty() || !IsValid(_Enemy->m_TargetObject))
    {
        _Enemy->m_PathReplanTimer += DT;
        
        if (_Enemy->m_PathReplanTimer < _Enemy->m_PathReplanInterval) return;
        _Enemy->m_PathReplanTimer = 0.f;
        
        // 새로운 Target 찾기
        GameObject* TargetSelected = FindNearestTargetFromAllObjects(_Enemy); // TODO : 이거 쓰지 않기 -> 여기서 성능이 많이 잡아먹는 것 같음
        // GameObject* TargetSelected = GetRandomTargetFromAllObjects(_Enemy);
        if (!TargetSelected) return; // 맵에 고를 Target이 없는 상황

        // Target을 향한 새로운 경로 지정
        const CellCoord CurrentCellCoord = BackgroundCellManager->GetWorldPosToCellCoord(ToVec2(EnemyPos));
        
        // DestCoord의 경우, AvailableCell이 아니라면 -> Adjacent한 Cell로 다시금 조정(Structure 위치의 경우, 그 자체가 Available하지 않음)
        const CellCoord ExactDestCoord = BackgroundCellManager->GetWorldPosToCellCoord(TargetSelected->Transform()->GetWorldPos2D());
        
        // Structure 자체의 경우, AvailableCell이 아니기 때문에 인접한 Cell로 잡아본다
        const CellCoord destCellCoord = (TargetSelected == GM->GetPlayerObject()) ? ExactDestCoord : ExactDestCoord.GetRandomAdjacentCellCoord(); 
        
        if (CurrentCellCoord != destCellCoord && GM->GetBackgroundCellManager()->IsCellAvailable(destCellCoord))
            AStarPathFinder::GetInst()->GetPath(CurrentCellCoord, destCellCoord, _Enemy->m_CellPath);
        
        // Target 지정
        _Enemy->m_TargetObject = TargetSelected;
        _Enemy->m_PathReplanTimer = _Enemy->m_PathReplanInterval;
    }

    // 새로운 경로를 받았음에도 empty일 경우가 있음 (이때는 처리 x)
    if (_Enemy->m_CellPath.empty()) return;
    
    const Vec2 Destination = BackgroundCellManager->GetCellCoordToWorldPos(_Enemy->m_CellPath.top()); 
    const Vec2 Direction = Destination - ToVec2(EnemyPos);
    
    const float DistToDest = Direction.Length();
    const float MoveDistThisFrame = _Enemy->m_MoveSpeedBase * _Enemy->m_MoveSpeedFactor * DT;
    
    // 이번 이동 거리보다 남은 거리가 작다면 도착한 것으로 판정
    if (DistToDest <= MoveDistThisFrame) 
    {
        _Enemy->m_Velocity = ToVec3(Direction * MoveDistThisFrame); // 멈춰보이지 않게끔 Dummy Velocity 부여 (AnimHandler에서 Velocity 길이 0이면 멈춘 모션이 나와버림)
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
        GameObject* TargetSelected = GetRandomTargetFromAllObjects(_Enemy); // 여기를 사실, 전체 물체탐색을 할게 아니라, perception에 들어온 Target에 대한 setting으로 변경처리를 해주어야 더 좋을 듯 -> 이걸 한 번 처리를 함
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
