#include "pch.h"
#include "CUIAnimationGroup.h"

#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/05.GameObject/GameObjectRefHolder.h"
#include "CUIAnimation.h"
#include "Source/ScriptMgr.h"

CUIAnimationGroup::CUIAnimationGroup()
    : CScript(SCRIPT_TYPE::UIANIMATIONGROUP)
{
}

CUIAnimationGroup::~CUIAnimationGroup()
{
}

void CUIAnimationGroup::AfterLevelGameObjectGuidTableInit()
{
    m_mapAnimations.clear();
    
    for (pair<const wstring, GameObjectRefHolder>& Pair : m_mapAnimationGameObjects)
    {
        GameObjectRefHolder& AnimObjRefHolder = Pair.second; 
        AnimObjRefHolder.LinkReferenceToGameObject(LevelMgr::GetInst()->GetCurLevel());
        if (!AnimObjRefHolder.GetGameObject())
        {
            DebugUtil::AddDebugLog("[CUIAnimationGroup::AfterLevelGameObjectGuidTableInit] : AnimObj Link reference failed!");
            continue;
        }

        // Add Animation Scripts 
        GameObject* AnimObj                  = AnimObjRefHolder.GetGameObject();
        const Ptr<CUIAnimation>& UIAnimation = AnimObj->GetScriptComponent<CUIAnimation>();
        m_mapAnimations.insert(make_pair(Pair.first, UIAnimation.Get()));
        
        /* GameObject 래퍼런스로 들고 있었던 GameObject Destroy 시, Callback 처리 */
        AnimObjRefHolder.SetReferenceObjDestroyDelegate(bind(&CUIAnimationGroup::RemoveAnimationByGameObject, this, placeholders::_1));
    }
}

void CUIAnimationGroup::Tick()
{
}

bool CUIAnimationGroup::RemoveAnimationByGameObject(GameObject* _GameObject)
{
    if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::STOP) return false;

    wstring RemovedKey{};
        
    for (map<wstring, GameObjectRefHolder>::iterator it = m_mapAnimationGameObjects.begin(); it != m_mapAnimationGameObjects.end(); ++it)
    {
        if (it->second.GetGameObject() == _GameObject)
        {
            RemovedKey = it->first;
            m_mapAnimationGameObjects.erase(it);
            break;
        }
    }

    // 삭제할 GameObject가 존재하지 않았음
    if (RemovedKey.empty()) return false;
    
    m_mapAnimations.erase(RemovedKey);
    return true;
}

bool CUIAnimationGroup::RemoveAnimationByKey(const wstring& _AnimKey)
{
    if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::STOP) return false;
    if (!m_mapAnimationGameObjects.contains(_AnimKey)) return false;

    // 지우기 전 Stop 처리한 뒤 지우기
    m_mapAnimations[_AnimKey]->Stop();
    
    m_mapAnimationGameObjects.erase(_AnimKey);
    m_mapAnimations.erase(_AnimKey);
    return true;
}

bool CUIAnimationGroup::AddAnimation(const wstring& _AnimKey, GameObject* _AnimObj)
{
    if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::STOP) return false;
    if (!_AnimObj) return false;

    // Animation Object가 아닌 경우
    CUIAnimation* UIAnimation = _AnimObj->GetScriptComponent<CUIAnimation>().Get();
    if (!UIAnimation) return false;
    
    // Invalid Key
    if (_AnimKey.empty()) return false;

    GameObjectRefHolder AnimGameObjectRefHolder{};
    AnimGameObjectRefHolder.SetGameObject(_AnimObj);
    
    m_mapAnimationGameObjects[_AnimKey] = AnimGameObjectRefHolder;
    m_mapAnimations[_AnimKey]           = UIAnimation;
    
    return true;
}

bool CUIAnimationGroup::PlayAnimation(const wstring& _AnimToPlay, UIAnimEndHandling _EndHandling)
{
    if (!m_mapAnimations.contains(_AnimToPlay)) return false;

    // Play 처리하는 Animation을 제외한 나머지 Animation들 Stop 처리
    for (const pair<const wstring, CUIAnimation*>& AnimationPair : m_mapAnimations)
        AnimationPair.second->Stop();
    
    m_mapAnimations[_AnimToPlay]->Play(_EndHandling);
    return true;
}

bool CUIAnimationGroup::StopAnimation(const wstring& _AnimToStop)
{
    if (!m_mapAnimations.contains(_AnimToStop)) return false;
    m_mapAnimations[_AnimToStop]->Stop();
    return true;
}

void CUIAnimationGroup::StopAll()
{
    for (const pair<const wstring, CUIAnimation*>& AnimationPair : m_mapAnimations)
        AnimationPair.second->Stop();
}

void CUIAnimationGroup::SaveToLevelFile(FILE* _File)
{
    // 개수 저장
    const int Count = m_mapAnimationGameObjects.size();
    fwrite(&Count, sizeof(int), 1, _File);

    for (pair<const wstring, GameObjectRefHolder>& Pair : m_mapAnimationGameObjects)
    {
        SaveWString(_File, Pair.first);
        Pair.second.SaveToLevelFile(_File);
    }
}

void CUIAnimationGroup::LoadFromLevelFile(FILE* _File)
{
    int Count{};
    fread(&Count, sizeof(int), 1, _File);
    
    for (int i = 0; i < Count; ++i)
    {
        const wstring& AnimKey = LoadWString(_File);
        GameObjectRefHolder RefHolder{};
        RefHolder.LoadFromLevelFile(_File);
        
        m_mapAnimationGameObjects.insert(make_pair(AnimKey, RefHolder)); // 아직 GUID값만 Init된 상태 (m_mapAnimations 복원은 래퍼런스 Link 끝난 후(AfterLevelGameObjectGuidTableInit))
    }
}
