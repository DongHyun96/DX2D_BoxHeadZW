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
    Vec4                TintColor = DEF_COLOR_WHITE; // Text GameObject의 경우, Text의 TintColor값으로 적용됨(주의) -> CText나 RenderComponent를 따로 가지고 있지 않을 경우 처리 x   
    
    UIAnimEasingType EasingType{};

public:
    
    UIAnimKeyFrameData();
    ~UIAnimKeyFrameData();
    
    UIAnimKeyFrameData(const UIAnimKeyFrameData& _Origin);
    UIAnimKeyFrameData(UIAnimKeyFrameData&& _Origin) noexcept;
    
    UIAnimKeyFrameData& operator=(const UIAnimKeyFrameData& _Other);
    UIAnimKeyFrameData& operator=(UIAnimKeyFrameData&& _Other) noexcept;
    
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
    vector<UIAnimKeyFrameData>  KeyFrames{};                    // 시간 순서에 따라 정렬된 키프레임들
    
private:
    
    static unordered_map<GUID, UIAnimKeyFrameData, GUIDHasher>    ORIGINAL_STATE_DATA;        // 원본 데이터 저장 (같은 오브젝트를 서로 다른 AnimationTrack이 관여할 때, 원본값은 항상 동일해야 한다)
    static unordered_map<GUID, int, GUIDHasher>                   ORIGINAL_STATE_DATA_COUNT;  // 한Track당(GUID 당), 원본 데이터값 저장을 몇개 했는지 기록

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

    /// <summary>
    /// 기존에는 GORefHolder에서 GUID 복사만 처리하고 AfterLevelGameObjectGuidTableInit 시점에 GO Ref 연결 처리를 해주었음
    /// Editing 상태에서 CUIAnimation 복사 생성이 일어나면(Animation GO Duplicate), 정확히 복사 처리를 해주어야 해서, GO Ref 연결 처리까지도 추가
    /// </summary>
    UIAnimTrack(const UIAnimTrack& _Origin);
    
    UIAnimTrack(UIAnimTrack&& _Origin) noexcept;
    
    UIAnimTrack& operator=(const UIAnimTrack& _Other);
    UIAnimTrack& operator=(UIAnimTrack&& _Other) noexcept;
        
public:

    static bool HasOriginalStateData(const GUID& _GUID) { return ORIGINAL_STATE_DATA.contains(_GUID); }

    /// <summary>
    /// 기존에 이미 추가된 OriginalData값 수정 
    /// </summary>
    /// <param name="_GUID"> : 수정할 GO</param>
    /// <param name="_ModifiedData"> : 수정된 데이터 </param>
    /// <returns> : 만약 기존에 추가한 OriginalData가 없다면 return false -> AddOriginalStateData로 최초 추가를 해주어야 비로소 이용 가능</returns>
    // TODO : 해당 처리해야할 부분에서 호출해줄 것
    static bool ModifyOriginalStateData(const GUID& _GUID, const UIAnimKeyFrameData& _ModifiedData);

    /// <summary>
    /// 해당 GUID의 원본 KeyFrame값 return
    /// </summary>
    /// <param name="_GUID"> : Target GUID </param>
    /// <param name="_OutOriginalData"> : 반환받을 OriginalData KeyFrame </param>
    /// <returns> : 해당 키가 없다면, return false </returns>
    static bool GetOriginalKeyFrameData(const GUID& _GUID, OUT UIAnimKeyFrameData& _OutOriginalData);

private:

    /// <summary>
    /// Original Data 추가 (주의, 기존에 추가된(다른 Animation Track에서도 참조중인) GO의 원본값에 대한 RefCount(정확히는 Origin Count) 개수가 올라감
    /// 만약 기존 원본 자료가 있다면, 기존의 원본 자료값을 이용하도록 처리한다
    /// </summary>
    static void AddOriginalStateData(const GUID& _GUID, const UIAnimKeyFrameData& _OriginalData);
    
    /// <summary>
    /// <para> OriginalStateData Count 줄이기 </para>
    /// <para> 만일 Count가 0이 되면, 원본값 폐기(참조중인 Track이 없는 것으로 간주) </para>
    /// </summary>
    /// <returns> : 원본값 자체가 저장이 안되어 있는 경우, return false (애초에 나오지 않아야 하는 상황이긴 함) </returns>
    static bool ReduceOriginalStateData(const GUID& _GUID);

public:
    
    /// <summary>
    /// <para> 자기 자신의 원본값을 기록하면서, OriginalStateData Count++ 처리 </para>
    /// <para> OriginalStateData KeyFrame 자체는 Default 값이 들어가게 된다 -> 추후 이 값을 수정하려면 ModifyOriginalStateData 함수 사용할 것 </para>
    /// </summary>
    /// <returns> 자기 자신의 ObjectReference의 GUID가 Valid한 GUID가 아니라면 return false </returns>
    bool AddSelfDefaultOriginalStateData() const;
    
    /// <summary>
    /// 자기 자신의 원본값을 기록하면서, OriginalStateData Count++ 처리
    /// </summary>
    /// <param name="_OriginalData"> : 기록할 원본값 </param>
    /// <returns> : 자기자신의 GUID 가 valid하지 않다면 return false </returns>
    bool AddSelfOriginalStateData(const UIAnimKeyFrameData& _OriginalData) const;

    /// <summary>
    /// 자기자신의 원본값 Count만 하나 키우기
    /// </summary>
    /// <returns> : 만약 Map에 자기자신의 GUID가 없는 상황이라면 return false </returns>
    bool TryAddSelfOriginalStateDataCount() const;
    
public:
    
    /// <summary>
    /// <para> 자기 자신의 원본값 Count 하나 줄이기 </para>
    /// <para> 만약 Count가 0이 되면 해당 원본값 삭제 </para>
    /// </summary>
    /// <returns> : 원본값 자체가 없는 경우, return false </returns>
    bool ReduceSelfOriginalStateData() const { return ReduceOriginalStateData(TargetObjectReference.GetRefGUID()); }
    
public:
    
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
    
public:
    
    void SaveToLevelFile(FILE* _File);
    void LoadFromLevelFile(FILE* _File);
    
};