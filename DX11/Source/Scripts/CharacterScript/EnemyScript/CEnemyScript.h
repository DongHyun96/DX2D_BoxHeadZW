#pragma once
#include "Source/Scripts/CharacterScript/CCharacterScript.h"

enum class ENEMY_TYPE;
enum class ENEMY_MAINSTATE;
enum class ENEMY_WALK_TYPE;


enum class ENEMY_WALK_TYPE
{
    CELL_PATH,
    STRAIGHT,
    FIRST_SPAWN_WALK, // 첫 Spawn region에서 spawn되었고, Valid한 Cell까지 이동
    PUSHED_OUT_TO_INVALID_CELL, // PushedOut으로 인해 InavlidCell로 밀려난 경우, 해당 Walk Strategy 사용할 것
};

class CEnemyScript : public CCharacterScript
{

    friend class EnemyWalkStrategy;
    friend class EnemyWalkThroughCellPathStrategy;
    friend class EnemyWalkStraightStrategy;
    friend class EnemyFirstSpawnWalkStrategy;
    friend class EnemyPushedOutToInvalidCell;
    
protected:

    ENEMY_TYPE      m_EnemyType{};
    ENEMY_MAINSTATE m_MainState{};

protected:

    // 현재, First Spawn pos에서 태어나서 Valid한 Cell로 이동처리를 해야하는 Enemy일 때, 해당 멤버변수를 초기화 받아서 이동처리함
    Vec2        m_FirstSpawnMoveDestination{};
    CellCoord   m_FirstSpawnMoveDestCellCoord{};

private: // PushedOut to invalid cell 관련
    
    // PushedOut으로 인해 invalid cell로 밀려난 경우, 해당 Cell에서 다시 Valid한 Cell로 이동할 때 사용할 Cell 좌표
    bool m_ValidCellFound{};
    Vec2 m_FoundValidCellPos{}; 
    
private:
    
    float m_AttackDamage{}; // 공격 Damage량 (Devil의 경우, 불쏘시개 하나 당 damage)

    class CPerceptionHandler* m_PerceptionHandler{};
    class CStatScript*        m_StatScript{};
    
private: // FadeIn Out 관련

    bool m_HasFadeInStart{};
    bool m_HasFadeOutStart{};
    
    const float m_FadeInOutTotalTime = 1.f;
    float       m_FadeInOutTime{};

private:
    
    Ptr<GameObject> m_TargetObject{}; // 접근하는 GameObject, 또는 공격대상 모두 이 TargetObject로 잡아서 처리
    
private: // AStar Path 및 Walk 관련
    
    stack<CellCoord> m_CellPath{}; // AStar로 찾은 CellPath
    float            m_PathReplanTimer{};
    float            m_PathReplanInterval = 3.f;
    
    static map<ENEMY_WALK_TYPE, Ptr<EnemyWalkStrategy>> s_mapWalkingStrategies;
    ENEMY_WALK_TYPE m_CurrentWalkType{};
    
protected:
    UINT m_AttackFlipbookCount = 8;
    
private:
    
    Vec3 m_PrevCellPathVelocity{};
    
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

public:
    
    /// <summary>
    /// 피격 당한 이후 처리를 해야 하는 Enemy 종류의 경우, 해당 함수 override 할 것 
    /// </summary>
    virtual void OnTakeDamage(GameObject* _DamageCauser) {}
    
private:
    
    /// <summary>
    /// FadeOut 처리가 끝난 뒤, 더 처리할 구현이 있다면 해당 함수 override 할 것 (Mummy) 
    /// </summary>
    virtual void OnFadeOutEnd() {}
    
private:

    virtual void Move() override;
    
    virtual void UpdateCurrentFacedDirection() override;

protected:
    
    virtual void AfterPushedOutFin() override;
    
private:
    
    void HandleFadeOut();

    /// <summary>
    /// Perception의 Sight에 들어온 Enemy들 및, 현 State / TargetObject 상태에 따른 State Transition 처리 담당
    /// </summary>
    virtual void HandleStateTransition();

public:
    
    /// <summary>
    /// Spawn 처리할 때 Init 처리해야하는 부분들 
    /// </summary>
    virtual void InitSpawn();
    
    /// <summary>
    /// 죽기 시작했을 떄의 처리 (Collider 꺼주기 등)  
    /// </summary>
    void OnDieStart();

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
    
    virtual void SetMainState(ENEMY_MAINSTATE _MainState) { m_MainState = _MainState; }
    ENEMY_MAINSTATE GetMainState() const { return m_MainState; }
    
    void SetTargetObject(const Ptr<GameObject>& _Target) { m_TargetObject = _Target; }
    const Ptr<GameObject>& GetTargetObject() const { return m_TargetObject; }

    // Runner의 경우, WALK_TYPE에 따른 속력이 바뀜
    // Vampire의 경우, CellPath Walk setting 시 setting할 값들이 있음
    virtual void SetCurrentWalkType(ENEMY_WALK_TYPE _WalkType);
    ENEMY_WALK_TYPE GetCurrentWalkType() const { return m_CurrentWalkType; }
    
    float GetAttackDamage() const { return m_AttackDamage; }
    
    // void SetFirstSpawnMoveDestination(const Vec2& _MoveDestination) { m_FirstSpawnMoveDestination = _MoveDestination; }
    void SetFirstSpawnMoveDestination(const CellCoord& _MoveDestinationCellCoord);
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
    
};
 