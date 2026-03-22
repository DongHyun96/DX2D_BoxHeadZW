#pragma once
// #include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/04.Asset/07.Flipbook/AFlipbook.h"
#include "GameEngine/06.Component/RenderComponent/CRenderComponent.h"

class CFlipbookRender : public CRenderComponent
{
    
    friend class FlipbookRenderUI;
    
private:

    map<wstring, vector<Ptr<AFlipbook>>> m_mapCategoryFlipbooks{}; // 카테고리 이름, Flipbook 벡터 형식의 자료구조로 Flipbook 저장

private:
    
    wstring                 m_CurSelectedCategory{};             // null 문자열일 경우, 선택된 카테고리가 존재하지 않음
    vector<Ptr<AFlipbook>>* m_vecCurSelectedCategoryFlipbooks{}; // 현재 선택된 카테고리의 Flipbook 집합 
    int                     m_CurSelectedFlipbookIdx{};          // 카테고리 내에서 선택된 Flipbook Idx
    
private: // 현재 선택된 AFlipbook 내에서의 Anim Data

    int             m_CurAnimatingSpriteIdx{}; // 현재 재생중인 Flipbook 내에서 재생중인 프레임 Sprite index

    int             m_RepeatCount{};    // -1 : 반복재생, 0 이상이면 재생 횟수
    bool            m_bCurCycleFinished{};   // 현재 Animation 재생 바퀴 완료 (끝 Sprite까지 도달 완료)
    
    float           m_FPS{};            // 1초당 보여줄 프레임수
    float           m_FrameTimer{};     // 1프레임 보여준 누적시간    
    
    bool            m_bPlayReverse{};   // 반대로 재생시킬건지
    bool            m_bStopped{};       // Animation이 멈춘 상태인지 (멈춘상태에서 Sprite 한 장을 보여주는 식으로 처리를 할 수도 있다)
    
public:
    
    CFlipbookRender();
    virtual ~CFlipbookRender() override;

    CLONE(CFlipbookRender)
    
public:
    
    void CreateMaterial() override;
    
public:
    void FinalTick() override;
    void Render() override;
    
private:
    
    bool CheckFinish();
    
public:
    
    /// <summary>
    /// 현재 카테고리 변경 
    /// </summary>
    /// <returns></returns>
    bool SetCurrentCategory(const wstring& _CategoryKey);

public: // 재생 관련

    // TODO : 반대로 재생시키는 Reverse 기능도 있어야 함
    
    /// <summary>
    /// 현재 지정된 카테고리에서의 재생 처리
    /// </summary>
    /// <param name="_FlipbookIdx"> : 현재 카테고리에서 재생시킬 Flipbook Idx </param>
    /// <param name="_FPS"> : 1초당 보여줄 프레임 수 </param>
    /// <param name="_RepeatCount"> : -1이면 반복재생 </param>
    /// <param name="_bPlayReverse"> : 반대로 재생시킬건지 </param>
    /// <returns> : 제대로 Play 시작할 수 없는 상황이라면 return false </returns>
    bool Play(int _FlipbookIdx, float _FPS = 10.f, int _RepeatCount = -1, bool _bPlayReverse = false);
    
    /// <summary>
    /// 카테고리 지정 처리 하면서 특정 Flipbook Animation Play 시작
    /// </summary>
    /// <param name="_Category"> : Flipbook 카테고리 </param>
    /// <param name="_FlipbookIdx"> : 카테고리 내에서의 Flipbook Idx 고르기</param>
    /// <param name="_FPS"> : 1초당 보여줄 프레임 수 </param>
    /// <param name="_RepeatCount"> : -1이면 반복재생 </param>
    /// <param name="_bPlayReverse"> : 반복재생 처리할건지 </param>
    /// <returns> : 제대로 Play 되지 않았다면 return false </returns>
    bool Play(const wstring& _Category, int _FlipbookIdx, float _FPS = 10.f, int _RepeatCount = -1, bool _bPlayReverse = false);

    /// <summary>
    /// 현재 재생중인 Animation 강제로 멈추기, 재생중인 Sprite Idx는 초기 지점으로 돌아감(Reverse에 따라 양 끝)
    /// </summary>
    /// <returns> : 멈출 수 없는 상태라면 return false </returns>
    bool Stop();
    
public:

    bool SetFlipbook(const wstring& _Category, int _Idx, const Ptr<AFlipbook>& _Flipbook);
    bool AddFlipbook(const wstring& _Category, const Ptr<AFlipbook>& _Flipbook);

public:
    
    UINT GetCategoryFlipbookCount(const wstring& _Category);
    
    Ptr<AFlipbook> GetFlipbook(const wstring& _Category, int _Idx);
    
    wstring GetCurSelectedCategory() const { return m_CurSelectedCategory; }

public:
    
    /// <summary>
    /// 카테고리 내에서 해당 Idx의 Flipbook 제거
    /// </summary>
    /// <remarks> : 제대로 제거되었다면 return true </remarks>
    bool RemoveFlipbook(const wstring& _Category, int _Idx);

    /// <summary>
    /// 해당 Flipbook 객체를 들고 있다면 제거
    /// </summary>
    /// <returns> : 제대로 제거되었다면 return true </returns>
    bool RemoveFlipbook(const wstring& _Category, const Ptr<AFlipbook>& _Flipbook);

    /// <summary>
    /// 카테고리 내에서의 Flipbook Swap 처리
    /// </summary>
    /// <param name="_Category"></param>
    /// <param name="_A"></param>
    /// <param name="_B"></param>
    /// <returns></returns>
    bool SwapFlipbook(const wstring& _Category, int _A, int _B);
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
    
};
