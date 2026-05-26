#include "pch.h"
#include "CItem.h"

#include <algorithm>

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/06.Component/03.Collider2D/CCollider2D.h"
#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"

#include "Source/Scripts/CharacterScript/CharacterStat/PlayerStat/CPlayerStat.h"
#include "Source/Scripts/CharacterScript/PlayerScript/InvenScript/CInvenScript.h"
#include "Source/Scripts/UIScript/InGameUIManager/CIngameUIManager.h"

CItem::CItem()
    : CScript(SCRIPT_TYPE::ITEM)
    , m_LifeTime(10.f)
    , m_MaxLifeTime(10.f)
{
}

CItem::~CItem()
{
}

void CItem::AfterLevelBegin()
{
    CScript::AfterLevelBegin();
    
    // GM에 Owner PoolComponent 등록
    if (!GM->GetItemPooler())
    {
        // ObjectPooling 처리된 객체의 OwnerPooler에 한해서만 등록 처리를 진행시킴 (Testing 환경에서 Pooling 되지 않은 오브젝트가 있을 수 있음)
        if (CPoolComponent* PoolComponent = GetOwner()->GetOwnerPoolComponent())
            GM->SetItemPooler(PoolComponent);
    }
}

void CItem::Begin()
{
    ADD_DYNAMIC_BEGIN_OVERLAP(CItem::OnColliderBeginOverlap);
    m_DynamicMtrl = GetOwner()->GetRenderCom()->CreateDynamicMaterial();
}

void CItem::Tick()
{
    m_LifeTime -= DT;
    
    // 여기서 서서히 Render TintColor alpha 0으로
    float fAlpha = 1.f;
    if (m_MaxLifeTime > 0.001f) fAlpha = m_LifeTime / m_MaxLifeTime;
    fAlpha = clamp(fAlpha, 0.f, 1.f);

    Vec4 vColor = DEF_COLOR_WHITE;
    if (m_DynamicMtrl) vColor = m_DynamicMtrl->GetScalar<Vec4>(VEC4_0);
        
    vColor.w = fAlpha;
    
    if (m_DynamicMtrl) m_DynamicMtrl->SetScalar(VEC4_0, vColor);
    
    if (m_LifeTime < 0.f)
    {
        GetOwner()->SetActive(false);
        m_LifeTime = m_MaxLifeTime;
    }
}

void CItem::OnColliderBeginOverlap(CCollider2D* _Owner, CCollider2D* _Other)
{
    GameObject* pOtherObj = _Other->GetOwner();
    CPlayerStat* pPlayerStat = pOtherObj->GetScriptComponent<CPlayerStat>().Get();
    
    if (!m_EarnedSound) m_EarnedSound = FIND_ASSET(ASound, L"Sound\\PickUpAmmo.wav");
    m_EarnedSound->Play(1, 0.5f, true);
    
    if (pPlayerStat)
    {
        m_LifeTime = m_MaxLifeTime;    
        GetOwner()->SetActive(false);

        // 충돌할 수 있는 충돌체는 Player 뿐이다 Player에게 랜덤하게 버프 주기
        // Heal, 구조물 하나 선택해서 늘리기(CInvenScript), 탄알 수 늘리기(CInvenScript)
        int iRandom = GetRandom(0, 2);

        if (iRandom == 0)
        {
            // 1. Heal
            pPlayerStat->ApplyHeal(GetRandom(30.f, 60.f));
        }
        else if (iRandom == 1)
        {
            // 2. 구조물 하나 선택해서 늘리기(CInvenScript)
            CInvenScript* pInven = pOtherObj->GetScriptComponent<CInvenScript>().Get();
            if (pInven)
            {
                const PLAYER_STRUCTURE_TYPE type = static_cast<PLAYER_STRUCTURE_TYPE>(rand() % static_cast<int>(PLAYER_STRUCTURE_TYPE::END));
                const int Amount = GetRandom(5, 10);
                pInven->IncreaseCurrentStructureCount(type, Amount);
            }
        }
        else
        {
            // 3. 탄알 수 늘리기(CInvenScript)
            CInvenScript* pInven = pOtherObj->GetScriptComponent<CInvenScript>().Get();
            if (pInven)
            {
                // PISTOL(1)은 무한이므로 제외하고 UZI(2) ~ ROCKET(5) 중 선택
                PLAYER_HANDSTATE type = static_cast<PLAYER_HANDSTATE>(rand() % 4 + 2);
                pInven->IncreaseCurrentAmmoCount(type, 50);
            }
        }
    }
}
