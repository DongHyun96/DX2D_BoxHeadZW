#include "pch.h"
#include "EnemyWalkStrategy.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "Source/AStar/AStarPathFinder.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/BackgroundTile/CBackgroundTile.h"
#include "Source/Scripts/CharacterScript/EnemyScript/CEnemyScript.h"
#include "Source/Scripts/StatScript/CStatScript.h"
#include "Source/Scripts/Structure/CStructure.h"
#include "GameEngine/03.Manager/08.CollisionMgr/CollisionMgr.h"


GameObject* EnemyWalkStrategy::FindNearestTargetFromAllObjects(CEnemyScript* _Enemy)
{
    const Vec2 EnemyPos = _Enemy->Transform()->GetWorldPos2D();

    // Player 레이어와 Structure 레이어를 포함한 마스크 생성
    UINT targetMask = 0;

    // Player
    GameObject* pPlayer = GM->GetPlayerObject();
    if (pPlayer) targetMask |= (1 << pPlayer->GetLayerIdx());

    // Structure
    const RandomizedSet<CStructure*>& structures = CStructure::GetInstalledStructures();
    if (!structures.empty())
    {
        // 첫 번째 구조물을 통해 해당 레이어를 알아냄
        CStructure* pFirst = *structures.begin();
        if (pFirst && pFirst->GetOwner())
            targetMask |= (1 << pFirst->GetOwner()->GetLayerIdx());
    }

    // CollisionMgr의 격자 기반 탐색 활용
    GameObject* targetSelected = CollisionMgr::GetInst()->FindNearestObject(EnemyPos, targetMask);

    // 아무것도 못 찾았다면 기본적으로 플레이어 반환
    return targetSelected ? targetSelected : pPlayer;
}

GameObject* EnemyWalkStrategy::GetRandomTargetFromAllObjects(CEnemyScript* _Enemy)
{
    if (GetRandom(0.f, 1.f) < 0.4f) return GM->GetPlayerObject();
    
    if (CStructure::GetInstalledStructures().empty()) return nullptr;
    if (CStructure* script = CStructure::GetInstalledStructures().getRandom())
        return script->GetOwner();
    return nullptr;
}

