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
    , m_EndHandling(_Origin.m_EndHandling)
    // 나머지 멤버변수는 초기값으로 처리 (Editing 환경에서 재생 Testing 중이던 Animation일 수 있음 -> 이걸 다시 멈춘 상태의 객체로 복사)
{
    
    if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::STOP) return;
    
    // Editing 환경에서의 사용자 요청에 의한(Duplicate) 복사 상황인 경우, Delegate 및 기본 처리를 해주어야 함
    for (UIAnimTrack& Track : m_vecTracks)
    {
        if (!Track.TargetObjectReference.GetGameObject())
        {
            DebugUtil::AddDebugLog("[CUIAnimation(Copy Constructor)] : Invalid Track's TargetObject Received during Duplicating AnimObj");
            continue;
        }
    }
    
    // Editing 환경에서 Tick 함수가 돌아야 하기 때문에, 해당 처리 추가
    // 이게 정확히 AddComponent 이전에 호출이 들어오기 때문에, OwnerGameObject가 명확히 잡혀있지 않은 상황
    // 따라서 Init 시점에 해당 처리를 해준다
    // RegisterEditingTickEnabled();
    
    // 기본 Stop인 상황으로 기본값 setting이 되어 있음
    // Stop();
}

CUIAnimation::~CUIAnimation()
{
}

void CUIAnimation::Init()
{
    // 사용자 요청으로 인한 Animation GO Duplicate 처리 시, EditingTickEnabled 옵션을 이 시점에서 켜주어야 valid하게 처리됨
    if (LevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::STOP)
        RegisterEditingTickEnabled();
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
        
        /* Original State 값들 기록 */
        if (LevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::STOP)
        {
            UIAnimKeyFrameData OriginalStateData{};
            OriginalStateData.SetAnimDataFromGameObject(TargetObject, -1.f);
            UIAnimTrack::ModifyOriginalStateData(TargetObject->GetGUID(), OriginalStateData);
        }
    }
    
    // Editing 환경에서도 Tick함수가 돌게끔 처리
    if (LevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::STOP)
        RegisterEditingTickEnabled();
    
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
        // 끝처리 Type에 따른 끝 처리
        switch (m_EndHandling)
        {
        case UIAnimEndHandling::DEFAULT:
            m_bIsPlaying = false;
            return;
        case UIAnimEndHandling::BACK_TO_STOP:
            Stop();
            m_bIsPlaying = false;
            return;
        case UIAnimEndHandling::LOOP:
            Play(UIAnimEndHandling::LOOP);
            return;
        }
    }
}

bool CUIAnimation::IsPlaying() const
{
    switch (m_EndHandling)
    {
    case UIAnimEndHandling::BACK_TO_STOP: case UIAnimEndHandling::LOOP: return m_bIsPlaying;
    case UIAnimEndHandling::DEFAULT: return m_bIsPlaying || m_AnimTimer > 0.f;
    }
    
    return false;
}

void CUIAnimation::SetEditingAnimTime(float _Time)
{
    // 재생 중이 아닐 때에만 Editing timer를 사용하여 상태 업데이트
    if (m_bIsPlaying) return;
    
    m_EditingAnimTimer = max(_Time, 0.f);
    
    for (UIAnimTrack& Track : m_vecTracks)
    {
        Track.ManuallyUpdateTintColorControlType();
        Track.UpdateTrack(m_EditingAnimTimer);
    }
}

