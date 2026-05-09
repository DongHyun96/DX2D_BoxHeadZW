#pragma once


enum class UIAnimEndHandling
{
    DEFAULT,        // 마지막 상태 유지
    BACK_TO_STOP,   // Stop 상태로 돌아가기
    LOOP            // Animation Loop 처리
};


class CUIAnimation : public CScript
{
private:

    // map<class GameObjectRefHolder, UIAnimTrack> m_mapTracks{};
    vector<struct UIAnimTrack> m_vecTracks{};

private:
    
    bool m_bIsPlaying{};
    UIAnimEndHandling m_EndHandling{}; // Animation 재생이 끝났을 때, 어떻게 처리할 것인지에 대한 정보
    
    float m_AnimTimer{}; // Animation 재생 Timer (실제 Play 처리에 쓰일 Timer)
    
private:

    // Editor에서 Animation Stop 상황에서도 해당 Time에 Animation 상태가 어떨지 확인하는 용도의 Timer
    float m_EditingAnimTimer{}; 
    
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

    bool IsPlaying() const;
    
    float GetAnimTimer() const { return m_AnimTimer; }
    
public:
    
    float GetEditingAnimTimer() const { return m_EditingAnimTimer; }

    /// <summary>
    /// 에디팅 타이머를 설정하고 해당 시점의 상태로 오브젝트들을 업데이트
    /// </summary>
    void SetEditingAnimTime(float _Time);
    
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
    bool RemoveTrackByTrackIdx(int _TrackIdx);

    /// <summary>
    /// Target GameObject를 들고 있는 Track 삭제하기 
    /// </summary>
    bool RemoveTrackByGameObject(GameObject* _GameObject);

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

    /// <summary>
    /// 특정 트랙의 키프레임을 시간순으로 정렬하고, 특정 인덱스가 정렬 후 어디로 이동했는지 반환
    /// </summary>
    /// <param name="_TrackIdx"></param>
    /// <param name="_CurrentIdx"></param>
    /// <returns></returns>
    int SortKeyFrames(int _TrackIdx, int _CurrentIdx);
    
private:
    
    /// <summary>
    /// 이미 해당 오브젝트 Track이 존재하는 경우 
    /// </summary>
    bool IsTrackHasObject(GameObject* _GameObject);
    
public: // 실제 Level play시 처리 가능한 함수들 
    
    bool Play(UIAnimEndHandling _EndHandlingType = UIAnimEndHandling::DEFAULT);
    void Stop();
    
public:
    
    void SaveToLevelFile(FILE* _File) override;
    void LoadFromLevelFile(FILE* _File) override;

private:
    
    /// <summary>
    /// 이 Script 제거 시에 호출받을 함수 (UIAnimation Script가 제거되기 이전, Stop처리로 레퍼런스로 들고 있었던 GameObject 원본값으로 돌리기) 
    /// </summary>
    virtual void OnRemoveScript() override;

    /// <summary>
    /// Owner GO 자체가 삭제처리되었을 때 (역시사 Stop 처리를 해주어, GO Reference가 원본값으로 돌아가도록 처리)
    /// </summary>
    virtual void OnOwnerDestroy() override;
    
};
