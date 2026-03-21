#pragma once
#include "GameEngine/06.Component/Script/CScript.h"

class CCamMoveScript : public CScript
{
private:
    
    class GameObject* m_2DFollowTargetObject{};
    Vec3 m_FollowDestPos{};
    bool m_UseLerpToFollow = true;
    
public:
    
    CCamMoveScript();
    virtual ~CCamMoveScript() override;

    CLONE(CCamMoveScript)
    
public:
    
    virtual void Tick() override;

private:
    void MoveOrthographic();
    void MovePerspective();
    void Move();
    // void CheckTogglingTargetMode();
    
public:
    
    void Set2DFollowTargetObject(GameObject* targetObject, bool useLerpToFollow)
    {
        m_2DFollowTargetObject = targetObject;
        m_UseLerpToFollow = useLerpToFollow;
    }
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override {}
    virtual void LoadFromLevelFile(FILE* _File) override {}
    
};
