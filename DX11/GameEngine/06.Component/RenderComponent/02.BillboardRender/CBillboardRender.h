#pragma once
#include "GameEngine/06.Component/RenderComponent/CRenderComponent.h"

class CBillboardRender : public CRenderComponent
{
private:

    Vec2 m_BillboardScale{};

public:

    CBillboardRender();
    virtual ~CBillboardRender() override;

    CLONE(CBillboardRender)

public:
    void FinalTick() override;
    void Render() override;
    void CreateMaterial() override;
    static void BeginInstancing();
    static void FlushInstancing();

public:

    GET_SET(Vec2, BillboardScale)

public:

    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

};
