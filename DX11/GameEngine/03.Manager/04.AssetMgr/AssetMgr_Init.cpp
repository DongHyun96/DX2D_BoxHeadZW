#include "pch.h"
#include "AssetMgr.h"

#include "GameEngine/03.Manager/01.PathMgr/PathMgr.h"
#include "Source/Scripts/AirplaneShootingScript/CBulletScript.h"

// #include "Header/assets.h"


void AssetMgr::Init()
{
    LoadAllTexMetaData();                   // Texture MetaData map 내용 불러오기
    LoadAllSoundMetaData();
    
    CreateEngineMesh();                     // Mesh - 코드 상에서 제작해야 하는 Asset
    CreateEngineShader();                   // Shader - fx shader 소스파일을 불러와 코드 상에서 제작해야 하는 Asset

    LoadAssetsFromAssetFolderRecursively<ASound>();
    LoadAssetsFromAssetFolderRecursively<ATexture>();
    LoadAssetsFromAssetFolderRecursively<AMaterial>();
    LoadAssetsFromAssetFolderRecursively<ASprite>();
    LoadAssetsFromAssetFolderRecursively<AFlipbook>();
    LoadAssetsFromAssetFolderRecursively<ATileMap>();
    
    CreateEngineMaterial();                             // Engine 제공 Material 제작 (Texture 불러오기 처리 때문에 이 순서로 해야 제대로 Texture가 입혀짐)

    LoadAssetsFromAssetFolderRecursively<APrefab>();
    LoadAssetsFromAssetFolderRecursively<ALevel>();     // 나머지 Asset들을 불러온 뒤, Level을 불러오는 순으로 하는게 좋음
    
    RemoveAnyDeletedTexturesMetaData();                 // 모든 Asset들을 불러온 뒤, 최종으로 Texture 메타데이터 정리할 부분이 있다면 처리함
    RemoveAnyDeletedSoundMetaData();

}

void AssetMgr::CreateEngineMesh()
{

    Ptr<AMesh> pMesh{};
    
    /***********************
    * 정점 데이터      
    ***********************/
    
    Vtx arrVtx[4] = {};

    arrVtx[0].vPos = Vec3(-0.5f, 0.5f, 0.f);
    arrVtx[0].vUV = Vec2(0.f, 0.f);
    arrVtx[0].vColor = GetRandomColor();

    arrVtx[1].vPos = Vec3(0.5f, 0.5f, 0.f);
    arrVtx[1].vUV = Vec2(1.f, 0.f); // Texture의 우측 상단 지점
    arrVtx[1].vColor = GetRandomColor();

    arrVtx[2].vPos = Vec3(0.5f, -0.5f, 0.f);
    arrVtx[2].vUV = Vec2(1.f, 1.f); // Texture의 우측 하단 지점 
    arrVtx[2].vColor = GetRandomColor();

    arrVtx[3].vPos = Vec3(-0.5f, -0.5f, 0.f);
    arrVtx[3].vUV = Vec2(0.f, 1.f); // Texture의 좌측 하단 지점
    arrVtx[3].vColor = GetRandomColor();

    UINT arrIdx[6] = { 0, 2, 3, 0, 1, 2 };

    /***********************
    * 사각형 mesh 생성      
    ***********************/
    
    pMesh = new AMesh;
    pMesh->Create(arrVtx, 4, arrIdx, 6);
    pMesh->SetName(L"RectMesh");
    pMesh->SetIsProvidedByEngine(true);
    AddAsset(L"RectMesh", pMesh.Get());
    
    /***********************
    * RectMesh_LineStrip
    ***********************/
    arrIdx[0] = 0; arrIdx[1] = 1; arrIdx[2] = 2; arrIdx[3] = 3; arrIdx[4] = 0;
    
    pMesh = new AMesh;
    pMesh->Create(arrVtx, 5, arrIdx, 5);
    pMesh->SetName(L"RectMesh_LineStrip");
    pMesh->SetIsProvidedByEngine(true);
    AddAsset(L"RectMesh_LineStrip", pMesh.Get());
    
    /***********************
    * LineMesh_LineStrip
    ***********************/
    Vtx lineVtx[2]{};
    lineVtx[0].vPos = Vec3(-0.5f, 0.f, 0.f);
    lineVtx[1].vPos = Vec3( 0.5f, 0.f, 0.f);

    UINT lineIdx[2] = { 0, 1 };

    Ptr<AMesh> pLineMesh = new AMesh;
    pLineMesh->Create(lineVtx, 2, lineIdx, 2);
    pLineMesh->SetName(L"LineMesh_LineStrip");
    pLineMesh->SetIsProvidedByEngine(true);
    AddAsset(L"LineMesh_LineStrip", pLineMesh.Get());
    

    /***********************
    * 삼각형 mesh 생성      
    ***********************/
    
    UINT arrIdx2[3] = { 0, 1, 2 };
    Ptr<AMesh> pTriMesh = new AMesh;
    pTriMesh->Create(arrVtx, 3, arrIdx2, 3);
    pTriMesh->SetName(L"TriMesh");
    pTriMesh->SetIsProvidedByEngine(true);
    AddAsset(L"TriMesh", pTriMesh.Get());

    
    /***********************
    * Circle mesh 생성      
    ***********************/
    vector<Vtx>     vecVtx{};
    vector<UINT>    vecIdx{};
    
    // 중점
    Vtx v{};
    v.vPos      = Vec3(0.f, 0.f, 0.f);
    v.vUV       = Vec2(0.5f, 0.5f);
    v.vColor    = Vec4(1.f, 1.f, 1.f, 1.f);
    vecVtx.push_back(v);
    
    float Theta     = 0.f;
    float Radius    = 0.5f;
    float Slice     = 40.f;

    // 정점 추가
    for (int i = 0; i < static_cast<int>(Slice) + 1; ++i)
    {
        v.vPos      = Vec3(Radius * cosf(Theta), Radius * sinf(Theta), 0.f);
        // v.vUV       = Vec2(0.5f, 0.5f);
        v.vColor    = Vec4(1.f, 1.f, 1.f, 1.f);
        vecVtx.push_back(v);
        
        Theta += XM_2PI / Slice;
    }
    
    // 인덱스
    for (int i = 0; i < static_cast<int>(Slice); ++i)
    {
        vecIdx.push_back(0);
        vecIdx.push_back(i + 2);
        vecIdx.push_back(i + 1);
    }
    
    pMesh = new AMesh;
    pMesh->Create(vecVtx.data(), vecVtx.size(), vecIdx.data(), vecIdx.size()); // vector::data() -> vector 내에 저장되어있는 배열의 시작 주소
    pMesh->SetName(L"CircleMesh");
    pMesh->SetIsProvidedByEngine(true);
    AddAsset(L"CircleMesh", pMesh.Get());
    
    /***********************
    * CircleMesh_LineStrip
    ***********************/

    vecIdx.clear();
    
    for (int i = 0; i < static_cast<int>(Slice) + 1; ++i)
        vecIdx.push_back(i + 1);
    
    pMesh = new AMesh;
    pMesh->Create(vecVtx.data(), vecVtx.size(), vecIdx.data(), vecIdx.size()); // vector::data() -> vector 내에 저장되어있는 배열의 시작 주소
    pMesh->SetName(L"CircleMesh_LineStrip");
    pMesh->SetIsProvidedByEngine(true);
    AddAsset(L"CircleMesh_LineStrip", pMesh.Get());
}

