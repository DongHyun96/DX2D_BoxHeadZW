#include "pch.h"
#include "CUIAnimation.h"

#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "Source/ScriptMgr.h"
#include "Source/Scripts/UIScript/CText.h"

CUIAnimation::CUIAnimation()
    : CScript(SCRIPT_TYPE::UIANIMATION)
{
}

CUIAnimation::~CUIAnimation()
{
}

void CUIAnimation::Tick()
{
    if (!m_bIsPlaying) return;
}

bool CUIAnimation::AddGameObjectToAnimate(GameObject* _GameObject)
{
    if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::STOP) return false;
    if (m_mapTracks.contains(_GameObject)) return false;
    if (!_GameObject->Transform()) return false; // 최소 Transform 정보는 있어야 Animation 가능
    
    // 초기 default 원본 기록 -> Animation Stop시, 해당 설정으로 돌아가기 위함
    m_mapTracks.insert(make_pair(_GameObject, UIAnimTrack()));
    
    UIAnimKeyFrameData FirstData{};
    
    FirstData.Time      = -1.f; // 초기 Time은 -1.f
    FirstData.Transform = _GameObject->Transform()->Clone();
    if (Ptr<CText> Text = _GameObject->GetScriptComponent<CText>())      FirstData.TintColor = Text->GetColor();
    else if (Ptr<CRenderComponent> Render = _GameObject->GetRenderCom()) FirstData.TintColor = Render->GetMaterial()->GetScalar<Vec4>(VEC4_0);
        
    m_mapTracks[_GameObject].KeyFrames.push_back(FirstData);
    
    return true;
}

bool CUIAnimation::RemoveGameObject(GameObject* _GameObject)
{
    if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::STOP) return false;
    if (!m_mapTracks.contains(_GameObject)) return false;

    // Animation 데이터에서 삭제
    m_mapTracks.erase(_GameObject);
    
    return true;
}

bool CUIAnimation::AddNewKeyFrame(GameObject* _GameObject, float _KeyFrameTime)
{
    if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::STOP) return false;
    if (!m_mapTracks.contains(_GameObject)) return false;                  // 해당 오브젝트가 Animation 정보에 등록되어 있지 않음 (먼저 AddGameObjectToAnimate 함수로 GameObject 추가할 것)

    vector<UIAnimKeyFrameData>& KeyFrames = m_mapTracks[_GameObject].KeyFrames;

    for (vector<UIAnimKeyFrameData>::iterator it = KeyFrames.begin(); it != KeyFrames.end(); ++it)
    {
        if (_KeyFrameTime < it->Time) continue;
        if (_KeyFrameTime == it->Time) return false; // 동시간대의 키프레임이 이미 존재한다면, 중복된 키프레임 추가 불가
        
        // 삽입될 위치가 여기다
        // 삽입 시, 이전 값과 동일한 it로 생성을 한다
        vector<UIAnimKeyFrameData>::iterator PrevIt = prev(it);
        UIAnimKeyFrameData NewData{};
        NewData.Time      = _KeyFrameTime;
        NewData.Transform = PrevIt->Transform->Clone();
        NewData.TintColor = PrevIt->TintColor;
        
        KeyFrames.insert(it, NewData);
        return true;
    }

    // 여기까지 애초에 도달할 수 없음
    // 도달하면 말이 안됨
    assert(nullptr);
    return false;
}

bool CUIAnimation::RemoveKeyFrame(GameObject* _GameObject, int _KeyFrameIdx)
{
    if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::STOP) return false;
    if (!m_mapTracks.contains(_GameObject)) return false;
    
    vector<UIAnimKeyFrameData>& KeyFrames = m_mapTracks[_GameObject].KeyFrames;
    if (_KeyFrameIdx <= 0 || _KeyFrameIdx >= KeyFrames.size()) return false; // 여기서 Default value인 index 0까지도 삭제 처리 불가능하게끔 처리 -> 사용자가 삭제가능한 키프레임은 idx 1부터 시작

    KeyFrames.erase(KeyFrames.begin() + _KeyFrameIdx);
    return true;
}

void CUIAnimation::Play(bool _bBackToStopOnEnd)
{
    // 초기 setting으로 조정 -> index 1번부터 재생처리를 해야 한다
    for (pair<GameObject* const, UIAnimTrack>& Pair : m_mapTracks)
        Pair.second.CurrentKeyIndex = 1;

    m_AnimTimer            = 0.f;
    m_bIsPlaying           = true;
    m_bBackToStopOnAnimEnd = _bBackToStopOnEnd;
}

void CUIAnimation::Stop()
{
    // 초기 setting으로 돌아게끔 처리
    for (pair<GameObject* const, UIAnimTrack>& Pair : m_mapTracks)
    {
        GameObject* TargetObject    = Pair.first;
        Pair.second.CurrentKeyIndex = 0;
        
        vector<UIAnimKeyFrameData>& KeyFrames = Pair.second.KeyFrames;
        
        TargetObject->Transform() = KeyFrames[0].Transform;
    }

    m_AnimTimer  = 0.f;
    m_bIsPlaying = false;
}

void CUIAnimation::SaveToLevelFile(FILE* _File)
{
}

void CUIAnimation::LoadFromLevelFile(FILE* _File)
{
    
}