bool CUIAnimation::AddGameObjectToAnimate(GameObject* _GameObject)
{
    if (!_GameObject)                                               return false;
    if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::STOP)  return false;
    if (IsTrackHasObject(_GameObject))                              return false; // 이미 해당 Object AnimTrack이 있는 경우
    if (!_GameObject->Transform())                                  return false; // 최소 Transform 정보는 있어야 Animation 가능
    
    /* 새로운 AnimTrack 추가 */
    UIAnimTrack NewAnimTrack{};
    NewAnimTrack.TargetObjectReference.SetGameObject(_GameObject);
    
    /* 원본값 기록 */
    // 1. 기존의 원본값이 이미 존재했다면 -> 해당 원본값으로 원본값 사용 -> 다른 Animation에서 Stop 처리를 안한 상태에서 해당 GO를 이 Animation에 추가할 경우 원본값 훼손이 될 수 있음 -> 기존의 원본값을 본래의 원본값으로 저장 처리할 것
    // 최초 Animate할 GO를 잡은 상황 -> 현재 GO 상태를 초기원본으로 기록처리
    
    UIAnimKeyFrameData OriginalKeyFrameData{};

    
    if (NewAnimTrack.TryAddSelfOriginalStateDataCount()) // 기존 데이터가 있다고 간주하고 Count만 올리기 시도
    {
        // 기존 데이터가 있는 경우, 기존 데이터를 새로 들어온 GO의 데이터로 훼손하지 않는다.
        UIAnimTrack::GetOriginalKeyFrameData(_GameObject->GetGUID(), OriginalKeyFrameData);
    }
    else // 처음 OriginalStateData를 기록하는 상황
    {
        // 기존 데이터가 없는 상황 -> 이번에 들어온 GameObject가 원본 데이터라고 간주하고 원본 데이터 최초 기록
        
        OriginalKeyFrameData.SetAnimDataFromGameObject(_GameObject, -1.f); // 초기 default 원본 기록 -> Animation Stop시, 해당 설정으로 돌아가기 위함

        if (!NewAnimTrack.AddSelfOriginalStateData(OriginalKeyFrameData))
            DebugUtil::AddDebugLog("[CUIAnimation::AddGameObjectToAnimate] : Failed to AddSelfOriginalStateData");
    }
    
    
    // 0번 키프레임 추가 (일단 첫 키 프레임 생성은 원본의 값과 동일한 값으로 세팅되게끔 처리한다)
    UIAnimKeyFrameData FirstKeyFrame = OriginalKeyFrameData;
    FirstKeyFrame.Time               = 0.f;
    NewAnimTrack.KeyFrames.push_back(move(FirstKeyFrame));

    // Stop 처리를 함으로 초기화 처리(ex) KeyFrameIdx UpdateStrategy 기타 등등)
    NewAnimTrack.Stop();
    
    m_vecTracks.push_back(move(NewAnimTrack));
    return true;
}

bool CUIAnimation::RemoveTrackByTrackIdx(int _TrackIdx)
{
    if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::STOP) return false;
    if (_TrackIdx < 0 || _TrackIdx >= m_vecTracks.size()) return false;

    if (!m_vecTracks[_TrackIdx].ReduceSelfOriginalStateData())
        DebugUtil::AddDebugLog("[CUIAnimation::RemoveTrackByTrackIdx] : ReduceSelfOriginalStateData failed");
    
    m_vecTracks.erase(m_vecTracks.begin() + _TrackIdx);
    return true;
}

UIAnimKeyFrameData* CUIAnimation::AddNewKeyFrame(int _TrackIdx, float _KeyFrameTime)
{
    if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::STOP) return nullptr;
    if (_TrackIdx < 0 || _TrackIdx >= m_vecTracks.size()) return nullptr;

    vector<UIAnimKeyFrameData>& KeyFrames = m_vecTracks[_TrackIdx].KeyFrames;

    for (vector<UIAnimKeyFrameData>::iterator it = KeyFrames.begin(); it != KeyFrames.end(); ++it)
    {
        if (_KeyFrameTime > it->Time) continue;
        if (_KeyFrameTime == it->Time) return nullptr; // 동시간대의 키프레임이 이미 존재한다면, 중복된 키프레임 추가 불가
        
        // 삽입될 위치가 여기다
        // 삽입 시, 이전 값과 동일한 it로 생성을 한다
        vector<UIAnimKeyFrameData>::iterator PrevIt = prev(it);
        UIAnimKeyFrameData NewData{};
        NewData.Time       = _KeyFrameTime;
        NewData.Transform  = PrevIt->Transform->Clone();
        NewData.TintColor  = PrevIt->TintColor;
        NewData.EasingType = PrevIt->EasingType;
        
        it = KeyFrames.insert(it, NewData);
        return &*it;
    }

    // 마지막 키프레임으로 추가되었을 때
    UIAnimKeyFrameData NewData{};
    NewData.Time      = _KeyFrameTime;
    NewData.Transform = KeyFrames.back().Transform->Clone();
    NewData.TintColor = KeyFrames.back().TintColor;
    KeyFrames.push_back(NewData);
    
    return &KeyFrames.back();
}

