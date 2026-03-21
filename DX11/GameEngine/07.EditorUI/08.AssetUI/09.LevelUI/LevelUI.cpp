#include "pch.h"
#include "LevelUI.h"

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
    
    if (ImGui::Button("Change Level"))
        ChangeLevel(GetTargetAsset()->GetKey());
}
