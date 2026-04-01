#include "pch.h"
#include "CMuzzleSmokeScript.h"

#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"

CMuzzleSmokeScript::CMuzzleSmokeScript()
    : CFlipbookEffectScript(SCRIPT_TYPE::MUZZLESMOKESCRIPT, FLIPBOOK_EFFECT_POOLER_TYPE::MUZZLE_SMOKE_POOLER, L"Smoke")
{
}

CMuzzleSmokeScript::~CMuzzleSmokeScript()
{
}

void CMuzzleSmokeScript::Begin()
{
    CFlipbookEffectScript::Begin();
    
    Ptr<AMaterial> DynamicMaterial = GetRenderCom()->CreateDynamicMaterial();
    DynamicMaterial->SetScalar(SCALAR_PARAM::VEC4_0, Vec4(1.f, 1.f, 1.f, 0.35f)); // 연기여서 알파를 좀 옅게 줌
}
