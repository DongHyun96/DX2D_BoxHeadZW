#include "pch.h"
#include "CSpriteRender.h"
#include "SpriteRenderInstancing.h"

#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"

CSpriteRender::CSpriteRender()
    :CRenderComponent(COMPONENT_TYPE::SPRITE_RENDER)
{
}

CSpriteRender::~CSpriteRender()
{
}

void CSpriteRender::FinalTick()
{
    CRenderComponent::FinalTick();
}

void CSpriteRender::Render()
{
    if (!m_Sprite) return;

    SpriteRenderInstancing::Submit(
        GetMesh().Get(),
        GetMaterial()->GetShader().Get(),
        m_Sprite->GetAtlas().Get(),
        GetOwner()->Transform()->GetWorldMatrix(),
        m_Sprite->GetLeftTopUV(),
        m_Sprite->GetSliceUV()
    );
}

void CSpriteRender::SaveToLevelFile(FILE* _File)
{
    CRenderComponent::SaveToLevelFile(_File);
    SaveAssetRef(_File, m_Sprite.Get());
}

void CSpriteRender::LoadFromLevelFile(FILE* _File)
{
    CRenderComponent::LoadFromLevelFile(_File);
    m_Sprite = LoadAssetRef<ASprite>(_File);
}

void CSpriteRender::CreateMaterial()
{
    wstring MeshName    = L"RectMesh";
    wstring MtrlName    = L"SpriteMtrl";
    wstring ShaderName  = L"SpriteShader";

    // RectMesh 설정
    SetMesh(AssetMgr::GetInst()->Find<AMesh>(MeshName));

    // 빌보드 전용 재질 생성
    Ptr<AMaterial> pMtrl = AssetMgr::GetInst()->Find<AMaterial>(MtrlName);

    // 찾는 재질이 없으면 생성한다
    if (!pMtrl)
    {
        PrintIrrelevantDebugLog();
        pMtrl = new AMaterial;
        pMtrl->SetIsProvidedByEngine(true);
        pMtrl->SetName(MtrlName);
        AssetMgr::GetInst()->AddAsset(pMtrl->GetName(), pMtrl.Get()); // 쉐이더를 찾아서 재질에 새팅해준다.

        // 찾는 쉐이더가 없으면 만들어서 에셋매니저에 등록해준다.
        Ptr<AGraphicShader> pShader = AssetMgr::GetInst()->Find<AGraphicShader>(ShaderName);
        pMtrl->SetShader(pShader);
        pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_MASKED);
    }

    SetMaterial(pMtrl);   
}

void CSpriteRender::BeginInstancing()
{
    SpriteRenderInstancing::BeginFrame();
}

void CSpriteRender::FlushInstancing()
{
    SpriteRenderInstancing::Flush();
}
