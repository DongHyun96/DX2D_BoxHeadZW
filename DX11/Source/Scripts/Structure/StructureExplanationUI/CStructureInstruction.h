#pragma once

enum class STRUCTURE_INSTRUCTION_STATE
{
    NONE,
    INSTALL_INSTRUCTION,
    REMOVE_INSTRUCTION
};

class CStructureInstruction : public CScript
{
private:

    STRUCTURE_INSTRUCTION_STATE m_State{}; // 현재 보여주어야 할 Instruction State

private:

    class CText*        m_InstallText{};
    CRenderComponent*   m_InstallImage{};
    
    CText*              m_RemoveText{};
    CRenderComponent*   m_RemoveImage{};
    
public:
    
    CStructureInstruction();
    virtual ~CStructureInstruction() override;
    CLONE(CStructureInstruction);
    
public:

    virtual void Begin() override;
    virtual void Tick() override;

public:
    
    void SetStructureInstructionState(STRUCTURE_INSTRUCTION_STATE _State) { m_State = _State; }
    void SetPos(const Vec2& _Pos) { Transform()->SetRelativePosXY(_Pos); }
    
private:
    
    /// <summary>
    /// Install ToolTip Lerp 처리 
    /// </summary>
    /// <returns> : 현재 Alpha 값 </returns>
    float LerpInstallAlpha(float _LerpDest);

    /// <summary>
    /// Remove ToolTip Lerp 처리
    /// </summary>
    /// <returns> : 현재 Alpha 값 </returns>
    float LerpRemoveAlpha(float _LerpDest);
    
public:
    
    void SaveToLevelFile(FILE* _File) override;
    void LoadFromLevelFile(FILE* _File) override;
    
};