bool CUIAnimation::RemoveKeyFrame(int _TrackIdx, int _KeyFrameIdx)
{
    if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::STOP) return false;
    if (_TrackIdx < 0 || _TrackIdx >= m_vecTracks.size()) return false;
    
    vector<UIAnimKeyFrameData>& KeyFrames = m_vecTracks[_TrackIdx].KeyFrames;
    if (_KeyFrameIdx < 0 || _KeyFrameIdx >= KeyFrames.size()) return false;
    
    if (KeyFrames.size() <= 1) return false; // 최소 하나의 키프레임은 있어야 Track이라고 볼 수 있음 -> 만일 Track 전체 삭제를 원한다면, Track 삭제로 처리할 것

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
    sort(KeyFrames.begin(), KeyFrames.end(), [](const UIAnimKeyFrameData& a, const UIAnimKeyFrameData& b) {
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

bool CUIAnimation::Play(UIAnimEndHandling _EndHandlingType)
{
    if (m_vecTracks.empty()) return false;
    
    for (UIAnimTrack& Track : m_vecTracks)
    {
        // 재생 불가능한 Track이 껴 있을 수 있음 ( ex) 키 프레임이 하나도 없는 경우 )
        // 모든 트랙이 재생 가능한 상태여야 Play 처리 가능하다고 판단
        if (!Track.OnPlayStart()) return false;
    }
    
    // Animation 관련 값 초기 setting으로 조정
    m_AnimTimer   = 0.f;
    m_bIsPlaying  = true;
    m_EndHandling = _EndHandlingType;
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
    // 저장 시에는 무조건 Stop처리된 상태에서 저장
    // 재생 상태에서 저장해버리면, 원본 GO가 수정된 값으로 저장되어버리게 된다
    // 만일 재생중인 상태에서 저장 요청이 들어간 경우, Stop처리를 한 뒤, 다시금 Level에 재저장 요청 처리
    
    // 만일 다른 Animation에서 동일한 GO를 건드린 상황(Play 중)이라면, 이건 또 어떻게 처리를 할 것인가에 대한 문제도 있음
    // 위의 상황은 다른 Animation 내에서도 동일한 처리로 똑같이 다시 Save 요청이 들어가기 때문에 알아서 처리가 됨
    if (IsPlaying())
    {
        Stop();
        RequestLevelToRetrySave();
        return; // 어차피 다시금 저장 처리가 되면서 저장될 예정
    }
    
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
        AnimTrack.LoadFromLevelFile(_File); // 여기서 Track GameObjectRefHolder의 GUID만 복원 처리(아직 GO가 초기화되지 않은 상태일 수 있음)
        
        // 복원된 GUID가 Valid한 GUID라면, 원본 데이터 자리 미리 차지(첫 AddOriginalStateData를 여기서 처리)
        // 첫 AddOriginalStateData 처리를 여기랑, AddObjectToAnimate에서 해주어야 OriginalStateDataCount가 정확히 잡힘
        if (!AnimTrack.AddSelfDefaultOriginalStateData())
            DebugUtil::AddDebugLog("[CUIAnimation::LoadFromLevelFile] : Failed to AddSelfDefaultOriginalStateData");
        
        m_vecTracks.push_back(move(AnimTrack));
    }
}

void CUIAnimation::OnRemoveScript()
{
    CScript::OnRemoveScript();
    Stop();
    
    // Owner GO의 EditingTick 등록 해제해야하는지 체크 (다른 Script에서 EditingTickEnabled 옵션을 켜놨을 수 있음)
    for (const Ptr<CScript>& Script : GetOwner()->GetScripts())
        if (Script != this && Script->GetIsUseEditingTick()) return;

    // 모든 Track에 대해, OnRemove 처리를 해준다. (원본값 관련 처리를 위해)
    for (const UIAnimTrack& Track : m_vecTracks)
    {
        if (!Track.ReduceSelfOriginalStateData())
            DebugUtil::AddDebugLog("[CUIAnimation::OnRemoveScript] : Failed to ReduceSelfOriginalStateData");
    }
    
    // 다른 Script에서 EditingTickEnable옵션을 사용하지 않는다면, 이 GameObject에 대해 EditingTick 비활성화
    DeRegisterEditingTickEnabled();
}

void CUIAnimation::OnOwnerDestroy()
{
    Stop();
    
    // Owner GO의 EditingTick 등록 해제해야하는지 체크 (다른 Script에서 EditingTickEnabled 옵션을 켜놨을 수 있음)
    for (const Ptr<CScript>& Script : GetOwner()->GetScripts())
        if (Script != this && Script->GetIsUseEditingTick()) return;
    
    for (const UIAnimTrack& Track : m_vecTracks)
    {
        if (!Track.ReduceSelfOriginalStateData())
            DebugUtil::AddDebugLog("[CUIAnimation::OnOwnerDestroy] : Failed to ReduceSelfOriginalStateData");
    }
    
    // 다른 Script에서 EditingTickEnable옵션을 사용하지 않는다면, 이 GameObject에 대해 EditingTick 비활성화
    // 오브젝트 자체가 삭제처리될 때에도, ALevel에는 EditingTick GO Set에 해당 GameObject가 들어가있는 상황
    // 이걸 지우기 위해서 DeRegisterEditingTick처리를 해주어야 함
    DeRegisterEditingTickEnabled(); 
}
