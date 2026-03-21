#pragma once
#include "GameEngine/04.Asset/06.Sprite/ASprite.h"
#include "GameEngine/06.Component/RenderComponent/CRenderComponent.h"

class CSpriteRender : public CRenderComponent
{
private:
    
    Ptr<ASprite> m_Sprite{};
    
public:
    
    CSpriteRender();
    virtual ~CSpriteRender() override;

    CLONE(CSpriteRender)
    
public:
    
    void CreateMaterial() override;
    
public:
    void FinalTick() override;
    void Render() override;
    
public:

    GET_SET(Ptr<ASprite>, Sprite)
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
    
};
