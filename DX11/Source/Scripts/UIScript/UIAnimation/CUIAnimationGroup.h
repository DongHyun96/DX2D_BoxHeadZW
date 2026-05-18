#pragma once

#include "CUIAnimation.h"
#include "GameEngine/05.GameObject/GameObjectRefHolder.h"


/// <summary>
/// UIAnimation GORefHolder로 Animation 오브젝트 관리
/// 재생 처리는, 하나만 재생처리를 한다
/// </summary>
class CUIAnimationGroup : public CScript
{
    
    friend class UIAnimationGroupUI;
    
private:

    map<wstring, GameObjectRefHolder> m_mapAnimationGameObjects{};
    map<wstring, class CUIAnimation*> m_mapAnimations{};
    
public:
    
    CUIAnimationGroup();
    CUIAnimationGroup(const CUIAnimationGroup& _Origin);
    virtual ~CUIAnimationGroup() override;
    
    CLONE(CUIAnimationGroup);

private:
    
    virtual void AfterLevelGameObjectGuidTableInit() override;

public:
    
    virtual void Tick() override;

public: // 삭제, 추가 처리는 Editing 환경에서만 가능하도록 처리
    
    bool RemoveAnimationByKey(const wstring& _AnimKey);
    
    bool AddAnimation(const wstring& _AnimKey, GameObject* _AnimObj);
    

public:
    
    bool PlayAnimation(const wstring& _AnimToPlay, UIAnimEndHandling _EndHandling = UIAnimEndHandling::DEFAULT);
    bool StopAnimation(const wstring& _AnimToStop);
    void StopAll();
    
    
public:
    
    void SaveToLevelFile(FILE* _File) override;
    void LoadFromLevelFile(FILE* _File) override;
    
};
