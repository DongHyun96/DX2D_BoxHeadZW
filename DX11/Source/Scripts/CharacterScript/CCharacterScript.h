#pragma once

/// <summary>
/// MainState 업데이트 
/// </summary>
class CCharacterScript : public CScript
{
protected:
    
    float m_MoveSpeedBase       = 300.f;
    float m_MoveSpeedFactor     = 1.f;
    
    Vec3 m_Velocity{};

private:
    
    Vec2 m_BodySize{};
    Vec2 m_BodySizeHalf{};
    
protected:
    
    EDIRECTION m_CurrentFacedDirection{};

private: // PushedOut 관련

    const float     m_PushedOutTotalTime = 0.4f; // PushedOut 멈춰있는 모습 처리로 보여질 총 시간
    float           m_PushedOutTime{};
    Vec2            m_PushedOutFaceDirection{};

public:
    
    CCharacterScript(enum SCRIPT_TYPE _ScriptType);
    virtual ~CCharacterScript() override;
    
public:

    virtual void Begin() override;
    virtual void Tick() override;

private:
    
    virtual void Move() = 0;
    virtual void UpdateCurrentFacedDirection() = 0;
    
protected:
    
    /// <summary>
    /// PushedOut Movement 처리
    /// </summary>
    void MovePushedOut();
    
private:
    
    virtual void AfterPushedOutFin() = 0;

public:
    
    bool IsCurrentlyOutOfBound() const;
    
protected:

    void HandleBoundary();

public:
    
    virtual void SaveToLevelFile(FILE* _File) override {}
    virtual void LoadFromLevelFile(FILE* _File) override {}
    
public:
    
    const Vec3& GetVelocity() const { return m_Velocity; }
    EDIRECTION GetCurrentFacedDirection() const { return m_CurrentFacedDirection; }

public:

    const Vec2& GetPushedOutFaceDirection() const { return m_PushedOutFaceDirection; }
    void RewindPushedOut(const Vec2& _PushedOutDirection);
    
public:

    const Vec2& GetBodySize() const { return m_BodySize; }
    const Vec2& GetBodySizeHalf() const { return m_BodySizeHalf; }
    
};
