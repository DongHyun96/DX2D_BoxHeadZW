#include "pch.h"
#include "CUIAnimation.h"

#include "UIAnimationData.h"
#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/05.GameObject/GameObjectRefHolder.h"
#include "Source/ScriptMgr.h"
#include "Source/Scripts/UIScript/CText.h"

CUIAnimation::CUIAnimation()
    : CScript(SCRIPT_TYPE::UIANIMATION)
{
}

CUIAnimation::CUIAnimation(const CUIAnimation& _Origin)
    : CScript(_Origin)
    , m_vecTracks(_Origin.m_vecTracks)
    , m_bBackToStopOnAnimEnd(_Origin.m_bBackToStopOnAnimEnd)
    // 나머지 멤버변수는 초기값으로 처리 (Editing 환경에서 재생 Testing 중이던 Animation일 수 있음 -> 이걸 다시 멈춘 상태로 시작)
{
}

CUIAnimation::~CUIAnimation()
{
}

void CUIAnimation::AfterLevelGameObjectGuidTableInit()
{
    // Track에 지정된 GameObject Reference 실제로 복원 처리 & Origin Data 또한 복원한 Reference로 복원 처리를 해준다
    for (UIAnimTrack& Track : m_vecTracks)
    {
        Track.TargetObjectReference.LinkReferenceToGameObject(LevelMgr::GetInst()->GetCurLevel().Get());

        // 원본 KeyFramer값 세팅
        GameObject* TargetObject = Track.TargetObjectReference.GetGameObject();
        if (!TargetObject)
        {
            DebugUtil::AddDebugLog("[CUIAnimation::AfterLevelGameObjectGuidTableInit] : GameObject ref invalid!");
            return;
        }
        
        /* GameObject 래퍼런스로 들고 있었던 원본 GameObject가 Destroy 되었을 때 Callback binding */
        // 원본 GameObject가 Destroy 당하면, 달려있었던 Track을 삭제처리한다
        Track.TargetObjectReference.SetDestroyDelegate(bind(&CUIAnimation::RemoveTrackByGameObject, this, placeholders::_1));

        /* Original State 값들 저장 */
        Track.OriginalStateData.SetAnimDataFromGameObject(TargetObject, -1.f);
    }
    
    // Editing 환경에서도 Tick함수가 돌게끔 처리
    if (LevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::STOP)
    {
        LevelMgr::GetInst()->GetCurLevel()->AddEditingTickEnabledGameObject(GetOwner());
        GetOwner()->SetDTContextType(DT_CONTEXT_TYPE::IMPLICIT_ENGINE_DT);
    }
    else GetOwner()->SetDTContextType(DT_CONTEXT_TYPE::DEFAULT);
    
    Stop(); // 첫 Stop 처리 추가 (Level Stop 시, Play 시 모두 필요)
    
}

void CUIAnimation::Tick()
{
    if (!m_bIsPlaying) return;

    m_AnimTimer += DT;

    UINT TrackFinishedCnt{};
    for (UIAnimTrack& Track : m_vecTracks)
    {
        Track.WhilePlaying(m_AnimTimer);
        if (!Track.GetIsPlaying()) ++TrackFinishedCnt;
    }
    
    // 모든 Track의 재생이 끝났는지 체크
    if (TrackFinishedCnt >= m_vecTracks.size())
    {
        // 모두 끝난 시점, 만약 원상태로 복구 처리 옵션이 체크 되어있다면, 원상태로 복구
        if (m_bBackToStopOnAnimEnd) Stop();
        m_bIsPlaying = false;
    }
}

void CUIAnimation::SetEditingAnimTime(float _Time)
{
    m_EditingAnimTimer = max(_Time, 0.f);
    
    // 재생 중이 아닐 때에만 Editing timer를 사용하여 상태 업데이트
    if (!m_bIsPlaying)
    {
        for (UIAnimTrack& Track : m_vecTracks)
        {
            Track.ManuallyUpdateTintColorControlType();
            Track.UpdateTrack(m_EditingAnimTimer);
        }
    }
}

bool CUIAnimation::AddGameObjectToAnimate(GameObject* _GameObject)
{
    if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::STOP)  return false;
    if (IsTrackHasObject(_GameObject))                              return false;
    if (!_GameObject->Transform())                                  return false; // 최소 Transform 정보는 있어야 Animation 가능
    
    // 새로운 AnimTrack 추가
    UIAnimTrack NewAnimTrack{};
    NewAnimTrack.TargetObjectReference.SetGameObject(_GameObject);
    NewAnimTrack.OriginalStateData.SetAnimDataFromGameObject(_GameObject, -1.f); // 초기 default 원본 기록 -> Animation Stop시, 해당 설정으로 돌아가기 위함
    
    // 0번 키프레임 추가 (일단 첫 키 프레임 생성은 원본의 값과 동일한 값으로 세팅되게끔 처리한다)
    UIAnimKeyFrameData FirstKeyFrame = NewAnimTrack.OriginalStateData;
    FirstKeyFrame.Time               = 0.f;
    NewAnimTrack.KeyFrames.push_back(FirstKeyFrame);
    
    m_vecTracks.push_back(NewAnimTrack);
    return true;
}

bool CUIAnimation::RemoveTrackByTrackIdx(int _TrackIdx)
{
    if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::STOP) return false;
    if (_TrackIdx < 0 || _TrackIdx >= m_vecTracks.size()) return false;

    m_vecTracks.erase(m_vecTracks.begin() + _TrackIdx);
    return true;
}

