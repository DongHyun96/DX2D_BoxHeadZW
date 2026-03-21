#pragma once
#include "GameEngine/04.Asset/Entity.h"
#include "GameEngine/05.GameObject/GameObject.h"

class Layer : public Entity
{
    
    friend class ALevel;
    
private:

    vector<Ptr<GameObject>> m_vecParents{};     // Layer에 소속된 최상위 부모타입 오브젝트 -> 실질적으로 이 벡터로 GameObject들을 관리한다 
    
    vector<Ptr<GameObject>> m_vecAllObjects{};  // Layer에 소속된 모든(부모, 자식타입) 오브젝트 -> Tick에서 매번 갱신된다

    int                     m_LayerIdx{}; // Layer 본인의 인덱스(0 ~ 31)
    
public:
    
    Layer();
    Layer(const Layer& _Origin);
    
    virtual ~Layer() override;
    
public:
    
    void AddObject(const Ptr<GameObject>& _Object);
    void DeregisterAsParent(const Ptr<GameObject>& _Object);

public:
    
    void RegisterObjectToAllObjects(const Ptr<GameObject>& _Object) { m_vecAllObjects.push_back(_Object); }
    void DeregisterObjectsFromAllObjects() { m_vecAllObjects.clear(); }
    
    
    const vector<Ptr<GameObject>>& GetParentObjects() const { return m_vecParents; }
    const vector<Ptr<GameObject>>& GetAllObjects()    const { return m_vecAllObjects; }
    
public:
    
    void Tick();
    void FinalTick();
    void Begin();
    void Render();
};
