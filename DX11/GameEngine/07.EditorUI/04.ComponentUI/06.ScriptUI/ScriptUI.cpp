#include "pch.h"
#include "ScriptUI.h"

#include <Source/ScriptMgr.h>

#include "GameEngine/03.Manager/09.EditorMgr/EditorMgr.h"
#include "GameEngine/07.EditorUI/07.TreeUI/TreeUI.h"

ScriptUI::ScriptUI()
	: ComponentUI(COMPONENT_TYPE::SCRIPT, "ScriptUI")
	, m_ItemHeight(0)
{
	// Inspector 가 표기하려는 GameObject 가 여러개의 Script 를 가지고 있을 수 있기 때문에
	// 각 Script 에 대응하는 ScriptUI 도 여러개가 될 수 있다.
	// 따라서 ScriptUI 끼리 Inspector 의 자식으로서 서로의 이름이 겹치지 않도록 추가로 Key 를 작성한다
	int idx = GetEntityInstID();
	char szNum[50]{};
	_itoa_s(idx, szNum, 50, 10);

	SetUIKey(szNum);
	
	SetSeparator(false);
}

ScriptUI::~ScriptUI()
{
}

void ScriptUI::SetScript(CScript* _Script)
{
	m_TargetScript = _Script;

	SetActive(m_TargetScript != nullptr);
	SetTargetObject(m_TargetScript ? m_TargetScript->GetOwner() : nullptr);
}

void ScriptUI::Tick_UI()
{
	if (m_TargetScript->GetScriptType() <= -1) return;
	
	m_ItemHeight = 0;

	// 스크립트 이름 출력
	ImGui::PushID(0);
	const ImVec4 Color = ImVec4(0, 0.38, 0.788, 1);
	ImGui::PushStyleColor(ImGuiCol_Button, Color);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Color);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, Color);

	wstring WScriptName = ScriptMgr::GetScriptName(m_TargetScript.Get());
	string ScriptName = string(WScriptName.begin(), WScriptName.end());

	ImGui::Button(ScriptName.c_str());
	AddItemHeight();

	ImGui::PopStyleColor(3);
	ImGui::PopID();

	ImGui::Separator();
	ImGui::SetNextItemOpen(false, ImGuiCond_FirstUseEver);
	
	const string CollapsingHeaderKey = "Serialized Params##" + GetUIKey();
	if (ImGui::CollapsingHeader(CollapsingHeaderKey.c_str(), ImGuiTreeNodeFlags_None))
		TickScriptParams();

	SetSizeAsChild(Vec2(0.f, static_cast<float>(m_ItemHeight)));
	
	ComponentUI::Tick_UI();
}

