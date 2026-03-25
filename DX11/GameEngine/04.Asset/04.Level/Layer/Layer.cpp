#include "pch.h"
#include "Layer.h"

#include "GameEngine/04.Asset/04.Level/ALevel.h"

Layer::Layer()
{
}

Layer::Layer(const Layer& _Origin)
    : Entity(_Origin)
    , m_LayerIdx(_Origin.m_LayerIdx)
{
    // AllObjects는 Tick마다 매번 갱신처리됨
    // ParentObjects만 복사해주면 됨

    for (const Ptr<GameObject>& Object : _Origin.m_vecParents)
        AddObject(Object->Clone());
}

Layer::~Layer()
{
}

void Layer::SetName(const wstring& name)
{
    if (m_OwnerLevel && m_OwnerLevel->IsLayerNameDuplicated(name, m_LayerIdx)) return;
    Entity::SetName(name);
}

void Layer::AddObject(const Ptr<GameObject>& _Object)
{
    m_vecParents.push_back(_Object);
    
    _Object->m_LayerIdx = m_LayerIdx;
    _Object->m_bInLayer = true;
    
    // 트리 레벨 순회 방문처리
    list<GameObject*> queue{};
    queue.push_back(_Object.Get());

    while (!queue.empty())
    {
        GameObject* pObject = queue.front();
        queue.pop_front();
        
        pObject->m_LayerIdx = m_LayerIdx;
        pObject->m_bInLayer = true;
        // m_vecAllObjects.push_back(pObject);

        for (const Ptr<GameObject>& pChild : pObject->m_vecChild)
            queue.push_back(pChild.Get());
    }
}

void Layer::DeregisterAsParent(const Ptr<GameObject>& _Object)
{
    vector<Ptr<GameObject>>::iterator iter = m_vecParents.begin();
    for (; iter != m_vecParents.end(); ++iter)
    {
        if (*iter == _Object)
        {
            (*iter)->m_bInLayer = false;
            m_vecParents.erase(iter);
            return;
        }
    }
    assert(nullptr);
}

void Layer::Tick()
{
    for (const Ptr<GameObject>& gameObject : m_vecParents)
        gameObject->Tick();
}

void Layer::FinalTick()
{
    vector<Ptr<GameObject>>::iterator iter = m_vecParents.begin();

    while (iter != m_vecParents.end())
    {
        Ptr<GameObject>& gameObject = *iter;
        gameObject->FinalTick();
        
        if (gameObject->IsDead()) iter = m_vecParents.erase(iter);
        else ++iter;
    }
}

void Layer::Begin()
{
    for (const Ptr<GameObject>& gameObject : m_vecParents)
        gameObject->Begin();
}

void Layer::Render()
{
    for (const Ptr<GameObject>& gameObject : m_vecParents)
        gameObject->Render();
}
