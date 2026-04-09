#include "pch.h"
#include "CExplosion.h"

#include <algorithm>

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "Source/ScriptMgr.h"

const Vec2 CExplosion::s_ScaleSizeBase = { 200.f, 200.f };

CExplosion::CExplosion()
    : CExplosionDome(SCRIPT_TYPE::EXPLOSION)
{
}

CExplosion::~CExplosion()
{
}

void CExplosion::Begin()
{
    CExplosionDome::Begin();
    GetOwner()->AddActivateDelegate(bind(&CExplosion::OnActivateOwnerObject, this, placeholders::_1));
    GetOwner()->SetIgnoreGlobalTimeScale(true); // 슬로모션 또는 빨리감기 영향 x
}

void CExplosion::Tick()
{
    CExplosionDome::Tick();
    
    /*Vec2 RenderOffset = GetRenderCom()->GetRenderOffset();
    RenderOffset += m_UpwardVelocity * DT;
    GetRenderCom()->SetRenderOffset(RenderOffset);
    
    GetRenderCom()->SetRenderScale({GetRandom(0.8f, 1.1f), GetRandom(0.8f, 1.1f)});*/
    
    // 누적 시간(Age)이 필요합니다. (클래스 멤버 변수로 m_Age 혹은 m_Time 등을 관리한다고 가정)
    m_Age += DT; 

    // -----------------------------------------------------------
    // 1. 가짜 펄린 노이즈 생성 (부드러운 일렁임)
    // 서로 다른 소수(Prime) 주기를 가진 삼각함수를 더해 불규칙하면서도 부드러운 곡선을 만듭니다.
    // -----------------------------------------------------------
    float noise1 = sinf(m_Age * 30.0f);
    float noise2 = cosf(m_Age * 46.0f);
    float pseudoNoise = (noise1 + noise2) * 0.5f; // -1.0 ~ 1.0 사이의 부드러운 난수

    // -----------------------------------------------------------
    // 2. Render Offset (위로 상승 + 좌우 일렁임)
    // -----------------------------------------------------------
    Vec2 RenderOffset = GetRenderCom()->GetRenderOffset();

    // 1. 상승 속도 점검: m_UpwardVelocity가 픽셀 단위라면 이 역시 매우 크게 작용합니다.
    // NDC 기준이므로 Y축으로 초당 화면의 5% 정도만 올라가게 하려면 0.05f 정도가 적당합니다.
    // 만약 m_UpwardVelocity가 픽셀 값이라면 상수로 임의 스케일링을 해주거나 NDC 전용 속도를 써야 합니다.
    // float ndcUpwardSpeed = 0.1f; // 초당 화면 세로 길이의 2.5% 이동 (가정)
    RenderOffset += m_UpwardVelocity * DT;

    // 2. 좌우 흔들림 폭을 NDC에 맞게 대폭 축소
    // 0.01f ~ 0.02f 정도면 화면 가로폭의 0.5% ~ 1% 정도 부드럽게 흔들리게 됩니다.
    float ndcSwayAmplitude = 0.015f; 
    RenderOffset.x += pseudoNoise * ndcSwayAmplitude * DT; 

    GetRenderCom()->SetRenderOffset(RenderOffset);

    // -----------------------------------------------------------
    // 3. Render Scale (생명 주기에 따른 감소 + 노이즈 기반 떨림)
    // -----------------------------------------------------------
    // 폭발의 전체 수명 대비 현재 수명 비율 (0.0 ~ 1.0)
    // m_MaxLifeTime은 폭발이 지속되는 총 시간이라고 가정합니다.
    float lifeRatio = m_Age / m_MaxLifeTime; 
    
    // 시간에 따라 크기가 서서히 줄어들게 만듭니다 (Ease-out 효과)
    // 단순히 1.0 - lifeRatio 보다 곡선을 주면 더 자연스럽습니다.
    float baseScale = 1.0f - (lifeRatio * lifeRatio);
    baseScale = max(baseScale, 0.f);

    // 기본 스케일에 노이즈를 더해 불꽃이 파르르 떠는 느낌을 줍니다 (+/- 15% 정도)
    float flickerScale = 1.0f + (pseudoNoise * 0.15f); 
    
    // 화염은 보통 가로보다 세로로 길게 늘어지므로 Y축 스케일을 조금 더 줍니다.
    float finalScaleX = baseScale * flickerScale;
    float finalScaleY = baseScale * flickerScale * 1.2f; 

    GetRenderCom()->SetRenderScale({finalScaleX, finalScaleY});
    
}

void CExplosion::SetExplosionSize(float _Factor)
{
    Transform()->SetRelativeScaleXY(s_ScaleSizeBase * _Factor); 
}

void CExplosion::OnActivateOwnerObject(const Ptr<GameObject>& _OwnerObject)
{
    // DebugUtil::AddDebugLog("CExplosion::OnActivateOwnerObject");
    
    GetRenderCom()->SetRenderOffset({0.f, 0.f});
    GetRenderCom()->SetRenderScale({1.f, 1.f});
    
    GetCollider2D()->SetActive(m_UseCollisionForDamaging);
    
    m_Age = 0.f;
}
