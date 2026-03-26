#pragma once

class CEditorCamMoveScript : public CScript
{
private:

    Vec3 m_FollowDestPos{};
    bool m_UseLerpToFollow = true;
    
    float m_Speed = 500.f;
    
    float m_MoveSpeed = 7000.f;

    Vec3 m_TransformPosDest{};
    
public:
    
    CEditorCamMoveScript();
    virtual ~CEditorCamMoveScript() override;

    CLONE(CEditorCamMoveScript)

public:

    virtual void SaveToLevelFile(FILE* _File) override {}
    virtual void LoadFromLevelFile(FILE* _File) override {}
    
    void SetTransformPosDest(const Vec2& _TransformPosDestXY)
    {
        m_TransformPosDest.x = _TransformPosDestXY.x;
        m_TransformPosDest.y = _TransformPosDestXY.y;
        m_TransformPosDest.x = clamp(m_TransformPosDest.x, -50000.f, 50000.f);
        m_TransformPosDest.y = clamp(m_TransformPosDest.y, -50000.f, 50000.f);
    }
    
public:

    virtual void Init() override;
    virtual void Tick() override;
    

private:
    /// <summary>
    /// Ctrl + Wheel 조합으로 줌인 줌아웃 처리
    /// </summary>
    void MouseWheelTick();
    
    void MoveOrthographic();
    void MovePerspective();
    
};
