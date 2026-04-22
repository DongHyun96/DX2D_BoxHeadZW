#include "pch.h"
#include "CUIAnimation.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
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

    // 모든 Animation 종료되었는지 체크
    if (m_ObjectAnimFinishedCount >= m_mapTracks.size())
    {
        // 종료 처리
        if (m_bBackToStopOnAnimEnd) Stop();
        return;
    }
    
    m_AnimTimer += DT;

    for (pair<GameObject* const, UIAnimTrack>& Pair : m_mapTracks)
    {
        GameObject* TargetObject                    = Pair.first;
        const vector<UIAnimKeyFrameData>& KeyFrames = Pair.second.KeyFrames;
        int& CurrentKeyIndex                        = Pair.second.CurrentKeyIndex;
        
        // 새로운 키 프레임 재생해야하는지 체크
        if (m_AnimTimer > KeyFrames[CurrentKeyIndex].Time) ++CurrentKeyIndex; // 다음 키 프레임 재생으로 넘어감

        // 모든 키프레임 소진 -> 이 GameObject에 대한 Animation 종료
        if (CurrentKeyIndex >= KeyFrames.size())
        {
            ++m_ObjectAnimFinishedCount;
            return;
        }
        
        // 이전 키 프레임과 현재 키 프레임 사이의 Time 간격으로 값 보간 처리
        const UIAnimKeyFrameData& PrevKeyFrame      = KeyFrames[CurrentKeyIndex - 1];
        const UIAnimKeyFrameData& CurrentKeyFrame   = KeyFrames[CurrentKeyIndex];
        
        const float TimeDiff            = CurrentKeyFrame.Time - PrevKeyFrame.Time;
        const float CurrentKeyFrameTime = m_AnimTimer - PrevKeyFrame.Time; // 이전 키 프레임과 현재 키 프레임 사이의 시간 흐름
        const float TimeAlpha           = MappingToNewRange(CurrentKeyFrameTime, 0.f, TimeDiff, 0.f, 1.f);
        
        // Transform S, R, T 사이 보간 처리
        Vec3 TargetPos   = TargetObject->Transform()->GetRelativePos();
        Vec3 TargetScale = TargetObject->Transform()->GetRelativeScale();
        Vec3 TargetRot   = TargetObject->Transform()->GetRelativeRot();
        
        TargetPos   = Vec3::Lerp(PrevKeyFrame.Transform->GetRelativePos(), CurrentKeyFrame.Transform->GetRelativePos(), TimeAlpha);
        TargetScale = Vec3::Lerp(PrevKeyFrame.Transform->GetRelativeScale(), CurrentKeyFrame.Transform->GetRelativeScale(), TimeAlpha);
        TargetRot   = Vec3::Lerp(PrevKeyFrame.Transform->GetRelativeRot(), CurrentKeyFrame.Transform->GetRelativeRot(), TimeAlpha);
        
        TargetObject->Transform()->SetRelativePos(TargetPos);
        TargetObject->Transform()->SetRelativeScale(TargetScale);
        TargetObject->Transform()->SetRelativeRot(TargetRot);
        
        if (Ptr<CText> Text = TargetObject->GetScriptComponent<CText>())
        {
            Vec4 Color = Text->GetColor();
            Color = Vec4::Lerp(PrevKeyFrame.TintColor, CurrentKeyFrame.TintColor, TimeAlpha);
            Text->SetColor(Color);
        }
        else if (TargetObject->GetRenderCom())
        {
            Vec4 Color = TargetObject->GetRenderCom()->GetMaterial()->GetScalar<Vec4>(VEC4_0);
            Color = Vec4::Lerp(PrevKeyFrame.TintColor, CurrentKeyFrame.TintColor, TimeAlpha);
            TargetObject->GetRenderCom()->GetMaterial()->SetScalar(VEC4_0, Color);
        }
    }
}

bool CUIAnimation::AddGameObjectToAnimate(GameObject* _GameObject)
{
    if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::STOP) return false;
    if (m_mapTracks.contains(_GameObject)) return false;
    if (!_GameObject->Transform()) return false; // 최소 Transform 정보는 있어야 Animation 가능
    
    // 초기 default 원본 기록 -> Animation Stop시, 해당 설정으로 돌아가기 위함
    m_mapTracks.insert(make_pair(_GameObject, UIAnimTrack()));
    
    UIAnimKeyFrameData DefaultData{};
    
    DefaultData.Time      = -1.f; // 초기 Time은 -1.f
    DefaultData.Transform = _GameObject->Transform()->Clone();
    
    if (Ptr<CText> Text = _GameObject->GetScriptComponent<CText>())      DefaultData.TintColor = Text->GetColor();
    else if (Ptr<CRenderComponent> Render = _GameObject->GetRenderCom()) DefaultData.TintColor = Render->GetMaterial()->GetScalar<Vec4>(VEC4_0);
        
    m_mapTracks[_GameObject].KeyFrames.push_back(DefaultData);
    
    // index 1번 기본 Default 키프레임 추가 (여기서부터 editor에 노출)
    UIAnimKeyFrameData FirstData{};
    FirstData      = DefaultData;
    FirstData.Time = 0.f;
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

    m_AnimTimer               = 0.f;
    m_bIsPlaying              = true;
    m_ObjectAnimFinishedCount = 0;
    m_bBackToStopOnAnimEnd    = _bBackToStopOnEnd;
    
    // 초기 setting(index 1) 로 처리해서 start
    for (pair<GameObject* const, UIAnimTrack>& Pair : m_mapTracks)
    {
        GameObject* TargetObject    = Pair.first;
        vector<UIAnimKeyFrameData>& KeyFrames = Pair.second.KeyFrames;
        
        TargetObject->Transform() = KeyFrames[1].Transform;
        
        if (Ptr<CText> Text = TargetObject->GetScriptComponent<CText>()) Text->SetColor(KeyFrames[1].TintColor);
        if (TargetObject->GetRenderCom()) TargetObject->GetRenderCom()->GetMaterial()->SetScalar(VEC4_0, KeyFrames[1].TintColor);
    }
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
        
        if (Ptr<CText> Text = TargetObject->GetScriptComponent<CText>()) Text->SetColor(KeyFrames[0].TintColor);
    }

    m_AnimTimer               = 0.f;
    m_bIsPlaying              = false;
    m_ObjectAnimFinishedCount = 0;
}

void CUIAnimation::SaveToLevelFile(FILE* _File)
{
    // TODO : 어떻게 레벨에 배치된 GameObject 레퍼런스를 저장하고 불러올지 고민해볼 것
    // Load에서 새로 생성 처리를 하면 안됨 -> Level에서 불러온 GameObject로, 다시금 트랙을 복구시켜야 함
}

void CUIAnimation::LoadFromLevelFile(FILE* _File)
{
    
}
