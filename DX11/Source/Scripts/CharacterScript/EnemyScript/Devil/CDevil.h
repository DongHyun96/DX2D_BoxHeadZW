#pragma once
#include "GameEngine/04.Asset/10.Sound/ASound.h"
#include "Source/Scripts/CharacterScript/EnemyScript/CEnemyScript.h"

/// <summary>
/// 불기둥 쏘는 방면 -> 근처 구조물이 있다면 구조물S
/// Walk Type은 항상 CellPath -> 중간에 이동하다가 막힌다면? -> 한자리에 머물렀던 총 시간 체크
/// </summary>
class CDevil : public CEnemyScript
{
private:
    
    // TargetObject가 현재 존재하고, 한 자리에 계속 머무른 시간을 측정해서 너무 길면 막혔다고 판단 -> 새로운 TargetObject 탐색해서 지정하고 바로 공격 처리할 것
    // 가장 가까운 TargetObject 공략할 것
    CellCoord   m_PrevCellCoord{};
    float       m_SameCoordStayTime{};
    
private:
    
    class CFlameLineHandler* m_FlameLineHandler{};
    class CEnemyAnimHandler* m_AnimHandler{};
    
private:

    float m_AttackTransitionWaitTime{};
    float m_TransitionToAttackTimer{};
    
    Ptr<ASound> m_AttackSound{};
    
    GameObject* m_LastDamageCauser{};
    
private:

    // 이동 중간에 Path 경로에 장애물이 새로 생겨서 이동을 못하는 상황 -> 이 때에는 바라보는 방면으로 공격처리를 한다
    bool m_Blocked{};
    
public:
    
    CDevil();
    virtual ~CDevil() override;
    CLONE(CDevil)
    
public:

    virtual void Init() override;
    virtual void Begin() override;

private:
    
    virtual void UpdateCurrentFacedDirection() override;
    
private:

    virtual void InitSpawn() override;
    
    virtual void OnTakeDamage(GameObject* _DamageCauser) override;
    
    virtual void AfterPushedOutFin() override;
    virtual void HandleStateTransition() override;
    virtual void OnAttackFlipbookEndNotify() override;

private:
    
    /// <summary>
    /// Attack 모션이 시작될 때 Callback 받음 -> 바라보는 방향으로 FlameLine 쏘기 처리할 것
    /// </summary>
    void OnAttackAnimStart();

private:

    virtual void SetMainState(ENEMY_MAINSTATE _MainState) override;

public:
    
    /// <summary>
    /// 현재 Block 상태인지 확인을 하고, true였으면 false로 되돌림 
    /// </summary>
    bool GetBlockedAndConsumeState()
    {
        bool Blocked = m_Blocked;
        m_Blocked = false;
        return Blocked;
    }
    
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
    
};
