#include "pch.h"
#include "UIAnimationData.h"

#include <algorithm>

#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "Source/Scripts/UIScript/CText.h"

map<UI_ANIM_CUR_KEYFRAME_UPDATE_TYPE, Ptr<UpdateUIAnimKeyIndexStrategy>> UIAnimTrack::s_mapUpdateAnimKeyIndexStrategies = 
{
    { UI_ANIM_CUR_KEYFRAME_UPDATE_TYPE::DEFAULT,        new PlayingStateUpdateIndexStrategy },
    { UI_ANIM_CUR_KEYFRAME_UPDATE_TYPE::EDITING_STOP,   new StopStateEditorUpdateIndexStrategy }
};

bool UIAnimKeyFrameData::SetAnimDataFromGameObject(GameObject* _GameObject, float _Time)
{
    if (!_GameObject || !_GameObject->Transform()) return false;    
    
    Time = _Time;
    
    Transform = new CTransform;
    Transform->CopyRelativePosScaleRot(_GameObject->Transform());
    
    // Tint Color
    if (const Ptr<CText>& Text = _GameObject->GetScriptComponent<CText>())
        TintColor = Text->GetColor();
    else if (_GameObject->GetRenderCom())
        TintColor = _GameObject->GetRenderCom()->GetMaterial()->GetScalar<Vec4>(VEC4_0);
    
    return true;    
}

void UIAnimKeyFrameData::SaveToLevelFile(FILE* _File)
{
    fwrite(&Time, sizeof(float), 1, _File);

    const bool TransformValid = Transform != nullptr;
    fwrite(&TransformValid, sizeof(bool), 1, _File);
    if (Transform) Transform->SaveToLevelFile(_File);
        
    fwrite(&TintColor, sizeof(Vec4), 1, _File);
    fwrite(&EasingType, sizeof(UIAnimEasingType), 1, _File);
}

void UIAnimKeyFrameData::LoadFromLevelFile(FILE* _File)
{
    fread(&Time, sizeof(float), 1, _File);
        
    bool TransformValid{};
    fread(&TransformValid, sizeof(bool), 1, _File);
    if (TransformValid)
    {
        Transform = new CTransform;
        Transform->LoadFromLevelFile(_File);
    }
        
    fread(&TintColor, sizeof(Vec4), 1, _File);
    fread(&EasingType, sizeof(UIAnimEasingType), 1, _File);
}

bool PlayingStateUpdateIndexStrategy::UpdateCurPlayingIndex(float _AnimTimer, int& _CurPlayingKeyIndex, const vector<UIAnimKeyFrameData>& _KeyFrames)
{
    // 새로운 키 프레임 재생해야하는지 체크
    if (_AnimTimer > _KeyFrames[_CurPlayingKeyIndex].Time) ++_CurPlayingKeyIndex; // 다음 프레임 재생으로 넘어감
    return true;
}

bool StopStateEditorUpdateIndexStrategy::UpdateCurPlayingIndex(float _AnimTimer, int& _CurPlayingKeyIndex, const vector<UIAnimKeyFrameData>& _KeyFrames)
{
    if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::STOP) return false;
    if (_KeyFrames.empty()) return false;

    for (int i = 0; i < _KeyFrames.size(); ++i)
    {
        // 타임라인 시간이 현재 검사하는 키프레임보다 작거나 같으면 그곳이 목표 인덱스
        if (_AnimTimer <= _KeyFrames[i].Time)
        {
            _CurPlayingKeyIndex = i; 
            return true;
        }
    }

    // 마우스를 마지막 키프레임보다 더 우측으로 끌었을 때
    _CurPlayingKeyIndex = _KeyFrames.size() - 1;
    return true;
}







UIAnimTrack::UIAnimTrack(const UIAnimTrack& _Origin)
    : TargetObjectReference(_Origin.TargetObjectReference) // 여기서는 GUID 복사만 일어남 (AfterLevelGameObjectGuidTableInit 시점에 GameObject ref 제대로 잡아주어야 함)
    , OriginalStateData(_Origin.OriginalStateData)
    , KeyFrames(_Origin.KeyFrames)
    // 나머지 멤버변수는 초기값으로 둠 (Editing 환경에서 Play 상태로 테스팅 중이었던 상태 복구처리)
{
}