void EnemyWalkThroughCellPathStrategy::UseWalkStrategy(CEnemyScript* _Enemy)
{
    _Enemy->m_Velocity = Vec3::Zero;
    
    CBackgroundTile* const BackgroundCellManager = GM->GetBackgroundCellManager();
    Vec3 EnemyPos = _Enemy->Transform()->GetRelativePos();
    
    // 이미 해당 이동방법으로 이동을 끝낸 상황이거나, 이동하려했던 Target이 죽은 상황
    if (_Enemy->m_CellPath.empty() || !IsValid(_Enemy->m_TargetObject))
    {
        // Target이 죽은 상황이라면, 가지고 있던 CellPath도 의미가 없으므로 비워준다.
        if (!IsValid(_Enemy->m_TargetObject)) stack<CellCoord>().swap(_Enemy->m_CellPath);
        
        _Enemy->m_PathReplanTimer += DT;
        if (_Enemy->m_PathReplanTimer < _Enemy->m_PathReplanInterval) return;

        // Path replan interval time 종료
        
        _Enemy->m_PathReplanTimer = 0.f;
        
        // 새로운 Target 찾기
        // GameObject* TargetSelected = FindNearestTargetFromAllObjects(_Enemy);
        GameObject* TargetSelected = GetRandomTargetFromAllObjects(_Enemy);
        
        if (!TargetSelected) return; // 맵에 고를 Target이 없는 상황

        // Target을 향한 새로운 경로 지정
        const CellCoord CurrentCellCoord = BackgroundCellManager->GetWorldPosToCellCoord(ToVec2(EnemyPos));
        
        // DestCoord의 경우, AvailableCell이 아니라면 -> Adjacent한 Cell로 다시금 조정(Structure 위치의 경우, 그 자체가 Available하지 않음)
        const CellCoord ExactDestCoord = BackgroundCellManager->GetWorldPosToCellCoord(TargetSelected->Transform()->GetWorldPos2D());
        
        // Structure 자체의 경우, AvailableCell이 아니기 때문에 인접한 Cell로 잡아본다
        const CellCoord destCellCoord = (TargetSelected == GM->GetPlayerObject()) ? ExactDestCoord : ExactDestCoord.GetRandomAdjacentCellCoord(); 

        AStarPathFinder::GetInst()->GetPath(CurrentCellCoord, destCellCoord, _Enemy->m_CellPath);
        
        // Target 지정
        _Enemy->m_TargetObject      = TargetSelected;
        _Enemy->m_PathReplanTimer   = _Enemy->m_PathReplanInterval;
    }

    // 새로운 경로를 받았음에도 empty일 경우가 있음 (이때는 처리 x)
    if (_Enemy->m_CellPath.empty()) return;

    const Vec2 Destination = BackgroundCellManager->GetCellCoordToWorldPos(_Enemy->m_CellPath.top());
    const Vec2 Direction   = Destination - ToVec2(EnemyPos);

    const float DistToDest        = Direction.Length();
    const float MoveDistThisFrame = _Enemy->m_MoveSpeedBase * _Enemy->m_MoveSpeedFactor * DT;
    
    // 이번 이동 거리보다 남은 거리가 작다면 이번 이동 Cell 도착한 것으로 판정
    if (DistToDest <= MoveDistThisFrame) 
    {
        // 멈춰보이지 않게끔 Dummy Velocity 부여 (AnimHandler에서 Velocity 길이 0이면 멈춘 모션이 나와버림)
        // 만약 m_PrevCellPathVelocity가 0이라면(방금 출발한 경우 등), 현재 방향을 유지하도록 처리
        if (_Enemy->m_PrevCellPathVelocity.LengthSquared() == 0.f)
        {
            _Enemy->m_Velocity = ToVec3(Direction / DistToDest) * _Enemy->m_MoveSpeedBase * _Enemy->m_MoveSpeedFactor;
        }
        else
        {
            _Enemy->m_Velocity = _Enemy->m_PrevCellPathVelocity;
        }
        
        _Enemy->Transform()->SetRelativePos(ToVec3(Destination, Destination.y)); // Dest로 위치보정 처리
        _Enemy->m_CellPath.pop(); 
        return;        
    }
    
    _Enemy->m_Velocity = ToVec3(Direction / DistToDest) *  _Enemy->m_MoveSpeedBase * _Enemy->m_MoveSpeedFactor;
    EnemyPos += _Enemy->m_Velocity * DT;
    
    _Enemy->Transform()->SetRelativePos(EnemyPos);
    
    _Enemy->m_PrevCellPathVelocity = _Enemy->m_Velocity;
}

void EnemyWalkStraightStrategy::UseWalkStrategy(CEnemyScript* _Enemy)
{
    // Target Object가 valid하지 않은 상황, 새로운 TargetObject 찾아서 지정
    if (!IsValid(_Enemy->m_TargetObject))
    {
        GameObject* TargetSelected = FindNearestTargetFromAllObjects(_Enemy);
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


void EnemyFirstSpawnWalkStrategy::UseWalkStrategy(CEnemyScript* _Enemy)
{
    Vec2 EnemyPos = _Enemy->Transform()->GetRelativePosXY();
    _Enemy->m_Velocity = Vec3::Zero;
    
    Vec2 Direction = _Enemy->m_FirstSpawnMoveDestination - EnemyPos;
    Direction.Normalize();
    
    _Enemy->m_Velocity = ToVec3(Direction * _Enemy->m_MoveSpeedBase * _Enemy->m_MoveSpeedFactor);
    
    const Vec3 NewPos = _Enemy->Transform()->GetRelativePos() + _Enemy->m_Velocity * DT;
    _Enemy->Transform()->SetRelativePos(NewPos);
}

void EnemyPushedOutToInvalidCell::UseWalkStrategy(CEnemyScript* _Enemy)
{
    const Vec2 EnemyPos = _Enemy->Transform()->GetRelativePosXY();
    
    Vec2 Direction = _Enemy->m_ValidCellFound ? _Enemy->m_FoundValidCellPos - EnemyPos : _Enemy->GetPushedOutFaceDirection(); 
    Direction.Normalize();

    _Enemy->m_Velocity = ToVec3(Direction * _Enemy->m_MoveSpeedBase * _Enemy->m_MoveSpeedFactor);

    const Vec3 NewPos = _Enemy->Transform()->GetRelativePos() + _Enemy->m_Velocity * DT;
    _Enemy->Transform()->SetRelativePos(NewPos);
}
