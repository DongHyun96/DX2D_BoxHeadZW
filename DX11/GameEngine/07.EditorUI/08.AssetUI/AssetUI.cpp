#include "pch.h"
#include "AssetUI.h"

#include "GameEngine/03.Manager/09.EditorMgr/EditorMgr.h"
#include "GameEngine/07.EditorUI/01.Inspector/Inspector.h"

AssetUI::AssetUI(ASSET_TYPE _Type)
    : EditorUI(ToString(_Type))
    , m_AssetType(_Type)
{
    SetActive(false);
    SetSeparator(false);
}

AssetUI::~AssetUI()
{
}

void AssetUI::OutputTitle() const
{
    ColoredButtonTitle(AssetTypeToString(m_AssetType), ImVec4(0.6f, 0.25f, 0.25f, 1.f));
    ImGui::SameLine();
    string Key = "Key : " + string(GetTargetAsset()->GetKey().begin(), GetTargetAsset()->GetKey().end()); 
    ColoredButtonTitle(Key, ImVec4(0.6f, 0.25f, 0.25f, 1.f));
    
    if (m_TargetAsset->GetIsProvidedByEngine())
    {
        ImGui::SameLine();
        ColoredButtonTitle("EngineProvided", ImVec4(0, 0.643, 0.741, 1));
    }
    ImGui::SeparatorText("");
}

void AssetUI::Tick_UI()
{
    OutputTitle();
}

void AssetUI::SaveButton()
{
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    
    if (ImGui::Button("Save Asset")) GetTargetAsset()->SaveBySelfRelativePath();
}

