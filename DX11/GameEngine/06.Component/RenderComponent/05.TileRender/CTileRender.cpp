#include "pch.h"
#include "CTileRender.h"

#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/06.Component/01.Transform/CTransform.h"

CTileRender::CTileRender()
    : CRenderComponent(COMPONENT_TYPE::TILE_RENDER)
{
    m_Buffer = new StructuredBuffer;
}

CTileRender::CTileRender(const CTileRender& _Origin)
    : CRenderComponent(_Origin)
    , m_TileMap(_Origin.m_TileMap)
    , m_vecSpriteInfo(_Origin.m_vecSpriteInfo)
{
    // 만약 들고 있었던 m_TileMap이 있었다면, 복사되는 TileRenderComponent에 대한 referencer도 ATileMap쪽으로 넘겨야함
    if (_Origin.m_TileMap) _Origin.m_TileMap->AddTileRenderReferencer(this);
    
    m_Buffer = new StructuredBuffer;
    SetTileMap(m_TileMap);
}

CTileRender::~CTileRender()
{
    // 레벨 Play 상태에서 Stop으로 돌아가는 과정에서 Play 상태였던 Level은 메모리 해제.
    // 여기서 CTileRender가 참조하는 TileMap이 있었다면, Referencer를 해제해주어야 함
    if (m_TileMap) m_TileMap->RemoveTileRenderReferencer(this);
}

void CTileRender::Init()
{
    CRenderComponent::Init();
    // SetTileMap(m_TileMap);
}

void CTileRender::FinalTick()
{
    CRenderComponent::FinalTick();
    
    // 크기조정
    if (m_TileMap)
    {
        
    }
}

void CTileRender::Render()
{
    if (!m_TileMap) return;
    
    m_Buffer->Binding(20);

    GetMaterial()->SetTexture(TEX_0, m_TileMap->GetAtlas());
    
    GetMaterial()->SetScalar(INT_0, m_TileMap->GetRow());
    GetMaterial()->SetScalar(INT_1, m_TileMap->GetCol());
    /*GetMaterial()->SetScalar(VEC2_0, m_vecSpriteInfo[0].LeftTop);
    GetMaterial()->SetScalar(VEC2_1, m_vecSpriteInfo[0].Slice);*/

    ApplyRenderTransformConst();
    GetMaterial()->Binding();
    
    GetMesh()->Render();
    
    m_Buffer->Clear();
}

void CTileRender::CreateMaterial()
{
    wstring MeshName    = L"RectMesh";
    wstring MtrlName    = L"TileMtrl";
    wstring ShaderName  = L"TileShader";

    // RectMesh 설정
    SetMesh(AssetMgr::GetInst()->Find<AMesh>(MeshName));
    
    // 빌보드 전용 재질 생성
    Ptr<AMaterial> pMtrl = AssetMgr::GetInst()->Find<AMaterial>(MtrlName);
    
    // 찾는 재질이 없으면 생성한다
    if (!pMtrl)
    {
        PrintIrrelevantDebugLog();
        pMtrl = new AMaterial;
        pMtrl->SetName(MtrlName);
        AssetMgr::GetInst()->AddAsset(pMtrl->GetName(), pMtrl.Get()); // 쉐이더를 찾아서 재질에 새팅해준다.
        
        // 찾는 쉐이더가 없으면 만들어서 에셋매니저에 등록해준다.
        Ptr<AGraphicShader> pShader = AssetMgr::GetInst()->Find<AGraphicShader>(ShaderName);
        pMtrl->SetShader(pShader);
        pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_OPAQUE);
        pMtrl->SetIsProvidedByEngine(true);
    }
    
    SetMaterial(pMtrl);   
}

void CTileRender::SetTileMap(const Ptr<ATileMap>& _TileMap)
{
    // 이전 TileMap이 세팅되어 있었고 해당 TileMap이 새로이 들어온 TileMap과 동일하지 않다면,
    // ATileMap Referencer에서 제거
    if (m_TileMap && m_TileMap != _TileMap)
        m_TileMap->RemoveTileRenderReferencer(this);
    
    m_TileMap = _TileMap;
    
    if (!m_TileMap) return;

    // Referencer 새로이 추가 (만일 기존에 이미 이 TilRender 객체가 있었다면 추가되지 않음)
    m_TileMap->AddTileRenderReferencer(this);
    
    // 이전 정보 리셋
    m_vecSpriteInfo.clear();
    
    // 크기조정 ->
    /*const UINT Row        = m_TileMap->GetRow();
    const UINT Col        = m_TileMap->GetCol();
    const Vec2 TileSize   = m_TileMap->GetTileSize();
    const Vec3 vScale     = Vec3(TileSize.x * static_cast<float>(Row), TileSize.y * static_cast<float>(Col), 1.f);
    Transform()->SetRelativeScale(vScale);*/
    
    // TileMap의 Sprite의 UV 정보를 받아옴
    const vector<Ptr<ASprite>>& vecSprites = m_TileMap->GetSprites();

    for (const Ptr<ASprite>& Sprite : vecSprites)
    {
        SpriteInfo info{};
        
        if (Sprite)
        {
            info.LeftTop = Sprite->GetLeftTopUV();
            info.Slice   = Sprite->GetSliceUV();
        }
        
        m_vecSpriteInfo.push_back(info);
    }
    
    // 구조화버퍼의 크기가 모자라거나 한번도 할당한 적이 없으면 메모리 확장 or 재확장
    if (m_Buffer->GetBufferSize() < sizeof(SpriteInfo) * m_vecSpriteInfo.size())
        m_Buffer->Create(sizeof(SpriteInfo), m_vecSpriteInfo.size(), SB_TYPE::SRV_ONLY, true);
    
    // Sprite들의 데이터를 구조화버퍼로 보내기
    m_Buffer->SetData(m_vecSpriteInfo.data(), sizeof(SpriteInfo) * m_vecSpriteInfo.size());
}

void CTileRender::SaveToLevelFile(FILE* _File)
{
    CRenderComponent::SaveToLevelFile(_File);
    SaveAssetRef(_File, m_TileMap.Get());
}

void CTileRender::LoadFromLevelFile(FILE* _File)
{
    CRenderComponent::LoadFromLevelFile(_File);
    
    m_TileMap = LoadAssetRef<ATileMap>(_File);
    SetTileMap(m_TileMap);
}
