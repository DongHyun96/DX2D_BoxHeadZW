#pragma once

#include "GameEngine/05.GameObject/GameObjectRefHolder.h"

enum class UIAnimEasingType
{
    LINEAR,     // 선형 보간
    EASE_OUT    // 초반 빠름, 후반 점점 느려짐 처리
};

enum class TintColorControl
{
    NONE,                   // TintColor 옵션 none (아무 색상도 건드리지 않는다)
    TEXT_COLOR,             // Text TintColor 수정
    RENDERCOM_TINTCOLOR,    // RenderComponent의 TintColor 변경
};

struct UIAnimKeyFrameData
{
    float               Time{};                       // 키프레임 시간대 (-1.f인 경우, 원본 GO의 데이터)
    Ptr<CTransform>     Transform{};
    Vec4                TintColor  = DEF_COLOR_WHITE; // Text GameObject의 경우, Text의 TintColor값으로 적용됨(주의) -> CText나 RenderComponent를 따로 가지고 있지 않을 경우 처리 x   
    
    UIAnimEasingType EasingType{};

public:
    /// <summary>
    /// GameObject로부터 AnimData 받아서 자신의 값으로 세팅
    /// </summary>
    /// <returns> GameObject가 Valid하지 않다면 return false </returns>
    bool SetAnimDataFromGameObject(GameObject* _GameObject, float _Time);
    
    void SaveToLevelFile(FILE* _File);
    void LoadFromLevelFile(FILE* _File);
};


enum class UI_ANIM_CUR_KEYFRAME_UPDATE_TYPE
{
    DEFAULT,        // Play 시에 동작하는 KeyFrame 업데이트 전략 Type
    EDITING_STOP    // Editing 상태 & Animation 멈춘 상태에서 현재 AnimTimer를 옮길 때의 업데이트 처리
};

/// <summary>
/// Stop 상황 testing 및 Play 상태에서의 KeyFrameIndex를 업데이트 처리하는 방식이 다름 (Play시에 더 성능적으로 좋음) -> 전략객체로 뺌
/// </summary>
struct UpdateUIAnimKeyIndexStrategy : public Entity
{
    virtual bool UpdateCurPlayingIndex(float _AnimTimer, int& _CurPlayingKeyIndex, const vector<UIAnimKeyFrameData>& _KeyFrames) = 0;
};

struct PlayingStateUpdateIndexStrategy : public UpdateUIAnimKeyIndexStrategy
{
    virtual bool UpdateCurPlayingIndex(float _AnimTimer, int& _CurPlayingKeyIndex, const vector<UIAnimKeyFrameData>& _KeyFrames) override;
};

struct StopStateEditorUpdateIndexStrategy : public UpdateUIAnimKeyIndexStrategy
{
    virtual bool UpdateCurPlayingIndex(float _AnimTimer, int& _CurPlayingKeyIndex, const vector<UIAnimKeyFrameData>& _KeyFrames) override;
};


/// <summary>
/// 하나의 GameObject를 담당하는 애니메이션 트랙
/// </summary>
struct UIAnimTrack
{
    GameObjectRefHolder         TargetObjectReference{};        // 이 Track을 사용할 GameObject
    
public:
    
    UIAnimKeyFrameData          OriginalStateData{};            // 원본 데이터 저장
    vector<UIAnimKeyFrameData>  KeyFrames{};                    // 시간 순서에 따라 정렬된 키프레임들

private: // Play 시작 시, 또는 Play 중 쓰일 값들

    int                         CurPlayingKeyIndex{};              // 현재 재생중인 Keyframe Index
    bool                        bIsPlaying{};
    
    // Animate 시작 시, RenderCom과 Text 존재 여부를 따져서 Text 먼저 있다면 TextColor를 조정 / 다음으로 RenderCom이 있다면 RenderCom의 TintColor 조정
    // 둘 다 없다면 none으로 주고 TintColor에 대한 Animation 처리 x
    TintColorControl            TintColorControlType{};
    
private:

    class CText*                TargetText{};
    AMaterial*                  TargetMaterial{};

private:

    static map<UI_ANIM_CUR_KEYFRAME_UPDATE_TYPE, Ptr<UpdateUIAnimKeyIndexStrategy>> s_mapUpdateAnimKeyIndexStrategies;
    UpdateUIAnimKeyIndexStrategy* m_UpdateUIKeyIndexStrategy = s_mapUpdateAnimKeyIndexStrategies[UI_ANIM_CUR_KEYFRAME_UPDATE_TYPE::DEFAULT].Get();

public:
    
    UIAnimTrack() = default;
    ~UIAnimTrack() = default;
    UIAnimTrack(const UIAnimTrack& _Origin);
    
public:
    
    void SaveToLevelFile(FILE* _File);
    void LoadFromLevelFile(FILE* _File);

    /// <summary>
    /// Play 시작 시 호출
    /// </summary>
    /// <returns> Play할 수 없는 Track이라면 return false </returns>
    bool OnPlayStart();

    /// <summary>
    /// Play 중일 때 호출
    /// </summary>
    void WhilePlaying(float _AnimTimer);

    /// <summary>
    /// 재생중인 Track stop
    /// </summary>
    void Stop();
    
public:
    
    /// <summary>
    /// AnimTimer에 따른 Track 상태 Update (Track Testing 용으로 필요 & Play 시에도 해당 함수 사용) 
    /// </summary>
    void UpdateTrack(float _AnimTimer);
    
    void ManuallyUpdateTintColorControlType();

public:
    
    bool GetIsPlaying() const { return bIsPlaying; }
    
};