void AssetMgr::CreateEngineShader()
{
    /***********************
    * std2d shader
    ***********************/
    Ptr<AGraphicShader> shader{};

    shader = new AGraphicShader;
    shader->CreateVertexShader(L"Shader\\std2d.fx", "VS_Std2D");
    shader->CreatePixelShader(L"Shader\\std2d.fx", "PS_Std2D");
    shader->SetBSType(BS_TYPE::ALPHA_BLEND);
    shader->SetRSType(RS_TYPE::CULL_NONE);
    shader->SetDSType(DS_TYPE::LESS);
    
    // 2개의 Parameter
    shader->AddShaderParam(SHADER_PARAM::VEC4, 0, L"TintColor");
    shader->AddShaderParam(SHADER_PARAM::TEX, 0, L"OutColor");
    shader->SetIsProvidedByEngine(true); 
    AddAsset(L"Std2DShader", shader.Get());
    
    /***********************
    * DebugRenderShader
    ***********************/
    shader = new AGraphicShader;
    shader->CreateVertexShader(L"Shader\\dbg.fx", "VS_Debug");
    shader->CreatePixelShader(L"Shader\\dbg.fx", "PS_Debug");
    shader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
    shader->SetRSType(RS_TYPE::CULL_NONE);
    shader->SetBSType(BS_TYPE::DEFAULT);
    shader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
    shader->SetIsProvidedByEngine(true);
    AddAsset(L"DbgShader", shader.Get());
    
    // ===============
    // BillboardShader
    // ===============
    shader = new AGraphicShader;
    shader->SetName(L"BillboardShader");
    shader->CreateVertexShader(L"Shader\\billboard.fx", "VS_Billboard");
    shader->CreatePixelShader(L"Shader\\billboard.fx", "PS_Billboard");
    shader->SetBSType(BS_TYPE::DEFAULT);
    shader->SetRSType(RS_TYPE::CULL_NONE);
    shader->SetIsProvidedByEngine(true);
    AddAsset(shader->GetName(), shader.Get());


    // ============
    // SpriteShader
    // ============
    shader = new AGraphicShader;
    shader->SetName(L"SpriteShader");
    shader->CreateVertexShader(L"Shader\\sprite.fx", "VS_Sprite");
    shader->CreatePixelShader(L"Shader\\sprite.fx", "PS_Sprite");
    shader->SetBSType(BS_TYPE::ALPHA_BLEND);
    shader->SetRSType(RS_TYPE::CULL_NONE);
    shader->SetDSType(DS_TYPE::LESS);
    
    shader->SetIsProvidedByEngine(true);
    AddAsset(shader->GetName(), shader.Get());

    // ==============
    // FlipbookShader
    // ==============
    shader = new AGraphicShader;
    shader->SetName(L"FlipbookShader");
    shader->CreateVertexShader(L"Shader\\flipbook.fx","VS_Flipbook");
    shader->CreatePixelShader(L"Shader\\flipbook.fx", "PS_Flipbook");
    shader->SetBSType(BS_TYPE::ALPHA_BLEND);
    shader->SetRSType(RS_TYPE::CULL_NONE);
    shader->SetDSType(DS_TYPE::LESS_NO_WRITE);
    
    shader->SetIsProvidedByEngine(true);
    
    shader->AddShaderParam(SHADER_PARAM::VEC4, 0, L"TintColor");
    AddAsset(shader->GetName(), shader.Get());
    // =============
    // TileShader
    // =============

    shader = new AGraphicShader;
    shader->SetName(L"TileShader");
    shader->CreateVertexShader(L"Shader\\tile.fx", "VS_Tile");
    shader->CreatePixelShader(L"Shader\\tile.fx",  "PS_Tile");
    shader->SetBSType(BS_TYPE::DEFAULT);
    shader->SetRSType(RS_TYPE::CULL_NONE);
    shader->SetIsProvidedByEngine(true);
    AddAsset(shader->GetName(), shader.Get());
    
}

