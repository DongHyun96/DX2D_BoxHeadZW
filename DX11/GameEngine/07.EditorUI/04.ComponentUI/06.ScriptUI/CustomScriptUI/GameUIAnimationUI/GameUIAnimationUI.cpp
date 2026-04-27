#include "pch.h"
#include "GameUIAnimationUI.h"

GameUIAnimationUI::GameUIAnimationUI()
    : CustomScriptUI("GameUIAnimation(Script)")
{
    m_ComponentTitle = "GameUIAnimation";
}

GameUIAnimationUI::~GameUIAnimationUI()
{
}

void GameUIAnimationUI::Tick_UI()
{
    ComponentUI::Tick_UI();
}
