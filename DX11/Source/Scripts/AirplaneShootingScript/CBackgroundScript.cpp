#include "pch.h"
#include "CBackgroundScript.h"

#include "GameEngine/02.Device/Device.h"
#include "Source/ScriptMgr.h"

CBackgroundScript::CBackgroundScript()
    : CScript(static_cast<int>(SCRIPT_TYPE::BACKGROUNDSCRIPT))
{
}

CBackgroundScript::CBackgroundScript(const CBackgroundScript& _Origin)
    : CScript(_Origin)
{
}

CBackgroundScript::~CBackgroundScript()
{
}

void CBackgroundScript::Tick()
{
    /*for (int i = 0; i < 2; i++)
    {
        CTransform* Transform = m_BackGrounds[i]->Transform().Get();
        
        Transform->SetPosY(Transform->GetPosY() - m_BackgroundSpeed * DT);
        
        if (Transform->GetPosY() < -Device::GetInst()->GetRenderResY()) // 위로 다시 setting
            Transform->SetPosY(Device::GetInst()->GetRenderResY());
    }*/
}

void CBackgroundScript::AddBackGroundObject(GameObject* _BackGroundObj, UINT slot)
{
    m_BackGrounds[slot] = _BackGroundObj;

    const float ResolX = Device::GetInst()->GetRenderResX();
    const float ResolY = Device::GetInst()->GetRenderResY();

    _BackGroundObj->Transform()->SetRelativeScale({ResolX, ResolY, 1.f});
    
    _BackGroundObj->Transform()->SetRelativePosZ(800.f);

    if (slot == 0) _BackGroundObj->Transform()->SetRelativePosY(0.f);
    if (slot == 1) _BackGroundObj->Transform()->SetRelativePosY(Device::GetInst()->GetRenderResY());
        
}
