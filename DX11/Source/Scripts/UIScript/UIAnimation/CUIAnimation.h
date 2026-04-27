#pragma once
#include "GameEngine/05.GameObject/GameObjectRefHolder.h"


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
    bool                bUseLerp{};                   // 이전 키 프레임에서 이 키 프레임으로 Animation 전환 처리 시, Lerp를 사용할지 여부

public:
    /// <summary>
    /// GameObject로부터 AnimData 받아서 자신의 값으로 세팅
    /// </summary>
    /// <returns> GameObject가 Valid하지 않다면 return false </returns>
    bool SetAnimDataFromGameObject(GameObject* _GameObject, float _Time);
    
    void SaveToLevelFile(FILE* _File);
    void LoadFromLevelFile(FILE* _File);
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
    void WhilePlaying(float AnimTimer);

    /// <summary>
    /// 재생중인 Track stop
    /// </summary>
    void Stop();

public:
    
    bool GetIsPlaying() const { return bIsPlaying; }
    
};

class CUIAnimation : public CScript
{
private:

    // map<class GameObjectRefHolder, UIAnimTrack> m_mapTracks{};
    vector<UIAnimTrack> m_vecTracks{};

private:
    
    bool m_bIsPlaying{};
    bool m_bBackToStopOnAnimEnd{}; // Animation 재생이 끝났을 때, 바로 Stop 처리할지(원본 데이터로 돌아갈지) 여부
    
    float m_AnimTimer{}; // Animation 재생 Timer
    
public:
    
    CUIAnimation();
    CUIAnimation(const CUIAnimation& _Origin);
    virtual ~CUIAnimation() override;
    CLONE(CUIAnimation);
    
public:

    virtual void AfterLevelGameObjectGuidTableInit() override;
    
    virtual void Tick() override;

public:

    const vector<UIAnimTrack>& GetTracks() const { return m_vecTracks; }
    vector<UIAnimTrack>& GetTracks() { return m_vecTracks; }
    bool IsPlaying() const { return m_bIsPlaying; }
    float GetAnimTimer() const { return m_AnimTimer; }
    
public: // Editing 관련 함수들
    
    /// <summary>
    /// Animate 시킬 GameObject 추가 (UI Script를 들고 있지 않아도 사용 가능)
    /// Animate 시킬 GameObject 본인이어야 한다(자식 객체는 control 불가)
    /// </summary>
    /// <returns> : 이미 해당 GameObject가 추가되어있다면 return false / Level 이 Play 상태라면, return false(Editing 환경에서만 새로이 추가 가능) </returns>
    bool AddGameObjectToAnimate(GameObject* _GameObject);

    /// <summary>
    /// 해당 index 번째 Animation에서 제거
    /// </summary>
    /// <returns> : Editing 상태가 아니거나, index 초과 시 return false </returns>
    bool RemoveTrack(int _TrackIdx);

public:
    /// <summary>
    /// 해당 index Track의 해당 프레임타임에 새로운 키프레임 추가
    /// </summary>
    /// <returns> : 해당 키프레임을 넣을 수 없는 상황이라면 return false </returns>
    bool AddNewKeyFrame(int _TrackIdx, float _KeyFrameTime);

    /// <summary>
    /// 특정 트랙의 특정 KeyFrame Idx 번 삭제 
    /// </summary>
    /// <returns> : 만약 삭제 불가능하다면 return false </returns>
    bool RemoveKeyFrame(int _TrackIdx, int _KeyFrameIdx);
    
private:
    
    bool IsTrackHasObject(GameObject* _GameObject);
    
public: // 실제 Level play시 처리 가능한 함수들 
    
    bool Play(bool _bBackToStopOnEnd = false);
    void Stop();
    
public:
    
    void SaveToLevelFile(FILE* _File) override;
    void LoadFromLevelFile(FILE* _File) override;
};
