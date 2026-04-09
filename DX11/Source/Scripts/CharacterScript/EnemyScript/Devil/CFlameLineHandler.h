#pragma once

/// <summary>
/// Devil에 붙어있는 자식 오브젝트여야 함
/// </summary>
class CFlameLineHandler : public CScript
{
private:

    class CCollider2D* m_AttackCollider{}; // 이 AttackCollider를 키우고 줄일 예정
    
public:
    
    CFlameLineHandler();
    virtual ~CFlameLineHandler() override;
    CLONE(CFlameLineHandler);
    
public:

    virtual void Begin() override;
    virtual void Tick() override;
    
public:
    
    /// <summary>
    /// Flame line 생성
    /// </summary>
    void MakeFlameLine(const Vec2& _Direction, float _Length, float _Damage);
    
public:
    
    void SaveToLevelFile(FILE* _File) override {}
    void LoadFromLevelFile(FILE* _File) override {}
    
};
