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

    if (!m_Level) m_Level = static_cast<ALevel*>(GetTargetAsset().Get()); 
    if (!m_Level) return; 
    
    if (ImGui::Button("Change Level"))
        ChangeLevel(m_Level->GetKey(), true);


    wstring FirstCamName{};
    if (!m_Level->m_FirstMainCamera)    FirstCamName = L"None";
    else                                FirstCamName = m_Level->m_FirstMainCamera->GetOwner()->GetName();

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
                if (m_Level->IsObjectInLevel(gObject) && gObject->Camera())
                    m_Level->SetFirstMainCamera(gObject->Camera().Get());
            }
        }
        
        ImGui::EndDragDropTarget();
    }
    
    wstring UICamName{};
    if (!m_Level->m_UICamera)  UICamName = L"None";
    else                       UICamName = m_Level->m_UICamera->GetOwner()->GetName();

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
                if (m_Level->IsObjectInLevel(gObject) && gObject->Camera())
                    m_Level->SetUICamera(gObject->Camera().Get());
            }
        }
        
        ImGui::EndDragDropTarget();
    }
}