UIAnimTrack::UIAnimTrack(UIAnimTrack&& _Origin) noexcept
    : TargetObjectReference(move(_Origin.TargetObjectReference)) // 여기서는 원본 GO 레퍼런스 모두 이동 (GUID 뿐 아니라 GO 포인터까지)
    , OriginalStateData(move(_Origin.OriginalStateData))
    , KeyFrames(move(_Origin.KeyFrames))
    , m_UpdateUIKeyIndexStrategy(_Origin.m_UpdateUIKeyIndexStrategy)
{
    // 곧 소멸할 객체에 대해, 안전 처리
    _Origin.TargetText                 = nullptr;
    _Origin.TargetMaterial             = nullptr;
    _Origin.m_UpdateUIKeyIndexStrategy = nullptr;
}

UIAnimTrack& UIAnimTrack::operator=(const UIAnimTrack& _Other)
{
    if (this == &_Other) return *this; 

    TargetObjectReference = _Other.TargetObjectReference;
    OriginalStateData     = _Other.OriginalStateData;
    KeyFrames             = _Other.KeyFrames;

    // 2. 런타임 상태 초기화 (복사 생성자와 동일한 로직 적용)
    CurPlayingKeyIndex   = 0;
    bIsPlaying           = false;
    TintColorControlType = TintColorControl::NONE;
    TargetText           = nullptr;
    TargetMaterial       = nullptr;
    m_UpdateUIKeyIndexStrategy = s_mapUpdateAnimKeyIndexStrategies[UI_ANIM_CUR_KEYFRAME_UPDATE_TYPE::DEFAULT].Get();

    return *this;
}

UIAnimTrack& UIAnimTrack::operator=(UIAnimTrack&& _Other) noexcept
{
    if (this == &_Other) return *this; 

    TargetObjectReference = move(_Other.TargetObjectReference);
    OriginalStateData     = move(_Other.OriginalStateData);
    KeyFrames             = move(_Other.KeyFrames);

    CurPlayingKeyIndex   = 0;
    bIsPlaying           = false;
    TintColorControlType = TintColorControl::NONE;
    TargetText           = nullptr;
    TargetMaterial       = nullptr;
    m_UpdateUIKeyIndexStrategy = s_mapUpdateAnimKeyIndexStrategies[UI_ANIM_CUR_KEYFRAME_UPDATE_TYPE::DEFAULT].Get();

    // 3. 뺏긴 원본 객체의 포인터 안전하게 비우기
    _Other.TargetText                 = nullptr;
    _Other.TargetMaterial             = nullptr;
    _Other.m_UpdateUIKeyIndexStrategy = nullptr;

    return *this;
}

void UIAnimTrack::SaveToLevelFile(FILE* _File)
{
    TargetObjectReference.SaveToLevelFile(_File); // GUID만 저장 처리됨
    
    // 이걸 여기서 저장 처리를 다시 해버리면, 순서가 꼬이게 됨 (GameObject Save -> Component Save -> Script Save 순으로 저장 처리가 기본 순서)
    // if (TargetObjectReference.GetGameObject()) TargetObjectReference.GetGameObject()->SaveToLevelFile(_File); 
        
    // 키프레임 개수 저장
    const int KeyFrameCount = KeyFrames.size();
    fwrite(&KeyFrameCount, sizeof(int), 1, _File);

    // 키프레임 데이터들 저장
    for (UIAnimKeyFrameData& KeyFrame : KeyFrames)
        KeyFrame.SaveToLevelFile(_File);
}

void UIAnimTrack::LoadFromLevelFile(FILE* _File)
{
    // 원본 TargetObject reference의 GUID 복원 -> 실질적인 GameObject 포인터 값은 Level에 GameObject가 배치된 이후 연결 처리
    TargetObjectReference.LoadFromLevelFile(_File);

    // OriginalStateData의 경우, 원본 GameObject 레퍼런스가 완전히 복원된 이후, 해당 원본값을 불러와서 처리

    // 키프레임 수 불러와서, 불러온 갯수 만큼 키프레임들 복원
    int KeyFrameCount{};
    fread(&KeyFrameCount, sizeof(int), 1, _File);
        
    KeyFrames.resize(KeyFrameCount);
    for (UIAnimKeyFrameData& KeyFrame : KeyFrames)
        KeyFrame.LoadFromLevelFile(_File);
}