void AssetMgr::CreateEngineMaterial()
{
    Ptr<AMaterial> pMtrl{};
    
    /***********************
    * Bullet Material
    ***********************/
    pMtrl = new AMaterial;
    pMtrl->SetName(L"BulletMaterial");
    pMtrl->SetShader(Find<AGraphicShader>(L"Std2DShader")); 
    pMtrl->SetTexture(TEX_0, Find<ATexture>(L"Texture\\LaserProjectile.png"));
    pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_MASKED);
    
    pMtrl->SetScalar(VEC4_0, Vec4(1.f, 1.f, 1.f, 1.f)); // Tint Color
    
    pMtrl->SetIsProvidedByEngine(true);
    
    AddAsset(pMtrl->GetName(), pMtrl.Get());
    
    /***********************
    * Std2D Material
    ***********************/
    pMtrl = new AMaterial;
    pMtrl->SetName(L"Std2DMtrl");
    pMtrl->SetShader(Find<AGraphicShader>(L"Std2DShader")); 
    pMtrl->SetTexture(TEX_0, Find<ATexture>(L"Texture\\Fighter.bmp"));
    pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_MASKED);

    pMtrl->SetScalar(INT_0, 10); // 컴파일러 타입 추론 개입 -> int로 추론
    pMtrl->SetScalar(VEC4_0, Vec4(1.f, 1.f, 1.f, 1.f)); // Tint Color
    
    // pMtrl->SetScalar<int>(INT_0, 10);
    pMtrl->SetIsProvidedByEngine(true);
    AddAsset(pMtrl->GetName(), pMtrl.Get());
    
    /***********************
    * Monster Material
    ***********************/
    pMtrl = new AMaterial;
    pMtrl->SetName(L"MonsterMtrl");
    pMtrl->SetShader(Find<AGraphicShader>(L"Std2DShader"));
    pMtrl->SetTexture(TEX_0, Find<ATexture>(L"Texture\\LaserProjectile.png"));
    
    pMtrl->SetScalar(VEC4_0, Vec4(1.f, 1.f, 1.f, 1.f)); // Tint Color
    
    // pMtrl->SetTexture(TEX_0, Find<ATexture>(L"FighterTexture"));
    pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_MASKED);
    pMtrl->SetIsProvidedByEngine(true);
    AddAsset(pMtrl->GetName(), pMtrl.Get());

    /***********************
    * Debug Material
    ***********************/
    pMtrl = new AMaterial;
    pMtrl->SetName(L"DbgMtrl");
    pMtrl->SetShader(Find<AGraphicShader>(L"DbgShader"));
    pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_DEBUG);
    pMtrl->SetIsProvidedByEngine(true);
    AddAsset(pMtrl->GetName(), pMtrl.Get());

    // Load<AMaterial>(L"Material\\Default Material_0.mtrl", L"Material\\Default Material_0.mtrl");
}
