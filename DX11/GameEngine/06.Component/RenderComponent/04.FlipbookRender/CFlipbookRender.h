#pragma once
// #include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/04.Asset/07.Flipbook/AFlipbook.h"
#include "GameEngine/06.Component/RenderComponent/CRenderComponent.h"

class CFlipbookRender : public CRenderComponent
{
private:

    vector<Ptr<AFlipbook>> m_vecFlipbook{};
    // Ptr<AFlipbook>  m_Flipbook{};

    int             m_CurFlipbook{}; // TODO : Idx로 명시적으로 이름 짓기 
    int             m_CurSprite{}; // 현재 재생중인 Flipbook 내에서 재생중인 프레임 Sprite index // TODO : idx로 명시적으로 이름 짓기

    int             m_RepeatCount{}; // -1 : 반복재생, 0 이상이면 재생 횟수
    bool            m_Repeat{};
    bool            m_Finish{};
    
    float           m_FPS{}; // 1초당 보여줄 프레임수
    float           m_AccTime{}; // 1프레임 보여준 누적시간    
    
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

    // SET(Ptr<AFlipbook>, Flipbook)
    
    void Play(int _FlipbookIdx, float _FPS, int _RepeatCount);
    
    // TODO : 추후, Sprite 한 장면만 보여주기식 함수도 추가하면 편할듯?

    void SetFlipbook(int _Idx, const Ptr<AFlipbook>& _Flipbook);
    
    void AddFlipbook(const Ptr<AFlipbook>& _Flipbook)
    {
        if (_Flipbook) m_vecFlipbook.push_back(_Flipbook);
    }

public:
    
    UINT GetFlipbookCount() const { return m_vecFlipbook.size(); }
    
    Ptr<AFlipbook> GetFlipbook(int _Idx) const
    {
        return _Idx < 0 || _Idx >= m_vecFlipbook.size() ? nullptr : m_vecFlipbook[_Idx];
    }

public:
    
    /// <summary>
    /// 해당 Idx의 Flipbook 제거
    /// </summary>
    /// <remarks> : 제대로 제거되었다면 return true </remarks>
    bool RemoveFlipbook(int _Idx);

    /// <summary>
    /// 해당 Flipbook 객체를 들고 있다면 제거
    /// </summary>
    /// <param name="_Flipbook"></param>
    /// <returns> : 제대로 제거되었다면 return true </returns>
    bool RemoveFlipbook(const Ptr<AFlipbook>& _Flipbook);
    
    bool SwapFlipbook(int _A, int _B);
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
    
};