bool UIAnimTrack::OnPlayStart()
{
    // Valid하지 않은 GameObject를 들고 있다면 재생 불가
    GameObject* TargetObject = TargetObjectReference.GetGameObject(); 
    if (!TargetObject) return false;
    
    // 키 프레임이 하나도 없다면 재생 불가
    if (KeyFrames.empty()) return false;
    
    bIsPlaying = true;
    
    /* Init tint color control type & TargetObject Color 0번 키프레임 Color로 세팅 처리 */
    if (TargetText = TargetObject->GetScriptComponent<CText>().Get())
    {
        TintColorControlType = TintColorControl::TEXT_COLOR;
        TargetText->SetColor(KeyFrames[0].TintColor);
    }
    else if (const Ptr<CRenderComponent>& RenderCom = TargetObject->GetRenderCom())
    {
        TintColorControlType = TintColorControl::RENDERCOM_TINTCOLOR;
        TargetMaterial       = RenderCom->CreateDynamicMaterial().Get();
        TargetMaterial->SetScalar(VEC4_0, KeyFrames[0].TintColor);
    }
    else TintColorControlType = TintColorControl::NONE; 
    
    /* Init Current Key index */
    CurPlayingKeyIndex = 0;
        
    // TargetObject Transform 초기 Setting으로 처리한 뒤, play 시작 처리    
    TargetObject->Transform()->CopyRelativePosScaleRot(KeyFrames[0].Transform);

    /* Init CurKeyFrame Index Update Strategy */
    m_UpdateUIKeyIndexStrategy = s_mapUpdateAnimKeyIndexStrategies[UI_ANIM_CUR_KEYFRAME_UPDATE_TYPE::DEFAULT].Get();
    
    return true;
}

void UIAnimTrack::WhilePlaying(float _AnimTimer)
{
    if (!bIsPlaying) return;
    UpdateTrack(_AnimTimer);
}

void UIAnimTrack::UpdateTrack(float _AnimTimer)
{
    // 키 프레임 업데이트
    // Editing 상태에서의 Stop 상황 & Normal Play 상황에 따른 Index 업데이트 처리가 다르기 때문에 전략 패턴으로 CurPlayingKeyIndex 업데이트 처리를 함
    m_UpdateUIKeyIndexStrategy->UpdateCurPlayingIndex(_AnimTimer, CurPlayingKeyIndex, KeyFrames);
    
    // 모든 키 프레임 소진 체크 -> 이 Track에 대한 Animation 종료
    if (CurPlayingKeyIndex >= KeyFrames.size())
    {
        CurPlayingKeyIndex = KeyFrames.size();
        bIsPlaying         = false;
        return;
    }

    // TargetObject가 여기서는 무조건 Valid한 상황
    GameObject* TargetObject = TargetObjectReference.GetGameObject();

    // 현재 KeyFrame이 First KeyFrame인 경우에도 예외처리를 해주어야 함
    const UIAnimKeyFrameData& PrevKeyFrame      = (CurPlayingKeyIndex == 0) ? KeyFrames[CurPlayingKeyIndex] : KeyFrames[CurPlayingKeyIndex - 1];
    const UIAnimKeyFrameData& CurrentKeyFrame   = KeyFrames[CurPlayingKeyIndex];
    
    // 이전 키 프레임과 현재 키 프레임 사이의 Time 간격으로 값 보간 처리

    const float TimeDiff            = CurrentKeyFrame.Time - PrevKeyFrame.Time;
    const float CurrentKeyFrameTime = _AnimTimer - PrevKeyFrame.Time; // 이전 키 프레임과 현재 키 프레임 사이의 시간 흐름
    float TimeAlpha                 = MappingToNewRangeClamped(CurrentKeyFrameTime, 0.f, TimeDiff, 0.f, 1.f);
    
    switch (CurrentKeyFrame.EasingType)
    {
    /*case UIAnimEasingType::LINEAR:
        break;*/
    case UIAnimEasingType::EASE_OUT:
        // Quadratic Ease-Out : 1 - (1 - t) * (1 - t)
        const float Temp = 1.f - TimeAlpha;
        TimeAlpha = 1.f - Temp * Temp;
        break;
    }
    
    // Transform S, R, T 사이 보간 처리 (다른 값 x 오로지 s, r, t만 처리할 것)
    Vec3 TargetPos   = Lerp(PrevKeyFrame.Transform->GetRelativePos(), CurrentKeyFrame.Transform->GetRelativePos(), TimeAlpha);
    Vec3 TargetScale = Lerp(PrevKeyFrame.Transform->GetRelativeScale(), CurrentKeyFrame.Transform->GetRelativeScale(), TimeAlpha);
    Vec3 TargetRot   = Lerp(PrevKeyFrame.Transform->GetRelativeRot(), CurrentKeyFrame.Transform->GetRelativeRot(), TimeAlpha);
    
    TargetObject->Transform()->SetRelativePos(TargetPos);
    TargetObject->Transform()->SetRelativeScale(TargetScale);
    TargetObject->Transform()->SetRelativeRot(TargetRot);

    switch (TintColorControlType)
    {
    case TintColorControl::TEXT_COLOR:
    {
        Vec4 Color = Lerp(PrevKeyFrame.TintColor, CurrentKeyFrame.TintColor,TimeAlpha);
        TargetText->SetColor(Color);
    }
        break;
    case TintColorControl::RENDERCOM_TINTCOLOR:
    {
        Vec4 Color = Lerp(PrevKeyFrame.TintColor, CurrentKeyFrame.TintColor,TimeAlpha);
        TargetMaterial->SetScalar(VEC4_0, Color);
    }
        break;
    }
}

