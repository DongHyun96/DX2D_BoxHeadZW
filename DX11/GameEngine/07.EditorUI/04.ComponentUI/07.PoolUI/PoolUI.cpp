#include "pch.h"
#include "PoolUI.h"

#include "GameEngine/04.Asset/09.Prefab/APrefab.h"
#include "GameEngine/07.EditorUI/07.TreeUI/TreeUI.h"

PoolUI::PoolUI()
    : ComponentUI(COMPONENT_TYPE::POOL, "PoolUI")
{
    m_ComponentTitle = "ObjectPooler";
}

PoolUI::~PoolUI()
{
}

void PoolUI::Tick_UI()
{
    ComponentUI::Tick_UI();

    Ptr<CPoolComponent> PoolComponent = GetTargetObject()->PoolComponent();
    
    ImGui::Text("Prefab to use Pooling : ");
    
    wstring PoolObjectPrefabKey = PoolComponent->m_PrefabToPool ? PoolComponent->m_PrefabToPool->GetKey() : L"None";
	
    ImGui::InputText("##PrefabName", string(PoolObjectPrefabKey.begin(), PoolObjectPrefabKey.end()).data(), PoolObjectPrefabKey.length() + 1, ImGuiInputTextFlags_ReadOnly);

    // 특정 위젯에서 드래그가 발생했고, 해당 위젯 위에 마우스가 호버링 중인지
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* PayLoad = ImGui::AcceptDragDropPayload("Content"))
        {
            if (!TreeUI::IsPayloadMultiData(PayLoad))
            {
                DWORD_PTR data = *static_cast<DWORD_PTR*>(PayLoad->Data);
                Ptr<Asset> pAsset = reinterpret_cast<Asset*>(data);

                if (ASSET_TYPE::PREFAB == pAsset->GetType())
                {
                    APrefab* Prefab = static_cast<APrefab*>(pAsset.Get()); 
                    PoolComponent->m_PrefabToPool = Prefab;
                }
            }
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::Spacing();
    ImGui::Spacing();
    
    ImGui::Text("Pool Count : ");
    ImGui::SameLine(200);
    int PoolCount = PoolComponent->m_PoolCount;
    if (ImGui::InputInt("##Pool Count", &PoolCount))
        if (PoolCount >= 0) PoolComponent->m_PoolCount = PoolCount;
}
