#include "pch.h"
#include "TileDecalInstancing.h"
#include "CTileRender.h"

#include <algorithm>

#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/06.Component/01.Transform/CTransform.h"

AMesh* CTileRender::s_RectMesh{};
AGraphicShader* CTileRender::s_TileDecalShader{};

CTileRender::CTileRender()
    : CRenderComponent(COMPONENT_TYPE::TILE_RENDER)
{
    m_TileBuffer = new StructuredBuffer;
}

CTileRender::CTileRender(const CTileRender& _Origin)
    : CRenderComponent(_Origin)
      , m_TileMap(_Origin.m_TileMap)
      , m_vecTileInfo(_Origin.m_vecTileInfo)
      , m_rsDecalInfo(_Origin.m_rsDecalInfo)
      , m_iNextDecalID(_Origin.m_iNextDecalID)
{
    if (m_TileMap) m_TileMap->AddTileRenderReferencer(this);

    m_TileBuffer = new StructuredBuffer;

    if (!m_vecTileInfo.empty())
    {
        m_TileBuffer->Create(sizeof(TileInfo), m_vecTileInfo.size(), SB_TYPE::SRV_ONLY, true);
        m_TileBuffer->SetData(m_vecTileInfo.data(), sizeof(TileInfo) * m_vecTileInfo.size());
    }
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

    if (!s_RectMesh) s_RectMesh = FIND_ASSET(AMesh, L"RectMesh").Get();
    if (!s_TileDecalShader) s_TileDecalShader = FIND_ASSET(AGraphicShader, L"TileDecalShader").Get();
}

void CTileRender::FinalTick()
{
    CRenderComponent::FinalTick();
}

void CTileRender::Render()
{
    if (!m_TileMap || m_vecTileInfo.empty()) return;

    m_TileBuffer->Binding(20);

    GetMaterial()->SetTexture(TEX_0, m_TileMap->GetAtlas());

    GetMaterial()->SetScalar(INT_0, m_TileMap->GetRow());
    GetMaterial()->SetScalar(INT_1, m_TileMap->GetCol());

    ApplyRenderTransformConst();
    GetMaterial()->Binding();

    GetMesh()->Render();

    m_TileBuffer->Clear();

    // 데칼 렌더링 (인스턴싱 제출)
    if (!m_rsDecalInfo.empty())
    {
        for (const auto& decal : m_rsDecalInfo)
        {
            TileDecalInstancing::Submit
            (
                s_RectMesh,
                s_TileDecalShader,
                decal.DecalAtlas,
                decal.matWorld,
                decal.LeftTop,
                decal.Slice,
                decal.TintColor
            );
        }
    }
}

void CTileRender::CreateMaterial()
{
    wstring MeshName   = L"RectMesh";
    wstring MtrlName   = L"TileMtrl";
    wstring ShaderName = L"TileShader";

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
    if (m_TileMap.Get() && m_TileMap.Get() != _TileMap.Get())
    {
        m_TileMap->RemoveTileRenderReferencer(this);
    }

    m_TileMap = _TileMap;

    if (!m_TileMap) return;

    // Referencer 새로이 추가 (만일 기존에 이미 이 TilRender 객체가 있었다면 추가되지 않음)
    m_TileMap->AddTileRenderReferencer(this);

    // 이전 정보 리셋
    m_vecTileInfo.clear();

    // 크기조정 ->
    /*const UINT Row        = m_TileMap->GetRow();
    const UINT Col        = m_TileMap->GetCol();
    const Vec2 TileSize   = m_TileMap->GetTileSize();
    const Vec3 vScale     = Vec3(TileSize.x * static_cast<float>(Row), TileSize.y * static_cast<float>(Col), 1.f);
    Transform()->SetRelativeScale(vScale);*/

    // TileMap의 Sprite의 UV 정보를 받아옴
    const vector<Ptr<ASprite>>& vecSprites = m_TileMap->GetSprites();

    m_vecTileInfo.clear();
    for (const Ptr<ASprite>& Sprite : vecSprites)
    {
        TileInfo info{};

        if (Sprite)
        {
            info.LeftTop = Sprite->GetLeftTopUV();
            info.Slice   = Sprite->GetSliceUV();
        }

        m_vecTileInfo.push_back(info);
    }

    // 구조화버퍼의 크기가 모자라거나 한번도 할당한 적이 없으면 메모리 확장 or 재확장
    if (m_TileBuffer->GetBufferSize() < sizeof(TileInfo) * m_vecTileInfo.size())
        m_TileBuffer->Create(sizeof(TileInfo), m_vecTileInfo.size(), SB_TYPE::SRV_ONLY, true);

    // Sprite들의 데이터를 구조화버퍼로 보내기
    m_TileBuffer->SetData(m_vecTileInfo.data(), sizeof(TileInfo) * m_vecTileInfo.size());
}

