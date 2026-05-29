#include "pch.h"
#include "CStructureInstruction.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "Source/ScriptMgr.h"

#include "Source/Scripts/UIScript/CText.h"

CStructureInstruction::CStructureInstruction()
    : CScript(SCRIPT_TYPE::STRUCTUREINSTRUCTION)
{
}

CStructureInstruction::~CStructureInstruction()
{
}

void CStructureInstruction::Begin()
{
    if (const Ptr<GameObject>& InstallTipGO = GetOwner()->GetChildByName(L"InstallStructureInstructor"))
    {
        if (const Ptr<GameObject>& TextChild = InstallTipGO->GetChildByName(L"InstallInstructionText"))
        {
            m_InstallText = TextChild->GetScriptComponent<CText>().Get();
            m_InstallText->SetAlpha(0.f);
        }
        else DebugUtil::AddDebugLog("[CStructureInstruction::Begin] : Cannot find TextChild");
        
        if (const Ptr<GameObject>& ImageChild = InstallTipGO->GetChildByName(L"InstallInstructionImage"))
        {
            m_InstallImage = ImageChild->GetRenderCom().Get();
            m_InstallImage->GetMaterial()->SetTintColorAlpha(0.f);
        }
        else DebugUtil::AddDebugLog("[CStructureInstruction::Begin] : Cannot find ImageChild");
        
        
    } else DebugUtil::AddDebugLog("[CStructureInstruction::Begin] : Cannot find InstallTipGO");
    
    if (const Ptr<GameObject>& RemoveTipGO = GetOwner()->GetChildByName(L"RemoveStructureInstructor"))
    {
        if (const Ptr<GameObject>& TextChild = RemoveTipGO->GetChildByName(L"RemoveInstructionText"))
        {
            m_RemoveText = TextChild->GetScriptComponent<CText>().Get();
            m_RemoveText->SetAlpha(0.f);
        }
        else DebugUtil::AddDebugLog("[CStructureInstruction::Begin] : Cannot find TextChild");

        if (const Ptr<GameObject>& ImageChild = RemoveTipGO->GetChildByName(L"RemoveInstructionImage"))
        {
            m_RemoveImage = ImageChild->GetRenderCom().Get();
            m_RemoveImage->GetMaterial()->SetTintColorAlpha(0.f);
        }
        else DebugUtil::AddDebugLog("[CStructureInstruction::Begin] : Cannot find ImageChild");
        
    } else DebugUtil::AddDebugLog("[CStructureInstruction::Begin] : Cannot find RemoveTipGO");
}

void CStructureInstruction::Tick()
{
    switch (m_State)
    {
    case STRUCTURE_INSTRUCTION_STATE::NONE:
    {
        // 둘 다 바로 Lerp 처리
        LerpInstallAlpha(0.f);
        LerpRemoveAlpha(0.f);
    }
        return;
    case STRUCTURE_INSTRUCTION_STATE::INSTALL_INSTRUCTION:
    {
        // Remove Lerp Alpha 먼저 시도
        const float RemoveCurrentAlpha = LerpRemoveAlpha(0.f);
        
        // Remove Lerp 처리가 아직 진행 중이라고 판단되면 기다림
        if (RemoveCurrentAlpha > 0.1f) return;
        
        // Remove Lerp가 모두 진행되었다고 판단, Install ToolTip Lerp 처리
        LerpInstallAlpha(1.f);
    }
        return;
    case STRUCTURE_INSTRUCTION_STATE::REMOVE_INSTRUCTION:
    {
        // Install Lerp Alpha 먼저 시도
        const float InstallCurrentAlpha = LerpInstallAlpha(0.f);
        
        // Install Lerp 처리가 아직 진행 중이라고 판단되면 기다림
        if (InstallCurrentAlpha > 0.1f) return;
        
        // Install Lerp가 모두 진행되었다고 판단, Remove ToolTip Lerp 처리
        LerpRemoveAlpha(1.f);
    }
        return;
    }
}

float CStructureInstruction::LerpInstallAlpha(float _LerpDest)
{
    float InstallAlpha = m_InstallText->GetAlpha();
    InstallAlpha       = Lerp(InstallAlpha, _LerpDest, DT * 15.f);
        
    m_InstallText->SetAlpha(InstallAlpha);
    m_InstallImage->GetMaterial()->SetTintColorAlpha(InstallAlpha);
    
    return InstallAlpha;
}

float CStructureInstruction::LerpRemoveAlpha(float _LerpDest)
{
    float RemoveAlpha = m_RemoveText->GetAlpha();
    RemoveAlpha       = Lerp(RemoveAlpha, _LerpDest, DT * 15.f);
        
    m_RemoveText->SetAlpha(RemoveAlpha);
    m_RemoveImage->GetMaterial()->SetTintColorAlpha(RemoveAlpha);
    
    return RemoveAlpha;
}

void CStructureInstruction::SaveToLevelFile(FILE* _File)
{
}

void CStructureInstruction::LoadFromLevelFile(FILE* _File)
{
}
