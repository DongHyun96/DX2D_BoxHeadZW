#include "pch.h"
#include "TextureUI.h"

#include "TextureSpriteCreatorUI.h"
#include "GameEngine/03.Manager/09.EditorMgr/EditorMgr.h"

TextureUI::TextureUI()
    : AssetUI(ASSET_TYPE::TEXTURE)
{
}

TextureUI::~TextureUI()
{
}

void TextureUI::Tick_UI()
{
    AssetUI::Tick_UI();
    
    Ptr<ATexture> pTexture = static_cast<ATexture*>(GetTargetAsset().Get());

    int Width = pTexture->GetWidth(); 
    int Height = pTexture->GetHeight();
    const int PreviewWidth = min(Width, 1000);
    const int PreviewHeight = min(Height, 1000);
    
    // 이미지 샘플
    ImGui::ImageWithBg
    (
        pTexture->GetSRV().Get(),
        ImVec2(PreviewWidth, PreviewHeight),
        Vec2(0.f, 0.f), Vec2(1.f, 1.f),
        ImVec4(0.0f, 0.0f, 0.0f, 1.0f)
    );
    

    ImGui::Text("Resolution");
    ImGui::SameLine(150);
    ImGui::SetNextItemWidth(100);
    ImGui::InputInt("##Width", &Width, 0, 0, ImGuiInputTextFlags_ReadOnly);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    ImGui::InputInt("##Height", &Height, 0, 0, ImGuiInputTextFlags_ReadOnly);
    
    // Create Atlas Sprites at once
    ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
    
    if (ImGui::Button("Create Sprite Assets in Atlas"))
    {
        // EditorMgr::GetInst()->Get
        Ptr<TextureSpriteCreatorUI> pUI = dynamic_cast<TextureSpriteCreatorUI*>(EditorMgr::GetInst()->FindUI("TextureSpriteCreator").Get());
        pUI->SetTargetTexture(pTexture);
        pUI->SetActive(true);
    }
}
