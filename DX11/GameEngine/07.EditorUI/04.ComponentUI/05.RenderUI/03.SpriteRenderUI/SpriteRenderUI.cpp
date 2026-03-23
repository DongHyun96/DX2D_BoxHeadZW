#include "pch.h"
#include "SpriteRenderUI.h"

#include "GameEngine/07.EditorUI/07.TreeUI/TreeUI.h"
#include "GameEngine/07.EditorUI/08.AssetUI/04.SpriteUI/SpriteUI.h"

SpriteRenderUI::SpriteRenderUI()
    : RenderUI(COMPONENT_TYPE::SPRITE_RENDER, "SpriteRenderUI")
{
    m_ComponentTitle = "Sprite Renderer";
}

SpriteRenderUI::~SpriteRenderUI()
{
}

void SpriteRenderUI::Tick_UI()
{
    RenderUI::Tick_UI();
    
    Ptr<CSpriteRender> SpriteRender = GetTargetObject()->SpriteRender();
    Ptr<ASprite> Sprite             = SpriteRender->GetSprite();

    const wstring SpriteKey = !Sprite ? L"NONE" : Sprite->GetKey(); 
    string SpriteKeyStr = string(SpriteKey.begin(), SpriteKey.end());
    
    
    ImGui::Text("Selected Sprite");
    ImGui::SameLine(120);
    ImGui::InputText("##SpriteKey", SpriteKeyStr.data(), SpriteKeyStr.length() + 1, ImGuiInputTextFlags_ReadOnly);

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    
    // Sprite 이미지 샘플 보여주기
    SpriteUI::DrawSpritePreview(Sprite, m_PreviewZoom, m_MinPreviewZoom, m_MaxPreviewZoom);
    
    // Drag and Drop으로 Sprite 지정
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* PayLoad = ImGui::AcceptDragDropPayload("Content"))
        {
            if (!TreeUI::IsPayloadMultiData(PayLoad))
            {
                DWORD_PTR data = *static_cast<DWORD_PTR*>(PayLoad->Data);
                Ptr<Asset> pAsset = reinterpret_cast<Asset*>(data);

                if (ASSET_TYPE::SPRITE == pAsset->GetType())
                    SpriteRender->SetSprite(static_cast<ASprite*>(pAsset.Get()));
            }
        }

        ImGui::EndDragDropTarget();
    }
}
