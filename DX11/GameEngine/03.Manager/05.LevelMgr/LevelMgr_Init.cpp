#include "pch.h"
#include "LevelMgr.h"
#include "GameEngine/02.Device/Device.h"
#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/04.Asset/11.ComputeShader/SetColorCS/ASetColorCS.h"
#include "Source/Scripts/CCamMoveScript.h"
#include "Source/Scripts/CCamPerspectiveMove.h"
#include "Source/Scripts/CMonsterScript.h"
#include "Source/Scripts/CharacterScript/PlayerScript/CPlayerScript.h"

void LevelMgr::CreateTestLevel()
{
    // Level 생성
    Ptr<ALevel> pLevel = new ALevel;
	const wstring LevelName = L"Level\\DefaultLevel.lv"; 
    pLevel->SetName(LevelName);
	pLevel->SetIsProvidedByEngine(true);
    
    // Paritlce Object
	Ptr<GameObject> pParticle = new GameObject;
	pParticle->SetName(L"Particle Object");

	pParticle->AddComponent(new CTransform);
	pParticle->AddComponent(new CParticleRender);

	pParticle->Transform()->SetRelativePos(Vec3(0.f, 0.f, -100.f));


	// 파티클 입자에 입힐 텍스쳐 설정
	Ptr<CParticleRender> pRender = pParticle->ParticleRender();
	
	Ptr<ATexture> pParticleTex = FIND_ASSET(ATexture, L"Texture\\FX_Flare.png");
	pRender->SetParticleTex(pParticleTex);

	// 스폰옵션 설정
	pRender->SetSpawnRate(2.f);
	pRender->SetSpawnShape(0);
	pRender->SetSpawnShapeScale(Vec3(500.f, 500.f, 500.f)); // 파티클 생성 영역
	pRender->SetMinLife(1.f);
	pRender->SetMaxLife(3.f);
	pRender->SetMinScale(Vec3(100.f, 30.f, 10.f));
	pRender->SetMaxScale(Vec3(200.f, 60.f, 20.f));

	// 파티클 시뮬레이션 좌표계 설정(Local or World)
	pRender->SetSpaceType(0);

	// AddVelocity 모듈 설정
	pRender->SetModlue(PARTICLE_MODULE::ADD_VELOCITY, true);
	pRender->SetAddVelocityType(3);
	pRender->SetMinSpeed(1000.f);
	pRender->SetMaxSpeed(1200.f);
	pRender->SetFixedVelocity(Vec3(0.f, -1.f, 0.f));

	// SpawnBurst 모듈 설정
	pRender->SetModlue(PARTICLE_MODULE::SPAWN_BURST, true);
	pRender->SetBurstParticleCount(200);
	pRender->SetBurstRepeatCount(10);
	pRender->SetBurstTerm(1.f);

	// Scale 
	pRender->SetModlue(PARTICLE_MODULE::SCALE, true);
	pRender->SetStartScale(2.f);
	pRender->SetEndScale(2.f);

	// Drag : 감속
	pRender->SetModlue(PARTICLE_MODULE::DRAG, false);
	pRender->SetDragDestNormalizeAge(0.9f);  // 드래그 시작시점
	pRender->SetDragLimitSpeed(0.f);		// 최종 속도 제한

	// NoiseForce : 랜덤 힘
	pRender->SetModlue(PARTICLE_MODULE::NOISE_FORCE, true);
	pRender->SetNoiseForceTerm(0.2f);
	pRender->SetNoiseForceScale(300.f);

	// ===========
	// Render 모듈
	// ===========
	pRender->SetModlue(PARTICLE_MODULE::RENDER, true);
	pRender->SetStartColor(Vec4(0.8f, 0.2f, 0.44f, 1.f));
	pRender->SetEndColor(Vec4(0.41f, 0.24f, 0.78f, 1.f));

	pRender->SetFadeOut(false);
	pRender->SetFadOutStartRatio(0.7f);

	// 속도 정렬기능
	pRender->SetVelocityAlignment(true, true);

	pLevel->AddObject(0, pParticle);
	
	// 카메라 오브젝트
	// 카메라 역할 오브젝트 
	Ptr<GameObject> pObject = new GameObject;
	pObject->SetName(L"MainCamera");

	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CCamera);
	pObject->AddComponent(new CCamPerspectiveMove);

	pObject->Camera()->LayerCheckAll();
	//pObject->Camera()->LayerCheck(0); 
	//pObject->Camera()->LayerCheck(1); 
	//pObject->Camera()->LayerCheck(2);

	pObject->Camera()->SetType(PROJ_TYPE::PERSPECTIVE);
	pObject->Camera()->SetFar(10000.f);
	pObject->Camera()->SetFOV(90.f);
	pObject->Camera()->SetOrthoScale(1.f);
	Vec2 vResolution = Device::GetInst()->GetRenderResolution();
	pObject->Camera()->SetAspectRatio(vResolution.x / vResolution.y); // 종횡비(AspectRatio)
	pObject->Camera()->SetWidth(vResolution.x);
	
	pLevel->AddObject(0, pObject);
    
	
    // 레벨 변경점 체크
    pLevel->SetChanged();
    
    // 레벨을 AssetMgr에 등록
    AssetMgr::GetInst()->AddAsset(LevelName, pLevel.Get());

    // 레벨을 파일로 저장
    //wstring ContentPath = CONTENT_PATH;
    //pLevel->Save(ContentPath + L"Level\\TestLevel.lv");
    
    // 레벨을 변경
    // ChangeLevel(L"Level\\TestLevel.lv", true);
}

