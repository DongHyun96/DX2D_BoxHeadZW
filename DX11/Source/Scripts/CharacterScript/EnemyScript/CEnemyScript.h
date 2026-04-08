#pragma once
#include "Source/Scripts/CharacterScript/CCharacterScript.h"

enum class ENEMY_TYPE;
enum class ENEMY_MAINSTATE;
enum class ENEMY_WALK_TYPE;


enum class ENEMY_WALK_TYPE
{
    CELL_PATH,
    STRAIGHT,
};

class CEnemyScript : public CCharacterScript
{

    friend class EnemyWalkStrategy;
    friend class EnemyWalkThroughCellPathStrategy;
    friend class EnemyWalkStraightStrategy;

protected:

    ENEMY_TYPE      m_EnemyType{};
    ENEMY_MAINSTATE m_MainState{};

private:
    
    float m_AttackDamage{}; // 공격 Damage량 (Devil의 경우, 불쏘시개 하나 당 damage)
    
private: // FadeIn Out 관련

    bool m_HasFadeInStart{};
    bool m_HasFadeOutStart{};
    
    const float m_FadeInOutTotalTime = 1.f;
    float       m_FadeInOutTime{};

private:
    
    Ptr<GameObject> m_TargetObject{}; // 접근하는 GameObject, 또는 공격대상 모두 이 TargetObject로 잡아서 처리
    
private: // AStar Path 및 Walk 관련
    
    stack<CellCoord> m_CellPath{}; // AStar로 찾은 CellPath
    
    static map<ENEMY_WALK_TYPE, Ptr<EnemyWalkStrategy>> s_mapWalkingStrategies;
    ENEMY_WALK_TYPE m_CurrentWalkType{};
    
private:

    bool m_HasAttackStart{};

protected:
    UINT m_AttackFlipbookCount = 8;
    
public:
    
    CEnemyScript();
    virtual ~CEnemyScript() override;
    CLONE(CEnemyScript)
    
protected:
    
    CEnemyScript(SCRIPT_TYPE _Type);

public:

    virtual void Init() override;
    virtual void Begin() override;
    virtual void AfterLevelBegin() override;
    virtual void Tick() override;
    
private:

    virtual void Move() override;
    
    virtual void UpdateCurrentFacedDirection() override;
    
    /// <summary>
    /// Attack 상태에서의 FacedDirection 업데이트
    /// Runner의 경우 16방향 업데이트로 처리를 해야해서 virtual로 뚫어둠
    /// </summary>
    virtual void UpdateAttackFacedDirection();
    
    virtual void AfterPushedOutFin() override;
    
    void HandleFadeOut();

    /// <summary>
    /// Perception의 Sight에 들어온 Enemy들 및, 현 State / TargetObject 상태에 따른 State Transition 처리 담당
    /// TODO : Devil의 경우 이 함수 override해서 처리해야할듯?
    /// </summary>
    virtual void HandleStateTransition();

    

private:
    
    /// <summary>
    /// m_CellPath를 따라 이동
    /// </summary>
    void MoveThroughCellPath();
    
public:
    
    /// <summary>
    /// Die Animation 이후로 호출 처리될 함수, FadeOut 및 Pool에 다시 들어갈 준비
    /// </summary>
    void OnDieFlipbookEndNotify();

    /// <summary>
    /// 공격 모션이 Interrupt, 또는 정상 종료 되었을 때 호출받을 함수 
    /// </summary>
    virtual void OnAttackFlipbookEndNotify(); // TODO : Devil의 경우, 이거 override

    /// <summary>
    /// Attack Flipbook 재생 시, 특정 Idx를 지날 때 호출받을 함수
    /// 이 함수 내에서 AttackDamageCollider를 켜준다
    /// </summary>
    void OnAttackNotify();

private:
    
    /// <summary>
    /// Player와의 충돌 Blocking 처리 
    /// </summary>
    void BodyColliderOverlapped(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider);
    
public:
    
    void SetMainState(ENEMY_MAINSTATE _MainState) { m_MainState = _MainState; }
    ENEMY_MAINSTATE GetMainState() const { return m_MainState; }
    
    void SetTargetObject(const Ptr<GameObject>& _Target) { m_TargetObject = _Target; }
    const Ptr<GameObject>& GetTargetObject() const { return m_TargetObject; }
    
    void SetCurrentWalkType(ENEMY_WALK_TYPE _WalkType) { m_CurrentWalkType = _WalkType; }
    
    float GetAttackDamage() const { return m_AttackDamage; }

public:
    
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
    
};
