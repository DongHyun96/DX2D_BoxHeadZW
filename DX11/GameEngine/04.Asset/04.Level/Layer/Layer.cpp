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

    // 아직 아무 Layer에도 할당되지 않았던 오브젝트의 경우
    if (_Object->m_LayerIdx == -1) _Object->m_LayerIdx = m_LayerIdx;

    // 트리 레벨 순회 방문처리 (Child까지 Layer의 번호를 설정하는 과정)
    // 여기서는 bInLayer만 true로 잡히도록 처리 (LayerIdx는 Child에서 고유한 LayerIdx를 들고 있을 수 있도록 수정 x)
    list<GameObject*> queue{};
    queue.push_back(_Object.Get());

    while (!queue.empty())
    {
        GameObject* pObject = queue.front();
        queue.pop_front();
        
        // pObject->m_LayerIdx = m_LayerIdx;
        pObject->m_bInLayer   = true;
        pObject->m_OwnerLevel = this->m_OwnerLevel;
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

void Layer::MoveParentObjectOrder(const Ptr<GameObject>& _Object, int _Dir)
{
    int idx = -1;
    for (int i = 0; i < (int)m_vecParents.size(); ++i)
    {
        if (m_vecParents[i] == _Object)
        {
            idx = i;
            break;
        }
    }

    if (idx == -1) return;

    int targetIdx = idx + _Dir;
    if (targetIdx < 0 || targetIdx >= (int)m_vecParents.size()) return;

    // Swap
    Ptr<GameObject> temp = m_vecParents[idx];
    m_vecParents[idx] = m_vecParents[targetIdx];
    m_vecParents[targetIdx] = temp;

    if (m_OwnerLevel)
        m_OwnerLevel->SetChanged();
}

void Layer::Tick()
{
    for (const Ptr<GameObject>& gameObject : m_vecParents)
    {
        if (gameObject->GetActive())
            gameObject->Tick();
    }
}

void Layer::FinalTick()
{
    vector<Ptr<GameObject>>::iterator iter = m_vecParents.begin();

    while (iter != m_vecParents.end())
    {
        Ptr<GameObject>& gameObject = *iter;
        
        // 이번 Tick에서 Deactivate 처리된 GameObject 또한 AllObject에 등록 처리를 해야 CollisionMgr에서 제대로된 충돌검사 Callback 처리를 할 수 있음
        if (gameObject->GetActive() || gameObject->GetObjectMarkedDeactivated())
        {
            gameObject->FinalTick();
            gameObject->m_ObjectMarkedDeactivated = false; // FinalTick 호출 이후, 해당 마킹 지워버리기
        }
        
        // 최상위 부모일 경우, 여기서 삭제 처리
        if (gameObject->IsObjectDestroyed()) iter = m_vecParents.erase(iter);
        else ++iter;
    }
}

void Layer::Begin()
{
    for (const Ptr<GameObject>& gameObject : m_vecParents)
        gameObject->Begin();
}

void Layer::AfterLevelBegin()
{
    for (const Ptr<GameObject>& gameObject : m_vecParents)
        gameObject->AfterLevelBegin();
}

void Layer::Render()
{
    for (const Ptr<GameObject>& gameObject : m_vecParents)
        gameObject->Render();
}
