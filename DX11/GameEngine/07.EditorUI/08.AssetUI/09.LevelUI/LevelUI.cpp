#include "pch.h"
#include "LevelUI.h"

#include "GameEngine/04.Asset/04.Level/ALevel.h"
#include "GameEngine/07.EditorUI/07.TreeUI/TreeUI.h"

LevelUI::LevelUI()
    : AssetUI(ASSET_TYPE::LEVEL)
{
}

LevelUI::~LevelUI()
{
}

void LevelUI::Tick_UI()
{
    AssetUI::Tick_UI();

    ALevel* pLevel = static_cast<ALevel*>(GetTargetAsset().Get());
     
    
    if (ImGui::Button("Change Level"))
        ChangeLevel(pLevel->GetKey(), true);


    wstring FirstCamName{};
    if (!pLevel->m_FirstMainCamera)    FirstCamName = L"None";
    else                                FirstCamName = pLevel->m_FirstMainCamera->GetOwner()->GetName();

    ImGui::Text("First Start Main Camera set : ");
    
    ImGui::InputText("##MainCameraName", string(FirstCamName.begin(), FirstCamName.end()).data(), FirstCamName.length() + 1, ImGuiInputTextFlags_ReadOnly);
    
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("Outliner"))
        {
            if (!TreeUI::IsPayloadMultiData(Payload))
            {
                DWORD_PTR data = *static_cast<DWORD_PTR*>(Payload->Data);
                Ptr<GameObject> gObject = reinterpret_cast<GameObject*>(data);
                
                // 현재 선택된 레벨에 존재하는 카메라 오브젝트인지 확인
                if (pLevel->IsObjectInLevel(gObject) && gObject->Camera())
                    pLevel->SetFirstMainCamera(gObject->Camera().Get());
            }
        }
        
        ImGui::EndDragDropTarget();
    }
    
    wstring UICamName{};
    if (!pLevel->m_UICamera)  UICamName = L"None";
    else                       UICamName = pLevel->m_UICamera->GetOwner()->GetName();

    ImGui::Text("UI Camera set : ");
    ImGui::SameLine(100);
    ImGui::InputText("##UICamName", string(UICamName.begin(), UICamName.end()).data(), UICamName.length() + 1, ImGuiInputTextFlags_ReadOnly);
    
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("Outliner"))
        {
            if (!TreeUI::IsPayloadMultiData(Payload))
            {
                DWORD_PTR data = *static_cast<DWORD_PTR*>(Payload->Data);
                Ptr<GameObject> gObject = reinterpret_cast<GameObject*>(data);
                
                // 현재 선택된 레벨에 존재하는 카메라 오브젝트인지 확인
                if (pLevel->IsObjectInLevel(gObject) && gObject->Camera())
                    pLevel->SetUICamera(gObject->Camera().Get());
            }
        }
        
        ImGui::EndDragDropTarget();
    }
}
