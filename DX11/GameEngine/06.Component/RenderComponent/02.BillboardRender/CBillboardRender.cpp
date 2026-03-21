#include "pch.h"
#include "CBillboardRender.h"

#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"

CBillboardRender::CBillboardRender()
    : CRenderComponent(COMPONENT_TYPE::BILLBOARD_RENDER)
{
}

CBillboardRender::~CBillboardRender()
{
}

void CBillboardRender::FinalTick()
{
}

void CBillboardRender::Render()
{
    if (!GetMesh() || !GetMaterial()) return;

    
    GetMaterial()->SetScalar(VEC2_0, m_BillboardScale);
    
    GetMaterial()->Binding();
    GetMesh()->Render();
    
    GetMaterial()->Clear();
}

void CBillboardRender::CreateMaterial()
{
    // RectMesh 설정
    SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"RectMesh"));
    
    // 빌보드 전용 재질 생성
    Ptr<AMaterial> pMtrl = AssetMgr::GetInst()->Find<AMaterial>(L"BillboardMtrl");
    
    // 찾는 재질이 없으면 생성한다
    if (!pMtrl)
    {
        PrintIrrelevantDebugLog();
        pMtrl = new AMaterial;
        pMtrl->SetName(L"BillboardMtrl");
        AssetMgr::GetInst()->AddAsset(pMtrl->GetName(), pMtrl.Get()); // 쉐이더를 찾아서 재질에 새팅해준다.
        
        Ptr<AGraphicShader> pShader = AssetMgr::GetInst()->Find<AGraphicShader>(L"BillboardShader");
        pMtrl->SetShader(pShader);
        pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_OPAQUE);
        pMtrl->SetIsProvidedByEngine(true);
    }

    SetMaterial(pMtrl);   
}

void CBillboardRender::SaveToLevelFile(FILE* _File)
{
    CRenderComponent::SaveToLevelFile(_File);
    
    fwrite(&m_BillboardScale, sizeof(Vec2), 1, _File);
    // SaveAssetRef(_File, m_Tex.Get());
}

void CBillboardRender::LoadFromLevelFile(FILE* _File)
{
    CRenderComponent::LoadFromLevelFile(_File);
    
    fread(&m_BillboardScale, sizeof(Vec2), 1, _File);
    // m_Tex = LoadAssetRef<ATexture>(_File);
}
