#include "pch.h"
#include "CUIAnimation.h"
#include "Source/Scripts/UIScript/CText.h"


bool UIAnimKeyFrameData::SetAnimDataFromGameObject(GameObject* _GameObject, float _Time)
{
    if (!_GameObject) return false;
    if (!_GameObject->Transform()) return false;    
    
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
    fwrite(&bUseLerp, sizeof(bool), 1, _File);
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
    fread(&bUseLerp, sizeof(bool), 1, _File);
}















// 여기서는 GUID 복사만 일어남 (AfterLevelGameObjectGuidTableInit 시점에 GameObject ref 제대로 잡아주어야 함)
UIAnimTrack::UIAnimTrack(const UIAnimTrack& _Origin)
    : TargetObjectReference(_Origin.TargetObjectReference)
    , OriginalStateData(_Origin.OriginalStateData)
    , KeyFrames(_Origin.KeyFrames)
    // 나머지 멤버변수는 초기값으로 둠 (Editing 환경에서 Play 상태로 테스팅 중이었던 상태 복구처리)
{
}

void UIAnimTrack::SaveToLevelFile(FILE* _File)
{
    TargetObjectReference.SaveToLevelFile(_File);

    // OriginalStateData의 경우, 원본 GameObject 레퍼런스가 완전히 복원된 이후, 해당 원본값을 불러와서 처리
        
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
        TargetMaterial = RenderCom->GetMaterial().Get();
        TargetMaterial->SetScalar(VEC4_0, KeyFrames[0].TintColor);
    }
    else TintColorControlType = TintColorControl::NONE; 
    
    /* Init Current Key index */
    CurPlayingKeyIndex = 0;
        
    // TargetObject Transform 초기 Setting으로 처리한 뒤, play 시작 처리    
    TargetObject->Transform()->CopyRelativePosScaleRot(KeyFrames[0].Transform);
    
    return true;
}

void UIAnimTrack::WhilePlaying(float AnimTimer)
{
    if (!bIsPlaying) return;
    
    // 새로운 키 프레임 재생해야하는지 체크
    if (AnimTimer > KeyFrames[CurPlayingKeyIndex]. Time) ++CurPlayingKeyIndex; // 다음 프레임 재생으로 넘어감
    
    // 모든 키 프레임 소진 체크 -> 이 Track에 대한 Animation 종료
    if (CurPlayingKeyIndex >= KeyFrames.size())
    {
        CurPlayingKeyIndex = KeyFrames.size();
        bIsPlaying         = false;
        return;
    }

    // TargetObject가 여기서는 무조건 Valid한 상황
    GameObject* TargetObject = TargetObjectReference.GetGameObject();
    
    // 이전 키 프레임과 현재 키 프레임 사이의 Time 간격으로 값 보간 처리
    const UIAnimKeyFrameData& PrevKeyFrame      = KeyFrames[CurPlayingKeyIndex - 1];
    const UIAnimKeyFrameData& CurrentKeyFrame   = KeyFrames[CurPlayingKeyIndex];
    
    const float TimeDiff            = CurrentKeyFrame.Time - PrevKeyFrame.Time;
    const float CurrentKeyFrameTime = AnimTimer - PrevKeyFrame.Time; // 이전 키 프레임과 현재 키 프레임 사이의 시간 흐름
    const float TimeAlpha           = MappingToNewRange(CurrentKeyFrameTime, 0.f, TimeDiff, 0.f, 1.f);
    
    // Transform S, R, T 사이 보간 처리 (다른 값 x 오로지 s, r, t만 처리할 것)
    Vec3 TargetPos   = TargetObject->Transform()->GetRelativePos();
    Vec3 TargetScale = TargetObject->Transform()->GetRelativeScale();
    Vec3 TargetRot   = TargetObject->Transform()->GetRelativeRot();
    
    TargetPos   = Vec3::Lerp(PrevKeyFrame.Transform->GetRelativePos(), CurrentKeyFrame.Transform->GetRelativePos(), TimeAlpha);
    TargetScale = Vec3::Lerp(PrevKeyFrame.Transform->GetRelativeScale(), CurrentKeyFrame.Transform->GetRelativeScale(), TimeAlpha);
    TargetRot   = Vec3::Lerp(PrevKeyFrame.Transform->GetRelativeRot(), CurrentKeyFrame.Transform->GetRelativeRot(), TimeAlpha);
    
    TargetObject->Transform()->SetRelativePos(TargetPos);
    TargetObject->Transform()->SetRelativeScale(TargetScale);
    TargetObject->Transform()->SetRelativeRot(TargetRot);

    switch (TintColorControlType)
    {
    case TintColorControl::TEXT_COLOR:
    {
        Vec4 Color = TargetText->GetColor();
        Color = Vec4::Lerp(PrevKeyFrame.TintColor, CurrentKeyFrame.TintColor,TimeAlpha);
        TargetText->SetColor(Color);
    }
        break;
    case TintColorControl::RENDERCOM_TINTCOLOR:
    {
        Vec4 Color = TargetMaterial->GetScalar<Vec4>(VEC4_0);
        Color = Vec4::Lerp(PrevKeyFrame.TintColor, CurrentKeyFrame.TintColor,TimeAlpha);
        TargetMaterial->SetScalar(VEC4_0, Color);
    }
        break;
    }
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
        TargetText->SetColor(KeyFrames[0].TintColor);
    else if (const Ptr<CRenderComponent>& RenderCom = TargetObject->GetRenderCom())
    {
        TargetMaterial = RenderCom->GetMaterial().Get();
        TargetMaterial->SetScalar(VEC4_0, KeyFrames[0].TintColor);
    }
}
