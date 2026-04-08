#pragma once

enum class FLIPBOOK_EFFECT_POOLER_TYPE;

/// <summary>
/// Effect Pooler로 Pooling 당할 GameObject의 Script
/// </summary>
class CFlipbookEffectScript : public CScript
{
private:

    wstring                     m_MainEffectAnimCategory{};
    FLIPBOOK_EFFECT_POOLER_TYPE m_PoolerType{};
    
public:
    
    CFlipbookEffectScript();
    virtual ~CFlipbookEffectScript() override;
    CLONE(CFlipbookEffectScript)
    
protected:
    
    CFlipbookEffectScript(SCRIPT_TYPE _Type, FLIPBOOK_EFFECT_POOLER_TYPE _PoolerType, const wstring& _MainEffectAnimCategory);
    
public:

    virtual void Init() override;
    virtual void Begin() override;
    virtual void AfterLevelBegin() override;
    virtual void Tick() override;

private:
    
    virtual void OnEffectAnimationEnd();
    
public:
    
    void SaveToLevelFile(FILE* _File) override;
    void LoadFromLevelFile(FILE* _File) override;
    
};