bool CUIAnimation::RemoveTrackByGameObject(GameObject* _GameObject)
{
    if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::STOP) return false;

    for (auto it = m_vecTracks.begin(); it != m_vecTracks.end(); ++it)
    {
        const UIAnimTrack& Track = *it;
        
        if (Track.TargetObjectReference.GetGameObject() == _GameObject)
        {
            m_vecTracks.erase(it);
            return true;
        }
    }

    // 일치하는 GameObject를 찾지 못함
    return false;
}

bool CUIAnimation::AddNewKeyFrame(int _TrackIdx, float _KeyFrameTime)
{
    if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::STOP) return false;
    if (_TrackIdx < 0 || _TrackIdx >= m_vecTracks.size()) return false;

    vector<UIAnimKeyFrameData>& KeyFrames = m_vecTracks[_TrackIdx].KeyFrames;

    for (vector<UIAnimKeyFrameData>::iterator it = KeyFrames.begin(); it != KeyFrames.end(); ++it)
    {
        if (_KeyFrameTime > it->Time) continue;
        if (_KeyFrameTime == it->Time) return false; // 동시간대의 키프레임이 이미 존재한다면, 중복된 키프레임 추가 불가
        
        // 삽입될 위치가 여기다
        // 삽입 시, 이전 값과 동일한 it로 생성을 한다
        vector<UIAnimKeyFrameData>::iterator PrevIt = prev(it);
        UIAnimKeyFrameData NewData{};
        NewData.Time       = _KeyFrameTime;
        NewData.Transform  = PrevIt->Transform->Clone();
        NewData.TintColor  = PrevIt->TintColor;
        NewData.EasingType = PrevIt->EasingType;
        
        KeyFrames.insert(it, NewData);
        return true;
    }

    // 마지막 키프레임으로 추가되었을 때
    UIAnimKeyFrameData NewData{};
    NewData.Time      = _KeyFrameTime;
    NewData.Transform = KeyFrames.back().Transform->Clone();
    NewData.TintColor = KeyFrames.back().TintColor;
    KeyFrames.push_back(NewData);
    
    return true;
}

bool CUIAnimation::RemoveKeyFrame(int _TrackIdx, int _KeyFrameIdx)
{
    if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::STOP) return false;
    if (_TrackIdx < 0 || _TrackIdx >= m_vecTracks.size()) return false;
    
    vector<UIAnimKeyFrameData>& KeyFrames = m_vecTracks[_TrackIdx].KeyFrames;
    if (_KeyFrameIdx < 0 || _KeyFrameIdx >= KeyFrames.size()) return false;

    KeyFrames.erase(KeyFrames.begin() + _KeyFrameIdx);
    return true;
}

int CUIAnimation::SortKeyFrames(int _TrackIdx, int _CurrentIdx)
{
    if (_TrackIdx < 0 || _TrackIdx >= m_vecTracks.size()) return -1;

    vector<UIAnimKeyFrameData>& KeyFrames = m_vecTracks[_TrackIdx].KeyFrames;
    if (KeyFrames.empty()) return -1;

    // 현재 선택된 키프레임의 포인터나 고유 값을 기억 (여기서는 시간과 데이터를 기준으로 찾음)
    UIAnimKeyFrameData selectedData = KeyFrames[_CurrentIdx];

    // 시간순 정렬
    std::sort(KeyFrames.begin(), KeyFrames.end(), [](const UIAnimKeyFrameData& a, const UIAnimKeyFrameData& b) {
        return a.Time < b.Time;
    });

    // 정렬 후 새로운 인덱스 찾아 반환
    for (int i = 0; i < static_cast<int>(KeyFrames.size()); ++i)
    {
        if (KeyFrames[i].Time == selectedData.Time)
            return i;
    }
    return 0;
}

bool CUIAnimation::IsTrackHasObject(GameObject* _GameObject)
{
    for (const UIAnimTrack& Track : m_vecTracks)
        if (Track.TargetObjectReference.GetGameObject() == _GameObject) return true;
    return false;
}

bool CUIAnimation::Play(bool _bBackToStopOnEnd)
{
    if (m_vecTracks.empty()) return false;
    
    for (UIAnimTrack& Track : m_vecTracks)
    {
        // 재생 불가능한 Track이 껴 있을 수 있음 ( ex) 키 프레임이 하나도 없는 경우 )
        // 모든 트랙이 재생 가능한 상태여야 Play 처리 가능하다고 판단
        if (!Track.OnPlayStart()) return false;
    }
    
    // Animation 관련 값 초기 setting으로 조정
    m_AnimTimer            = 0.f;
    m_bIsPlaying           = true;
    m_bBackToStopOnAnimEnd = _bBackToStopOnEnd;
    return true;
}

void CUIAnimation::Stop()
{
    // 초기 setting으로 돌아게끔 처리
    for (UIAnimTrack& Track : m_vecTracks) Track.Stop();

    m_AnimTimer  = 0.f;
    m_bIsPlaying = false;
}

void CUIAnimation::SaveToLevelFile(FILE* _File)
{
    // 개수 저장
    const int TrackCount = m_vecTracks.size();
    fwrite(&TrackCount, sizeof(int), 1, _File);

    for (UIAnimTrack& Track : m_vecTracks)
        Track.SaveToLevelFile(_File);
}

void CUIAnimation::LoadFromLevelFile(FILE* _File)
{
    int TrackCount{};
    fread(&TrackCount, sizeof(int), 1, _File);
    
    for (int i = 0; i < TrackCount; ++i)
    {
        UIAnimTrack AnimTrack{};
        AnimTrack.LoadFromLevelFile(_File);
        m_vecTracks.push_back(AnimTrack);
    }
}
