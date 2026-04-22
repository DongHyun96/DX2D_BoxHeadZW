#pragma once


enum class TintColorControl
{
    NONE,                   // TintColor 옵션 none
    UI_COLOR,               // UIColor의 TintColor 변경
    RENDERCOM_TINTCOLOR,    // RenderComponent의 TintColor 변경
    BOTH                    // 둘 다 변경
};

struct UIAnimKeyFrameData
{
    float               Time{};                       // 키프레임 시간대
    Ptr<CTransform>     Transform{};
    Vec4                TintColor  = DEF_COLOR_WHITE; // Text GameObject의 경우, Text의 TintColor값으로 적용됨(주의) -> CText나 RenderComponent를 따로 가지고 있지 않을 경우 처리 x   
    bool                bUseLerp{};                   // 이전 키 프레임에서 이 키 프레임으로 Animation 전환 처리 시, Lerp를 사용할지 여부
};

/// <summary>
/// 하나의 GameObject를 담당하는 애니메이션 트랙
/// </summary>
struct UIAnimTrack
{
    vector<UIAnimKeyFrameData>  KeyFrames{}; // 시간 순서에 따라 정렬된 키프레임들
    int CurrentKeyIndex{}; // 현재 재생중인 Keyframe Index
};

class CUIAnimation : public CScript
{
private:

    map<GameObject*, UIAnimTrack> m_mapTracks{};

private:
    
    bool m_bIsPlaying{};
    bool m_bBackToStopOnAnimEnd{}; // Animation 재생이 끝났을 때, 바로 Stop 처리할지(원본 데이터로 돌아갈지) 여부
    
    float m_AnimTimer{}; // Animation 재생 Timer
    UINT m_ObjectAnimFinishedCount{}; // Animation 재생이 끝난 GameObject Count (재생 중, 끝난 시점을 알아야 해서 Count check함)
    
public:
    
    CUIAnimation();
    virtual ~CUIAnimation() override;
    CLONE(CUIAnimation);
    
public:
    
    void Tick() override;
    
public: // Editing 관련 함수들
    
    /// <summary>
    /// Animate 시킬 GameObject 추가 (UI Script를 들고 있지 않아도 사용 가능)
    /// Animate 시킬 GameObject 본인이어야 한다(자식 객체는 control 불가)
    /// </summary>
    /// <returns> : 이미 해당 GameObject가 추가되어있다면 return false / Level 이 Play 상태라면, return false(Editing 환경에서만 새로이 추가 가능) </returns>
    bool AddGameObjectToAnimate(GameObject* _GameObject);

    /// <summary>
    /// 해당 게임오브젝트 Animation에서 제거
    /// </summary>
    /// <returns> : Editing 상태가 아니거나, GameObject를 가지고 있지 않으면 return false </returns>
    bool RemoveGameObject(GameObject* _GameObject);

public:
    /// <summary>
    /// 해당 GameObject의 키프레임 해당 프레임타임에 추가
    /// </summary>
    /// <param name="_GameObject"></param>
    /// <param name="_KeyFrameTime"></param>
    /// <returns> : 해당 키프레임을 넣을 수 없는 상황이라면 return false </returns>
    bool AddNewKeyFrame(GameObject* _GameObject, float _KeyFrameTime);

    /// <summary>
    /// 특정 GameObject 트랙의 특정 KeyFrame Idx 번 삭제 
    /// </summary>
    /// <returns> : 만약 삭제 불가능하다면 return false </returns>
    bool RemoveKeyFrame(GameObject* _GameObject, int _KeyFrameIdx);
    
public: // 실제 Level play시 처리 가능한 함수들 
    
    void Play(bool _bBackToStopOnEnd = false);
    void Stop();
    
public:
    
    void SaveToLevelFile(FILE* _File) override;
    void LoadFromLevelFile(FILE* _File) override;
};
