#pragma once
#include "GameEngine/07.EditorUI/EditorUI.h"

class Menu : public EditorUI
{
private:
    
public:

    Menu();
    virtual ~Menu() override;
    
public:
    
    virtual void Tick() override;
    virtual void Tick_UI() override;
    
private:
    
    void FileTick();
    void LevelTick();
    void ViewTick();
    void GameObjectTick();
    void AssetTick();
    void CollisionMatrixTick();

public:
    
    void TrySaveAllAssets();
    void TrySaveCurrentLevel();
    
};
