#pragma once
#include "Source/Scripts/CharacterScript/CCharacterScript.h"

enum class ENEMY_TYPE;
enum class ENEMY_MAINSTATE;

class CEnemyScript : public CCharacterScript
{
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
    
public:
    
    CEnemyScript();
    virtual ~CEnemyScript() override;
    CLONE(CEnemyScript)

private:

    virtual void Init() override;
    virtual void Begin() override;
    virtual void AfterLevelBegin() override;
    virtual void Tick() override;
    
private:

    virtual void Move() override;
    virtual void UpdateCurrentFacedDirection() override;
    virtual void AfterPushedOutFin() override;
    
    void HandleFadeOut();

public:
    
    /// <summary>
    /// Die Animation 이후로 호출 처리될 함수, FadeOut 및 Pool에 다시 들어갈 준비
    /// </summary>
    void OnDieFlipbookEndNotify();

private:
    
    /// <summary>
    /// Player와의 충돌 Blocking 처리 
    /// </summary>
    void BodyColliderOverlapped(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider);
    
public:
    
    void SetMainState(ENEMY_MAINSTATE _MainState) { m_MainState = _MainState; }
    ENEMY_MAINSTATE GetMainState() const { return m_MainState; }

public:
    
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
    
};