void UIAnimTrack::ManuallyUpdateTintColorControlType()
{
    GameObject* TargetObject = TargetObjectReference.GetGameObject();
    if (!TargetObject) return;
    
    /* Init tint color control type & TargetObject Color 0번 키프레임 Color로 세팅 처리 */
    if (TargetText = TargetObject->GetScriptComponent<CText>().Get())
        TintColorControlType = TintColorControl::TEXT_COLOR;
    else if (const Ptr<CRenderComponent>& RenderCom = TargetObject->GetRenderCom())
    {
        TargetMaterial       = RenderCom->CreateDynamicMaterial().Get();
        TintColorControlType = TintColorControl::RENDERCOM_TINTCOLOR;
    }
    else TintColorControlType = TintColorControl::NONE;     
}

void UIAnimTrack::Stop()
{
    CurPlayingKeyIndex    = 0;
    bIsPlaying            = false;
    TargetText            = nullptr;
    TargetMaterial        = nullptr;
    TintColorControlType  = TintColorControl::NONE;
    
    GameObject* TargetObject = TargetObjectReference.GetGameObject();
    if (!TargetObject) return;
    
    TargetObject->Transform()->CopyRelativePosScaleRot(OriginalStateData.Transform);

    // 굳이 ControlType을 쓰지 않은 이유 -> Play 처리 안한 상태에서 Stop을 한 상황일 수 있음
    if (TargetText = TargetObject->GetScriptComponent<CText>().Get())
        TargetText->SetColor(OriginalStateData.TintColor);
    else if (const Ptr<CRenderComponent>& RenderCom = TargetObject->GetRenderCom())
    {
        TargetMaterial = RenderCom->GetMaterial().Get();
        TargetMaterial->SetScalar(VEC4_0, OriginalStateData.TintColor);
    }
    
    /* Init CurKeyFrame Index Update Strategy */
    if (LevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::STOP)
        m_UpdateUIKeyIndexStrategy = s_mapUpdateAnimKeyIndexStrategies[UI_ANIM_CUR_KEYFRAME_UPDATE_TYPE::EDITING_STOP].Get();
    else  m_UpdateUIKeyIndexStrategy = s_mapUpdateAnimKeyIndexStrategies[UI_ANIM_CUR_KEYFRAME_UPDATE_TYPE::DEFAULT].Get();
}
