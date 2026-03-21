#pragma once
#include "GameEngine/04.Asset/Asset.h"
#include "GameEngine/05.GameObject/GameObject.h"
#include "Layer/Layer.h"

class ALevel : public Asset
{
private:
    
    Layer   m_arrLayer[MAX_LAYER]{};        // 하나의 레벨안에 총 32개의 레이어가 존재
    UINT    m_CollisionMatrix[MAX_LAYER]{}; // 어떤 레이어와, 어떤 레이어가 충돌검사를 진행할지 마킹한 데이터 테이블
    bool    m_Changed{};                    // 레벨 안에 오브젝트들의 상태가 변경됐는지 확인하는 변수 (오브젝트 추가, 삭제, 또는 오브젝트끼리의 계층관계가 변경 등)
    
public:
    
    ALevel();
    virtual ~ALevel() override;
    
public:
    
    ALevel* Clone() const { return new ALevel(*this); } // 복사생성자를 사용한 새로운 레벨 생성
    
    void AddObject(int _LayerIdx, const Ptr<GameObject>& _Object);
    void Deregister();

public:
    
    /// <summary>
    /// Level 처음 시작 시 호출 
    /// </summary>
    void Begin();
    
    void Tick();
    void FinalTick();

public:
    
    void CheckCollisionLayer(UINT _LayerIdx1, UINT _LayerIdx2);
    void CheckCollisionLayer(const wstring& _LayerName1, const wstring& _LayerName2);
    
    UINT* GetCollisionMatrix() { return m_CollisionMatrix; }
    
    virtual HRESULT Save(const wstring& _FilePath) override;
    virtual HRESULT Load(const wstring& _FilePath) override;
    
public:
    
    Layer* GetLayer(UINT _Idx)
    {
        assert(0 <= _Idx && _Idx < MAX_LAYER);
        return &(m_arrLayer[_Idx]);
    }

    Ptr<GameObject> FindObjectByName(const wstring& _Name);

    /// <summary>
    /// <para> Level의 변경점이 있는지 확인 </para>
    /// <para> 변경점을 확인하면서, 동시에 변경점이 있었다고 체크된 상태였다면, 해당상태 다시 되돌려둠 </para>
    /// </summary>
    bool HasChanged()
    {
        bool Changed = m_Changed;
        m_Changed = false;
        return Changed;
    }

    /// <summary>
    /// 레벨에 변경사항이 있다면(ex 오브젝트 추가, 삭제, Hierarchy의 변화 등) 변화 여부 true로 기록 처리
    /// </summary>
    void SetChanged() { m_Changed = true; }
};
