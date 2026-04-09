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
    
private:

    float m_AttackTransitionWaitTime{};
    float m_TransitionToAttackTimer{};
    
    Ptr<ASound> m_AttackSound{};
    
public:
    
    CDevil();
    virtual ~CDevil() override;
    CLONE(CDevil)
    
public:

    virtual void Init() override;
    virtual void Begin() override;
    virtual void Tick() override;
    
private:

    virtual void OnTakeDamage() override;
    virtual void HandleStateTransition() override;
    virtual void OnAttackFlipbookEndNotify() override;

private:
    
    /// <summary>
    /// Attack 모션이 시작될 때 Callback 받음 -> 바라보는 방향으로 FlameLine 쏘기 처리할 것
    /// </summary>
    void OnAttackAnimStart();
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
    
};
