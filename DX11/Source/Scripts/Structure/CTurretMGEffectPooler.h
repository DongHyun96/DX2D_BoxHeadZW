#pragma once

/// <summary>
/// Turret MG 자체적으로 들고 있을 EffectPooler 오브젝트에 붙일 CScript
/// </summary>
class CTurretMGEffectPooler : public CScript
{
private:
    
public:
    
    CTurretMGEffectPooler();
    virtual ~CTurretMGEffectPooler() override;
    CLONE(CTurretMGEffectPooler);
    
public:

    // Script의 Begin이 제일 먼저 호출이 됨 -> 그 다음이 Component 순서상, 여기서 붙일 수가 없음
    virtual void Begin() override;
    virtual void Tick() override;
    
public:
    
    void SaveToLevelFile(FILE* _File)   override {}
    void LoadFromLevelFile(FILE* _File) override {}
    
    
};