int CTileRender::AddDecal(ATexture* _DecalAtlasTexture, const Vec2& _vPos, const Vec2& _vScale, const Ptr<ASprite>& _pDecalSprite, const Vec4& _TintColor)
{
    if (!_DecalAtlasTexture) return -1;
    if (!_pDecalSprite || !m_TileMap) return -1;

    DecalInfo info{};
    info.Pos        = _vPos;
    info.Scale      = _vScale;
    info.LeftTop    = _pDecalSprite->GetLeftTopUV();
    info.Slice      = _pDecalSprite->GetSliceUV();
    info.TintColor  = _TintColor;
    info.ID         = m_iNextDecalID++;
    info.DecalAtlas = _DecalAtlasTexture;

    // matWorld 미리 계산
    const Matrix& matWorldTileMap = Transform()->GetWorldMatrix();

    // decal.Pos는 타일맵 UV 기준 (0~1)
    // 타일맵 로컬 좌표는 -0.5 ~ 0.5 범위 (RectMesh 기준)
    // const Vec3 vLocalPos = Vec3(info.Pos.x - 0.5f, -(info.Pos.y - 0.5f), 0.f);
    // const Vec3 vLocalPos = Vec3(info.Pos.x - 0.5f, info.Pos.y - 0.5f, 0.f);

    // Decal Scale 1.f 이 CellSize 1개의 크기
    const Vec2& CellSize = m_TileMap->GetTileSize();

    // 월드상에서의 데칼 크기
    const Vec3 DecalWorldScale = Vec3(CellSize.x * info.Scale.x, CellSize.y * info.Scale.y, 1.f);

    // 부모(타일맵)의 월드 스케일로 나누어서 로컬 스케일을 구함
    // Vec3 vDecalLocalScale = Vec3(vDecalWorldScale.x / vWorldScale.x, vDecalWorldScale.y / vWorldScale.y, 1.f);

    info.matWorld = Matrix::CreateScale(DecalWorldScale);
    info.matWorld *= Matrix::CreateTranslation(ToVec3(info.Pos, Transform()->GetWorldPos().z - 1000.f)); // 타일보다 살짝 앞으로

    m_rsDecalInfo.insert(info);
    m_bDecalChanged = true;

    return info.ID;
}

int CTileRender::AddDecal(ATexture* _DecalAtlasTexture, const Vec2& _vPos, const Vec2& _vScale, const Ptr<ASprite>& _pDecalSprite, float _ColorAlpha)
{
    Vec4 Color = DEF_COLOR_WHITE;
    Color.w    = _ColorAlpha;
    return AddDecal(_DecalAtlasTexture, _vPos, _vScale, _pDecalSprite, Color);
}

void CTileRender::RemoveDecal(int _ID)
{
    DecalInfo info{};
    info.ID = _ID;
    if (m_rsDecalInfo.remove(info)) m_bDecalChanged = true;
}

void CTileRender::ClearAllDecals()
{
    m_rsDecalInfo.clear();
    m_bDecalChanged = true;
}

DecalInfo* CTileRender::GetDecalInfo(int _ID)
{
    DecalInfo info{};
    info.ID = _ID;
    return m_rsDecalInfo.find(info);
}

void CTileRender::SetDecalAlpha(int _ID, float _Alpha)
{
    DecalInfo* pInfo = GetDecalInfo(_ID);
    if (pInfo)
    {
        pInfo->TintColor.w = _Alpha;
        m_bDecalChanged    = true;
    }
}


void CTileRender::SaveToLevelFile(FILE* _File)
{
    CRenderComponent::SaveToLevelFile(_File);
    SaveAssetRef(_File, m_TileMap.Get());

    // TileInfo 저장
    size_t TileCount = m_vecTileInfo.size();
    fwrite(&TileCount, sizeof(size_t), 1, _File);
    fwrite(m_vecTileInfo.data(), sizeof(TileInfo), TileCount, _File);

    // DecalInfo 저장
    size_t DecalCount = m_rsDecalInfo.size();
    fwrite(&DecalCount, sizeof(size_t), 1, _File);
    fwrite(m_rsDecalInfo.data(), sizeof(DecalInfo), DecalCount, _File);
}

void CTileRender::LoadFromLevelFile(FILE* _File)
{
    CRenderComponent::LoadFromLevelFile(_File);

    m_TileMap = LoadAssetRef<ATileMap>(_File);

    // TileInfo 로드
    size_t TileCount = 0;
    fread(&TileCount, sizeof(size_t), 1, _File);
    m_vecTileInfo.resize(TileCount);
    fread(m_vecTileInfo.data(), sizeof(TileInfo), TileCount, _File);

    // DecalInfo 로드
    size_t DecalCount = 0;
    fread(&DecalCount, sizeof(size_t), 1, _File);

    vector<DecalInfo> vecDecals;
    vecDecals.resize(DecalCount);
    fread(vecDecals.data(), sizeof(DecalInfo), DecalCount, _File);

    m_rsDecalInfo.clear();
    for (const auto& decal : vecDecals)
    {
        m_rsDecalInfo.insert(decal);
    }

    // 타일 버퍼 재생성 및 데이터 전송
    if (m_TileMap && !m_vecTileInfo.empty())
    {
        m_TileMap->AddTileRenderReferencer(this);
        m_TileBuffer->Create(sizeof(TileInfo), m_vecTileInfo.size(), SB_TYPE::SRV_ONLY, true);
        m_TileBuffer->SetData(m_vecTileInfo.data(), sizeof(TileInfo) * m_vecTileInfo.size());
    }

    // 데칼 버퍼 재생성 및 데이터 전송
    if (!m_rsDecalInfo.empty())
    {
        m_bDecalChanged = true;
    }

    // 데칼 ID 카운터 갱신
    m_iNextDecalID = 0;
    for (const auto& info : m_rsDecalInfo)
    {
        if (m_iNextDecalID <= info.ID)
            m_iNextDecalID = info.ID + 1;
    }
}
