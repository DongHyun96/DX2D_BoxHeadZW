#include "pch.h"
#include "TextureSpriteCreatorUI.h"

#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"

TextureSpriteCreatorUI::TextureSpriteCreatorUI()
    : EditorUI("TextureSpriteCreator")
{
}

TextureSpriteCreatorUI::~TextureSpriteCreatorUI()
{
}

void TextureSpriteCreatorUI::Tick_UI()
{
    ColoredButtonTitle("Current Selected Atlas");

    if (!m_TargetTexture) return;

    string Key = string(m_TargetTexture->GetKey().begin(), m_TargetTexture->GetKey().end());

    ImGui::Text("Name");
    ImGui::SameLine(120);
    ImGui::InputText("##TextureName", Key.data(), Key.length() + 1, ImGuiInputTextFlags_ReadOnly);

    const float MaxAtlasSize = 260.f;
    const float MaxDim = max(m_TargetTexture->GetWidth(), m_TargetTexture->GetHeight());
    const float AtlasScale = (MaxDim > 0.f && MaxDim > MaxAtlasSize) ? (MaxAtlasSize / MaxDim) : 1.f;
    
    // 이미지 샘플
    ImGui::ImageWithBg
    (
        m_TargetTexture->GetSRV().Get(),
        ImVec2(m_TargetTexture->GetWidth() * AtlasScale, m_TargetTexture->GetHeight() * AtlasScale),
        Vec2(0.f, 0.f), Vec2(1.f, 1.f),
        ImVec4(0.0f, 0.0f, 0.0f, 1.0f)
    );
    
    int Width = m_TargetTexture->GetWidth(); 
    int Height = m_TargetTexture->GetHeight(); 

    ImGui::Text("Resolution");
    ImGui::SameLine(150);
    ImGui::SetNextItemWidth(100);
    ImGui::InputInt("##Width", &Width, 0, 0, ImGuiInputTextFlags_ReadOnly);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    ImGui::InputInt("##Height", &Height, 0, 0, ImGuiInputTextFlags_ReadOnly);
    
    
    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Text("Total Frame XY Count ");
    ImGui::SameLine();
    
    ImGui::InputFloat2("##Total Frame XY Count", m_TotalFrameXYCount, "%.f");
    
    ImGui::Text("SpriteAsset Name ");
    ImGui::SameLine();

    m_InputSpriteName.resize(40);
    ImGui::InputText("##SpriteAssetName", m_InputSpriteName.data(), m_InputSpriteName.length());
    m_InputSpriteName.resize(strlen(m_InputSpriteName.c_str()));
    
    // Create Sprite Button
    if (ImGui::Button("Create Sprites"))
    {
        if (m_TotalFrameXYCount == Vec2::Zero)  return; // Invalid Frame Count
        if (m_InputSpriteName.length() == 0)    return; // Invalid Name received

        vector<Ptr<ASprite>> vecSprites{};
        
        bool CreateSucceeded = AssetMgr::GetInst()->CreateNewAssetsBySuffixXYCount<ASprite>(wstring(m_InputSpriteName.begin(), m_InputSpriteName.end()), vecSprites, m_TotalFrameXYCount);
        if (!CreateSucceeded) return;
        else
        {
            // 새로이 생성된 Asset들 AssetMgr에 등록
            for (const Ptr<ASprite>& sprite : vecSprites)
                AssetMgr::GetInst()->AddAsset(sprite->GetKey(), sprite.Get());
        }

        // 제대로 생성되었다면 새롭게 생성된 Sprite들의 Atlas와 FrameCoord에 따른 UV 좌표 잡기
        for (UINT y = 0; y < m_TotalFrameXYCount.y; ++y)
        {
            for (UINT x = 0; x < m_TotalFrameXYCount.x; ++x)
            {
                const UINT index = y * m_TotalFrameXYCount.x + x;
                vecSprites[index]->SetAtlas(m_TargetTexture);
                vecSprites[index]->SetByFrameCoord(Vec2(x, y), m_TotalFrameXYCount);
            }
        }
        
        SetActive(false);
    }
}

void TextureSpriteCreatorUI::Activate()
{
    m_TotalFrameXYCount = Vec2::Zero;
    m_InputSpriteName = "";
}
