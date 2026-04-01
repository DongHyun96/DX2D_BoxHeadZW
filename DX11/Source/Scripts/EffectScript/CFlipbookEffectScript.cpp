#include "pch.h"
#include "CFlipbookEffectScript.h"

#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"

CFlipbookEffectScript::CFlipbookEffectScript()
    : CScript(SCRIPT_TYPE::FLIPBOOKEFFECTSCRIPT)
{
}

CFlipbookEffectScript::~CFlipbookEffectScript()
{
}

CFlipbookEffectScript::CFlipbookEffectScript
(
    SCRIPT_TYPE _Type,
    FLIPBOOK_EFFECT_POOLER_TYPE _PoolerType,
    const wstring& _MainEffectAnimCategory
)
    : CScript(_Type)
    , m_MainEffectAnimCategory(_MainEffectAnimCategory)
    , m_PoolerType(_PoolerType)
{
}

void CFlipbookEffectScript::Init()
{
    AddScriptParam(SCRIPT_PARAM::WSTRING, &m_MainEffectAnimCategory, L"MainEffectAnimCategory");
    AddScriptParam(SCRIPT_PARAM::INT, &m_PoolerType, L"PoolerType");
}

void CFlipbookEffectScript::Begin()
{
    FlipbookRender()->SetCurrentCategory(m_MainEffectAnimCategory);
    FlipbookRender()->AddNotifyFlipbookEndEvent(m_MainEffectAnimCategory, 0, bind(&CFlipbookEffectScript::OnEffectAnimationEnd, this));
}

void CFlipbookEffectScript::AfterLevelBegin()
{
    // GM에 Owner PoolComponent 등록
    if (!GM->GetFlipbookEffectPooler(m_PoolerType))
    {
        // ObjectPooling 처리된 객체의 OwnerPooler에 한해서만 등록 처리를 진행시킴 (Testing 환경에서 Pooling 되지 않은 오브젝트가 있을 수 있음)
        if (CPoolComponent* PoolComponent = GetOwner()->GetOwnerPoolComponent())
            GM->AddFlipbookEffectPooler(m_PoolerType, PoolComponent);
    }
}

void CFlipbookEffectScript::Tick()
{
}

void CFlipbookEffectScript::OnEffectAnimationEnd()
{
    GetOwner()->SetActive(false);
}

// Begin 이후에야 비로소 생성되기 때문에 저장이 제대로 안될수 있음
void CFlipbookEffectScript::SaveToLevelFile(FILE* _File)
{
    SaveWString(_File, m_MainEffectAnimCategory);
    UINT PoolerTypeToUINT = static_cast<UINT>(m_PoolerType);
    fwrite(&PoolerTypeToUINT, sizeof(UINT), 1, _File);
}

void CFlipbookEffectScript::LoadFromLevelFile(FILE* _File)
{
    m_MainEffectAnimCategory = LoadWString(_File);
    UINT PoolerTypeToUINT{};
    fread(&PoolerTypeToUINT, sizeof(UINT), 1, _File);
    m_PoolerType = static_cast<FLIPBOOK_EFFECT_POOLER_TYPE>(PoolerTypeToUINT);
}
