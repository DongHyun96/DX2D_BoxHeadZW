#include "pch.h"
#include "ASetColorCS.h"


ASetColorCS::ASetColorCS()
    : AComputeShader(L"Shader\\setcolor.fx", "CS_SetColor", 32, 32, 1)
{
}

ASetColorCS::~ASetColorCS()
{
}

int ASetColorCS::Binding()
{
    if (nullptr == m_Target)
    {
        return E_FAIL;
    }

    m_Target->Binding_CS_UAV(0);
    m_Const.v4Arr[0] = m_Color;

    return S_OK;
}

void ASetColorCS::CalcGroupNum()
{
    m_GroupX = m_Target->GetWidth() / m_GroupPerThreadX;
    m_GroupY = m_Target->GetHeight() / m_GroupPerThreadY;
    m_GroupZ = 1;
}

void ASetColorCS::Clear()
{
    m_Target->Clear_CS_UAV();
}