void ScriptUI::TickScriptParams()
{
	// Script 파라미터
	const vector<tScriptParam>& vecParam = m_TargetScript->GetScriptParam();

	for (size_t i = 0; i < vecParam.size(); ++i)
	{
		char ID[255]{};
		sprintf_s(ID, 255, "%d", i);

		ImGui::BeginDisabled(!vecParam[i].Enabled);
		switch (vecParam[i].Param)
		{
		case SCRIPT_PARAM::INT:
		{
			ImGui::Text(string(vecParam[i].Desc.begin(), vecParam[i].Desc.end()).c_str());
			ImGui::SameLine(120);

			string Key = "##Int";
			Key += ID;
			
			
			if (vecParam[i].IsInput) ImGui::InputInt(Key.c_str(), static_cast<int*>(vecParam[i].Data), vecParam[i].Step);
			else					 ImGui::DragInt(Key.c_str(), static_cast<int*>(vecParam[i].Data), vecParam[i].Step);
			AddItemHeight();
		}
			break;
		case SCRIPT_PARAM::FLOAT:
		{
			ImGui::Text(string(vecParam[i].Desc.begin(), vecParam[i].Desc.end()).c_str());
			ImGui::SameLine(120);

			string Key = "##Float";
			Key += ID;
			
			if (vecParam[i].IsInput) ImGui::InputFloat(Key.c_str(), static_cast<float*>(vecParam[i].Data), vecParam[i].Step);
			else					 ImGui::DragFloat(Key.c_str(), static_cast<float*>(vecParam[i].Data), vecParam[i].Step);

			AddItemHeight();
		}
		break;
		case SCRIPT_PARAM::VEC2:
		{
			ImGui::Text(string(vecParam[i].Desc.begin(), vecParam[i].Desc.end()).c_str());
			ImGui::SameLine(120);

			string Key = "##Vec2";
			Key += ID;

			Vec2* ReceivedVecAddress = static_cast<Vec2*>(vecParam[i].Data);
			
			if (vecParam[i].IsInput) ImGui::InputFloat2(Key.c_str(), *ReceivedVecAddress);
			else					 ImGui::DragFloat2(Key.c_str(), static_cast<float*>(vecParam[i].Data), vecParam[i].Step);

			AddItemHeight();
		}
		break;
		case SCRIPT_PARAM::VEC3:
		{
			ImGui::Text(string(vecParam[i].Desc.begin(), vecParam[i].Desc.end()).c_str());
			ImGui::SameLine(120);

			string Key = "##Vec3";
			Key += ID;

			Vec3* ReceivedVecAddress = static_cast<Vec3*>(vecParam[i].Data);
			
			if (vecParam[i].IsInput) ImGui::InputFloat3(Key.c_str(), *ReceivedVecAddress);
			else					 ImGui::DragFloat3(Key.c_str(), static_cast<float*>(vecParam[i].Data), vecParam[i].Step);

			AddItemHeight();
		}
		break;
		case SCRIPT_PARAM::VEC4:
		{
			
		}
			break;
		case SCRIPT_PARAM::MATRIX:
			break;
		case SCRIPT_PARAM::TEXTURE:
		{
			ImGui::Text(string(vecParam[i].Desc.begin(), vecParam[i].Desc.end()).c_str());
			string Key = "##Texture";
			Key += ID;

			Ptr<ATexture> pTex = *static_cast<Ptr<ATexture>*>(vecParam[i].Data);
			
			ImTextureRef TexID = (pTex) ? pTex->GetSRV().Get() : nullptr;
			
			// 이미지 샘플
			ImGui::ImageWithBg
			(
				TexID,
				ImVec2(200, 200),
				Vec2(0.f, 0.f), Vec2(1.f, 1.f),
				ImVec4(0.0f, 0.0f, 0.0f, 1.0f)
			);
			
			// 특정 위젯에서 드래그가 발생했고, 해당 위젯 위에 마우스가 호버링 중인지
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* PayLoad = ImGui::AcceptDragDropPayload("Content"))
				{
					if (!TreeUI::IsPayloadMultiData(PayLoad))
					{
						DWORD_PTR data = *static_cast<DWORD_PTR*>(PayLoad->Data);
						Ptr<Asset> pAsset = reinterpret_cast<Asset*>(data);

						if (ASSET_TYPE::TEXTURE == pAsset->GetType())
							*static_cast<Ptr<ATexture>*>(vecParam[i].Data) = static_cast<ATexture*>(pAsset.Get());
					}
				}

				ImGui::EndDragDropTarget();
			}

			AddItemHeight();
		}
			break;
		case SCRIPT_PARAM::MATERIAL:
			break;
		default:
			break;
		}
		ImGui::EndDisabled();
	}
}

void ScriptUI::AddItemHeight()
{
	ImVec2 vSize = ImGui::GetItemRectSize();
	m_ItemHeight += vSize.y + 200.f;
}

void ScriptUI::OnRemoveScriptConfirmed(bool _Confirmed)
{
	if (_Confirmed)
	{
		GetTargetObject()->RemoveScript(m_TargetScript);
		EditorMgr::GetInst()->SetTargetObjectToInspectors(GetTargetObject());
	}
}
