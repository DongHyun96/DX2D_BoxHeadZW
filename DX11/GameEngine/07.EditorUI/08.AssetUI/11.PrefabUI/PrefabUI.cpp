#include "pch.h"
#include "PrefabUI.h"

#include "GameEngine/04.Asset/09.Prefab/APrefab.h"

PrefabUI::PrefabUI()
    : AssetUI(ASSET_TYPE::PREFAB)
{
    m_ObjectViewer.Init(this);
}

PrefabUI::~PrefabUI()
{
}

void PrefabUI::Tick_UI()
{
    AssetUI::Tick_UI();
    

    Ptr<APrefab> pPrefab = static_cast<APrefab*>(GetTargetAsset().Get());
    if (!pPrefab)
    {
        m_ObjectViewer.SetTargetObject(nullptr);
        SaveButton();
        return;
    }

    m_ObjectViewer.SetTargetObject(pPrefab->m_ProtoObj);

    if (!m_ObjectViewer.GetTargetObject())
    {
        ImGui::Text("Proto Object : None");
        SaveButton();
        return;
    }

    m_ObjectViewer.TickHeaderUI(); // TODO : 최상위 Parent 정보만 띄울 수 있는 한계점이 있다. / Tree 구조로 m_ObjectViewer를 Child용으로도 두어, 보여주기 처리를 할 수 있을듯?
    
    SaveButton();
}
