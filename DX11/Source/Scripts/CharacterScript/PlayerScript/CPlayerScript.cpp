#include "pch.h"
#include "CPlayerScript.h"

#include "CPlayerWeaponHandler.h"
#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/04.Asset/10.Sound/ASound.h"
#include "GameEngine/05.GameObject/GameObject.h"
#include "GameEngine/06.Component/01.Transform/CTransform.h"
#include "GameEngine/06.Component/03.Collider2D/CColliderRect.h"
#include "InvenScript/CEquipmentScript.h"
#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/CharacterScript/EnemyScript/CEnemyScript.h"
#include "Source/Scripts/StatScript/CStatScript.h"

CPlayerScript::CPlayerScript()
    : CCharacterScript(SCRIPT_TYPE::PLAYERSCRIPT)
{
}

CPlayerScript::~CPlayerScript()
{
}

void CPlayerScript::Init()
{

    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_MoveSpeedBase, L"PlayerSpeedBase", false);
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_MoveSpeedFactor, L"PlayerSpeedFactor", false);
    
    /*AddScriptParam(SCRIPT_PARAM::FLOAT, &m_Speed, L"PlayerSpeed", false);
    AddScriptParam(SCRIPT_PARAM::TEXTURE, &m_Tex, L"TextureExample");
    
    m_BulletPrefab = LOAD_ASSET(APrefab, L"Prefab\\Bullet.pref");*/
    // if (!APrefab::IsPrefabPrototypeObject(GetOwner()))
    
}

void CPlayerScript::Begin()
{
    GM->SetPlayerObject(GetOwner());
    
    /*if (GetOwner()->FlipbookRender())
        GetOwner()->FlipbookRender()->Stop(L"UnArmed", 6, 0);*/
    
    GM->SetMainPlayerScript(this);
    //Ptr<ASound> pSound = LOAD_ASSET(ASound, L"Sound\\ParadiseOnE.wav");
    //pSound->Play(0, 0.5f, false);
    
    /*Collider2D()->AddDynamicBeginOverlap(this, static_cast<COLLISION_EVENT>(&CBulletScript::BeginOverlap));
    Collider2D()->AddDynamicOverlap     (this, static_cast<COLLISION_EVENT>(&CBulletScript::Overlap));
    Collider2D()->AddDynamicEndOverlap  (this, static_cast<COLLISION_EVENT>(&CBulletScript::EndOverlap));*/
    ADD_DYNAMIC_BEGIN_OVERLAP(CPlayerScript::BodyColliderOverlapped);
    ADD_DYNAMIC_OVERLAP(CPlayerScript::BodyColliderOverlapped);
}

void CPlayerScript::AfterLevelBegin()
{
    CCharacterScript::AfterLevelBegin();
    GetOwner()->GetScriptComponent<CPlayerWeaponHandler>()->SetHandState(PLAYER_HANDSTATE::PISTOL);    
}

void CPlayerScript::Tick()
{
    CCharacterScript::Tick();
}

void CPlayerScript::Move()
{
    Transform()->SetPrevRelativePos(Transform()->GetRelativePos()); // 이동 처리 직전에 이전 PrevPos 저장(blocking 처리용)
    
    switch (m_PlayerMainState)
    {
    case PLAYER_MAINSTATE::DIE: case PLAYER_MAINSTATE::END: return;
        
    case PLAYER_MAINSTATE::IDLE:
    {
        m_Velocity = Vec3(); // Velocity 초기화
        Vec3 Direction{};
        
        if (KEY_PRESSED(KEY::A)) Direction.x -= 1.f; // Left
        if (KEY_PRESSED(KEY::D)) Direction.x += 1.f; // Right
        if (KEY_PRESSED(KEY::W)) Direction.y += 1.f; // Up
        if (KEY_PRESSED(KEY::S)) Direction.y -= 1.f; // Down

        // TODO : 이 라인 지우기 (testing 환경에서의 Fast Move 처리)
        m_MoveSpeedFactor = KEY_PRESSED(KEY::LSHIFT) ? 2.f : 1.f; 

        if (Direction.LengthSquared() == 0.f) return;
        Direction.Normalize();
        
        m_Velocity = Direction * m_MoveSpeedBase * m_MoveSpeedFactor;
        
        Vec3 Pos = Transform()->GetRelativePos() + m_Velocity * DT;
        Transform()->SetRelativePos(Pos);
    }
        return;
        
    case PLAYER_MAINSTATE::PUSHED_OUT: MovePushedOut(); break;
        
    }
}

void CPlayerScript::UpdateCurrentFacedDirection()
{
    switch (m_PlayerMainState) 
    {
    // PushedOut과 Die의 경우, Flipbook 관련 방향 처리가 다르기 때문에 FacedDirection Update 필요 없이 따로 처리
    case PLAYER_MAINSTATE::PUSHED_OUT: case PLAYER_MAINSTATE::DIE: case PLAYER_MAINSTATE::END: return;
    case PLAYER_MAINSTATE::IDLE:
    {
        const Vec2 MousePos     = ToVec2(KeyMgr::GetInst()->GetMouseWorldPos());
        const Vec2 PlayerPos2D  = ToVec2(Transform()->GetRelativePos());
    
        m_PlayerToMousePos      = MousePos - PlayerPos2D;
        m_CurrentFacedDirection = GetEightDirection(m_PlayerToMousePos);
    
        // 이 경우, 마우스포인터 좌표와 Player의 위치가 완전히 일치하는 상황 (거의 아예 안나올거다)
        // 따로 DOWN 방향으로 처리
        if (m_CurrentFacedDirection == EDIRECTION::END) m_CurrentFacedDirection = EDIRECTION::DOWN;
    }
        return;
    }
    
}

void CPlayerScript::AfterPushedOutFin()
{
    PLAYER_MAINSTATE NextState = GetOwner()->GetScriptComponent<CStatScript>()->IsDead()
                                         ? PLAYER_MAINSTATE::DIE : PLAYER_MAINSTATE::IDLE;
    SetMainState(NextState);    
}

PLAYER_HANDSTATE CPlayerScript::GetHandState() const
{
    return GetOwner()->GetScriptComponent<CPlayerWeaponHandler>()->GetHandState();
}

void CPlayerScript::BodyColliderOverlapped(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider)
{
    if (_OtherCollider->GetOwner()->GetScriptComponent<CEnemyScript>())
    {
        Transform()->UpdateTransformToPrevRelativePos();        
    }
}

void CPlayerScript::SaveToLevelFile(FILE* _File)
{
    CCharacterScript::SaveToLevelFile(_File);
}

void CPlayerScript::LoadFromLevelFile(FILE* _File)
{
    CCharacterScript::LoadFromLevelFile(_File);
}
