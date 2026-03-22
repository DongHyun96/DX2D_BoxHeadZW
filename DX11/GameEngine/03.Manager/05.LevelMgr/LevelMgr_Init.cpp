#include "pch.h"
#include "LevelMgr.h"
#include "GameEngine/02.Device/Device.h"
#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "Source/Scripts/CCamMoveScript.h"
#include "Source/Scripts/CMonsterScript.h"
#include "Source/Scripts/CPlayerScript.h"

void LevelMgr::CreateTestLevel()
{
    // Level 생성
    Ptr<ALevel> pLevel = new ALevel;
    pLevel->SetName(L"Level\\TestLevel.lv");

    /*for (UINT i = 0; i < static_cast<UINT>(LAYER_TYPE::END); ++i)
    {
        const LAYER_TYPE Type = static_cast<LAYER_TYPE>(i);
        // pLevel->GetLayer(i)->SetName(Layer::LAYER_NAMES[Type]);
    }*/
    
    pLevel->GetLayer(0)->SetName(L"Default");
    pLevel->GetLayer(1)->SetName(L"Background");
    pLevel->GetLayer(2)->SetName(L"Tile");
    
    pLevel->GetLayer(3)->SetName(L"Player");
    pLevel->GetLayer(4)->SetName(L"PlayerProjectile");
    
    pLevel->GetLayer(5)->SetName(L"Enemy");
    pLevel->GetLayer(6)->SetName(L"EnemyProjectile");
    
    // AssetMgr::GetInst()->AddAsset(pLevel->GetName(), pLevel.Get());
    
    Ptr<GameObject> pObject = nullptr;	
	

    // 카메라 역할 오브젝트 
    pObject = new GameObject;
    pObject->SetName(L"MainCamera");

    pObject->AddComponent(new CTransform);
    pObject->AddComponent(new CCamera);
    pObject->AddComponent(new CCamMoveScript);

    pObject->Camera()->LayerCheckAll();
    //pObject->Camera()->LayerCheck(0); 
    //pObject->Camera()->LayerCheck(1); 
    //pObject->Camera()->LayerCheck(2);

    pObject->Camera()->SetType(PROJ_TYPE::ORTHOGRAPHIC);
    pObject->Camera()->SetFar(10000.f);
    pObject->Camera()->SetFOV(90.f);
    pObject->Camera()->SetOrthoScale(1.f);
    Vec2 vResolution = Device::GetInst()->GetRenderResolution();
    pObject->Camera()->SetAspectRatio(vResolution.x / vResolution.y); // 종횡비(AspectRatio)
    pObject->Camera()->SetWidth(vResolution.x);
	
    pLevel->AddObject(0, pObject);
    
    // 광원 추가
    pObject = new GameObject;
    pObject->SetName(L"DirectionalLight");
    pObject->AddComponent(new CTransform);
    pObject->AddComponent(new CLight2D);

    // Directional light
    pObject->Light2D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
    pObject->Light2D()->SetLightColor(Vec3(0.5f, 0.5f, 0.5f));
    // pObject->Light2D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
    pObject->Light2D()->SetAmbient(Vec3(0.05f, 0.05f, 0.05f));
    pObject->Transform()->SetRelativePos(Vec3(0.f, 0.f, 0.f)); // z좌표는 의미 없음
    pLevel->AddObject(0, pObject);

    // SpotLight
    Ptr<GameObject> SpotLight = new GameObject;
    SpotLight->SetName(L"Player SpotLight");
    SpotLight->AddComponent(new CTransform);
    SpotLight->AddComponent(new CLight2D);

    SpotLight->Light2D()->SetLightType(LIGHT_TYPE::SPOT);
    SpotLight->Light2D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
    SpotLight->Light2D()->SetRadius(800.f);
    SpotLight->Light2D()->SetAngle(XM_PI * 0.25f);
    SpotLight->Transform()->SetIndependentScale(true);
    
    // pLevel->AddObject(0, pObject);
    
    // pObject->Transform()->SetRelativePos(Vec3(-200.f, 0.f, 0.f)); // z좌표는 의미 없음
    
    /*pObject = new GameObject;
    pObject->SetName(L"Light 2");
    pObject->AddComponent(new CTransform);
    pObject->AddComponent(new CLight2D);

    pObject->Light2D()->SetLightType(LIGHT_TYPE::POINT);
    pObject->Light2D()->SetLightColor(Vec3(0.f, 0.f, 1.f));
    pObject->Light2D()->SetRadius(350.f);
    
    
    pObject->Transform()->SetRelativePos(Vec3(200.f, 0.f, 0.f)); // z좌표는 의미 없음
    pLevel->AddObject(0, pObject);*/
    

    // MeshRender Object test
    Ptr<GameObject> pMeshRenderObj = new GameObject;
    pMeshRenderObj->SetName(L"MeshRenderTester");

    pMeshRenderObj->AddComponent(new CTransform);
    pMeshRenderObj->AddComponent(new CMeshRender);

    pMeshRenderObj->Transform()->SetRelativePos(Vec3(-200.f, 0.f, 100.f));	
    pMeshRenderObj->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 0.f));
    pMeshRenderObj->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"RectMesh")); 
    pMeshRenderObj->MeshRender()->SetMaterial(AssetMgr::GetInst()->Find<AMaterial>(L"Std2DMtrl"));
    pLevel->AddObject(0, pMeshRenderObj);
    
    // Billboard Object test
    Ptr<GameObject> pBillboardObj = new GameObject;
    pBillboardObj->SetName(L"BillboardRenderTester");

    pBillboardObj->AddComponent(new CTransform);
    pBillboardObj->AddComponent(new CBillboardRender);
    pBillboardObj->AddComponent(new CColliderCircle);

    pBillboardObj->Transform()->SetRelativePos(Vec3(-200.f, 200.f, 100.f));	
    pBillboardObj->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 0.f));
    pBillboardObj->BillboardRender()->SetBillboardScale(Vec2(50.f, 50.f));
    pBillboardObj->BillboardRender()->GetMaterial()->SetTexture(TEX_0, FIND_ASSET(ATexture, L"Texture\\LaserProjectile.png"));
    
    pLevel->AddObject(0, pBillboardObj);
    
    
    // 몬스터 오브젝트 생성
    for (int i = 0; i < 5; ++i)
    {
        Ptr<GameObject> pMonster = new GameObject;
        pMonster->SetName(L"Monster");

        pMonster->AddComponent(new CTransform);
        pMonster->AddComponent(new CMeshRender);
        pMonster->AddComponent(new CColliderRect);
        pMonster->AddComponent(new CMonsterScript);

        pMonster->Transform()->SetRelativePos(Vec3(300.f * static_cast<float>(i), 0.f, 100.f));	
        pMonster->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 1.f));

        pMonster->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"RectMesh"));
        pMonster->MeshRender()->SetMaterial(FIND_ASSET(AMaterial, L"MonsterMtrl"));
        
        // pMonster->Collider2D()->SetScale(Vec2(50.f, 50.f));
	    
        pLevel->AddObject(5, pMonster);
    }

    // Player Object 추가
    Ptr<GameObject> PlayerObject = new GameObject; 
    PlayerObject->SetName(L"Player");
    PlayerObject->AddComponent(new CTransform);
    // PlayerObject->AddComponent(new CMeshRender);
    // PlayerObject->AddComponent(new CSpriteRender);
    PlayerObject->AddComponent(new CFlipbookRender);
    PlayerObject->AddComponent(new CPlayerScript);
    PlayerObject->AddComponent(new CColliderRect);
    // PlayerObject->AddComponent(new CColliderCircle);
    // PlayerObject->AddComponent(new CColliderPoint);
    
    // PlayerObject->GetScriptComponent<CPlayerScript>()->SetTarget(pMonster);

    PlayerObject->Transform()->SetRelativePos(Vec3(0.f, 0.f, 100.f));
    PlayerObject->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));

    // PlayerObject->SpriteRender()->SetSprite(FIND_ASSET(ASprite, L"LinkSprite"));
    
    PlayerObject->FlipbookRender()->AddFlipbook(L"Default", LOAD_ASSET(AFlipbook, L"Flipbook\\Link_MoveDown.flip"));
    PlayerObject->FlipbookRender()->AddFlipbook(L"Default", LOAD_ASSET(AFlipbook, L"Flipbook\\Link_MoveUp.flip"));
    PlayerObject->FlipbookRender()->AddFlipbook(L"Default", LOAD_ASSET(AFlipbook, L"Flipbook\\Link_MoveLeft.flip"));
    PlayerObject->FlipbookRender()->AddFlipbook(L"Default", LOAD_ASSET(AFlipbook, L"Flipbook\\Link_MoveRight.flip"));
    
    PlayerObject->FlipbookRender()->Play(L"Default",0, 10.f, 5);
    //PlayerObject->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"RectMesh"));
    //PlayerObject->MeshRender()->SetMaterial(AssetMgr::GetInst()->Find<AMaterial>(L"Std2DMtrl"));
    
    PlayerObject->GetCollider2D()->SetOffset({0.f, -0.1f});
    if (CColliderRect* ColliderRect = dynamic_cast<CColliderRect*>(PlayerObject->GetCollider2D().Get()))
        ColliderRect->SetScale({0.25f, 0.5f});
    if (CColliderCircle* ColliderCircle = dynamic_cast<CColliderCircle*>(PlayerObject->GetCollider2D().Get()))
        ColliderCircle->SetRadius(200.f);
    // if (CColliderPoint* ColliderPoint = dynamic_cast<CColliderPoint*>(PlayerObject->Collider2D().Get()))
    

    // Child Object 추가
    Ptr<GameObject> pChild = new GameObject;
    pChild->SetName(L"ChildObject");
    pChild->AddComponent(new CTransform);
    pChild->AddComponent(new CMeshRender);
    pChild->AddComponent(new CColliderRect);

    pChild->Transform()->SetIndependentScale(true);
    pChild->Transform()->SetRelativePos(Vec3(-200.f, 0.f, 0.f));
    pChild->Transform()->SetRelativeScale(Vec3(50.f, 50.f, 1.f));

    pChild->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"RectMesh"));
    pChild->MeshRender()->SetMaterial(AssetMgr::GetInst()->Find<AMaterial>(L"Std2DMtrl"));
    
    Ptr<GameObject> pChild2 = new GameObject;
    pChild2->SetName(L"ChildObject2");
    pChild2->AddComponent(new CTransform);
    pChild2->AddComponent(new CMeshRender);
    pChild2->AddComponent(new CColliderRect);

    pChild2->Transform()->SetIndependentScale(true);
    pChild2->Transform()->SetRelativePos(Vec3(-200.f, 0.f, 0.f));
    pChild2->Transform()->SetRelativeScale(Vec3(50.f, 50.f, 1.f));

    pChild2->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"RectMesh"));
    pChild2->MeshRender()->SetMaterial(AssetMgr::GetInst()->Find<AMaterial>(L"Std2DMtrl"));

    //PlayerObject->AddChild(pChild);
    //pChild->AddChild(pChild2);
    
    PlayerObject->AddChild(SpotLight);

    // pLevel->AddObject(0, pChild);
    
    // Player(부모 오브젝트)만 레벨에 추가
    pLevel->AddObject(3, PlayerObject);
    
    // Tile Object
    Ptr<GameObject> pTileObj = new GameObject;

    pTileObj->AddComponent(new CTransform);
    pTileObj->AddComponent(new CColliderRect);
    pTileObj->AddComponent(new CTileRender);

    pTileObj->TileRender()->SetTileMap(LOAD_ASSET(ATileMap, L"TileMap\\TestTileMap.tilemap"));
    pTileObj->Transform()->SetRelativeScale(Vec3(5000.f, 5000.f, 1.f));
    Vec3 Scale = pTileObj->Transform()->GetRelativeScale(); 
    pTileObj->Transform()->SetRelativePos(Vec3(-Scale.x * 0.5f, Scale.y * 0.5f, 500.f));
    
    pLevel->AddObject(2, pTileObj);
    
    
    /*// BulletMgr Object 추가
    Ptr<GameObject> BulletManager = new GameObject;
    BulletManager->SetName(L"BulletManager");
    Ptr<CBulletMgrScript> BMgrScript = new CBulletMgrScript; 
    BulletManager->AddComponent(BMgrScript.Get());
    pLevel->AddObject(0, BulletManager);
    
    PlayerObject->GetScriptComponent<CPlayerScript>()->SetBulletMgrScript(BMgrScript);
    
    // Bullet Object 추가
    for (int i = 0; i < CBulletMgrScript::BULLET_POOL_COUNT; ++i)
    {
        Ptr<GameObject> BulletObj = new GameObject;
        
        const wstring BulletObjectName = L"Bullet" + to_wstring(i); 
        BulletObj->SetName(BulletObjectName);
        
        BulletObj->AddComponent(new CBulletScript);
        BulletObj->AddComponent(new CTransform);
        BulletObj->AddComponent(new CMeshRender);
        BulletObj->AddComponent(new CColliderRect);
        
        BulletObj->MeshRender()->SetMesh(FIND_ASSET(AMesh, L"RectMesh"));
        BulletObj->MeshRender()->SetMaterial(FIND_ASSET(AMaterial, L"BulletMaterial"));
        
        BulletObj->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));
        BulletObj->Transform()->SetRelativeRot(Vec3(0.f, 0.f, XM_PI * 0.5f));
        
        BMgrScript->AddBulletToPool(BulletObj);
        pLevel->AddObject(4, BulletObj);
    }*/
    

    // 레벨 충돌 설정
    pLevel->CheckCollisionLayer(3, 5); // Player vs monster
    pLevel->CheckCollisionLayer(4, 5); // Player Projectile vs Monster
    pLevel->CheckCollisionLayer(3, 6); // EnemyProjectile vs Player
    
    // 레벨 변경점 체크
    pLevel->SetChanged();
    
    // 레벨을 AssetMgr에 등록
    AssetMgr::GetInst()->AddAsset(L"Level\\TestLevel.lv", pLevel.Get());

    // 레벨을 파일로 저장
    wstring ContentPath = CONTENT_PATH;
    pLevel->Save(ContentPath + L"Level\\TestLevel.lv");
    
    // 레벨을 변경
    ChangeLevel(L"Level\\TestLevel.lv");
}

void LevelMgr::LoadTestLevel()
{
    Ptr<ALevel> pLevel = LOAD_ASSET(ALevel, L"Level\\TestLevel.lv");
    
    if (!pLevel)
    {
        CreateTestLevel();
        return;
    }
    
    ChangeLevel(L"Level\\TestLevel.lv");
}
