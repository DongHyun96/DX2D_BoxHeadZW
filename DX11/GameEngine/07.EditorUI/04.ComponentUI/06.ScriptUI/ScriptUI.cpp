#include "pch.h"
#include "ScriptUI.h"

#include <Source/ScriptMgr.h>
#include "Source/Scripts/BackgroundTile/CBackgroundTile.h"

#include "GameEngine/03.Manager/09.EditorMgr/EditorMgr.h"
#include "GameEngine/04.Asset/10.Sound/ASound.h"
#include "GameEngine/07.EditorUI/07.TreeUI/TreeUI.h"

#include "Source/Scripts/RoundHandler/CRoundHandler.h"
#include "Source/Scripts/UIScript/CProgressBar.h"
#include "GameEngine/03.Manager/10.FontMgr/FontMgr.h"
#include "GameEngine/05.GameObject/GameObjectRefHolder.h"

namespace
{
	std::string WStringToUtf8(const std::wstring& wstr) {
		if (wstr.empty()) return std::string();
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
		std::string result(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &result[0], size_needed, NULL, NULL);
		return result;
	}

	// UTF-8 -> WString 변환 예시 (Windows API 사용)
	std::wstring Utf8ToWString(const std::string& str) {
		if (str.empty()) return std::wstring();
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
		std::wstring result(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &result[0], size_needed);
		return result;
	}

	int ResizeInputBuffer(ImGuiInputTextCallbackData* data)
	{
		if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
		{
			auto* buffer = static_cast<vector<char>*>(data->UserData);
			IM_ASSERT(buffer && data->Buf == buffer->data());
			buffer->resize(data->BufSize);
			data->Buf = buffer->data();
		}
		return 0;
	}

	bool InputTextDynamic(const char* label, vector<char>& buffer, ImGuiInputTextFlags flags = 0)
	{
		if (buffer.empty()) buffer.resize(1, '\0');
		return ImGui::InputText(label, buffer.data(), buffer.size(),
			flags | ImGuiInputTextFlags_CallbackResize, ResizeInputBuffer, &buffer);
	}
}

bool ScriptUI::s_BackgroundTileCellEditingEnabled = false;
CBackgroundTile* ScriptUI::s_BackgroundTileEditingTarget = nullptr;
TILE_EDIT_BRUSH ScriptUI::s_BackgroundTileBrush = TILE_EDIT_BRUSH::NONE;
FIRST_SPAWN_LOC ScriptUI::s_BackgroundTileSelectedSpawnLoc = FIRST_SPAWN_LOC_END;


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
	CScript* prevScript = m_TargetScript.Get();
	if (prevScript && prevScript == s_BackgroundTileEditingTarget)
		s_BackgroundTileEditingTarget = nullptr;

	m_TargetScript = _Script;
	// if (m_TargetScript && m_TargetScript->GetScriptType() == 12)
	// 	s_BackgroundTileEditingTarget = static_cast<CBackgroundTile*>(m_TargetScript.Get());

	if (m_TargetScript)
	{
		if (m_TargetScript->GetScriptType() == BACKGROUNDTILE)
			s_BackgroundTileEditingTarget = static_cast<CBackgroundTile*>(m_TargetScript.Get());
	}

	SetActive(m_TargetScript != nullptr);
	SetTargetObject(m_TargetScript ? m_TargetScript->GetOwner() : nullptr);
	m_WStringInputBuffer.clear();
}

void ScriptUI::Tick_UI()
{
	if (m_TargetScript->GetScriptType() <= -1) return;
	
	ImGui::PushID(GetUIKey().c_str());
	
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
	{
		TickScriptParams();
		TickBackgroundTileEditingUI();
	}

	SetSizeAsChild(Vec2(0.f, static_cast<float>(m_ItemHeight)));
	
	ComponentUI::Tick_UI();
	ImGui::PopID();
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
			Key += ID + GetUIKey();
			
			
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
			Key += ID + GetUIKey();
			
			if (vecParam[i].IsInput) ImGui::InputFloat(Key.c_str(), static_cast<float*>(vecParam[i].Data), vecParam[i].Step);
			else					 ImGui::DragFloat(Key.c_str(), static_cast<float*>(vecParam[i].Data), vecParam[i].Step);

			AddItemHeight();
		}
		break;
		case SCRIPT_PARAM::PROGRESS_BAR:
		{
			ImGui::Text(string(vecParam[i].Desc.begin(), vecParam[i].Desc.end()).c_str());
			ImGui::SameLine(120);

			string Key = "##ProgressBar";
			Key += ID + GetUIKey();

			CProgressBar* Progressbar = static_cast<CProgressBar*>(vecParam[i].Data);
			float Ratio = Progressbar->GetRatio();
			
			if (ImGui::InputFloat(Key.c_str(), &Ratio, vecParam[i].Step))
				Progressbar->SetRatio(Ratio);
			
			AddItemHeight();
		}
			break;
		case SCRIPT_PARAM::VEC2:
		{
			ImGui::Text(string(vecParam[i].Desc.begin(), vecParam[i].Desc.end()).c_str());
			ImGui::SameLine(120);

			string Key = "##Vec2";
			Key += ID + GetUIKey();

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
			Key += ID + GetUIKey();

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
		case SCRIPT_PARAM::COLOR:
		{
			ImGui::Text(string(vecParam[i].Desc.begin(), vecParam[i].Desc.end()).c_str());
			ImGui::SameLine(120);

			string Key = "##Color";
			Key += ID + GetUIKey();

			Vec4* ReceivedVecAddress = static_cast<Vec4*>(vecParam[i].Data);
			
			
			static ImGuiColorEditFlags base_flags = ImGuiColorEditFlags_Float;
			ImGui::ColorEdit4(Key.c_str(), *ReceivedVecAddress, base_flags);
			AddItemHeight();
			
		}
			break;
		case SCRIPT_PARAM::WSTRING:
		{
			ImGui::Text(string(vecParam[i].Desc.begin(), vecParam[i].Desc.end()).c_str());

			wstring* target = static_cast<wstring*>(vecParam[i].Data);
			if (!target)
			{
				AddItemHeight();
				break;
			}

			string key = "##WString";
			key += ID + GetUIKey();

			vector<char>& buffer = m_WStringInputBuffer[vecParam[i].Data];
			const string sourceUtf8 = WStringToUtf8(*target);

			// C-Style 문자열 비교를 사용하여 실제 문자열 데이터만 비교
			if (buffer.empty() || strcmp(buffer.data(), sourceUtf8.c_str()) != 0) {
				buffer.clear();
				// 변환된 UTF-8 데이터와 널 문자까지 포함하여 버퍼에 삽입
				buffer.insert(buffer.end(), sourceUtf8.begin(), sourceUtf8.end());
				buffer.push_back('\0'); 
				// 고정 크기(64바이트)로 조정하는 로직은 제거합니다.
			}

			// 1. 기본 상태 (아무 옵션도 없는 0으로 초기화)
			ImGuiInputTextFlags flags = 0;

			// 2. 만약 해당 파라미터가 입력 가능한 상태(IsInput)가 아니라면,
			if (!vecParam[i].IsInput) 
			{
				// '읽기 전용' 속성을 덧붙입니다.
				flags |= ImGuiInputTextFlags_ReadOnly; 
			}
			
			if (InputTextDynamic(key.c_str(), buffer, flags)) 
			{
				// 버퍼 데이터를 WString으로 다시 변환하여 원본에 반영
				*target = Utf8ToWString(buffer.data()); 
			}

			AddItemHeight();   
		}
			break;
		case SCRIPT_PARAM::FONT_STYLE:
		{
			ImGui::Text(string(vecParam[i].Desc.begin(), vecParam[i].Desc.end()).c_str());
			ImGui::SameLine(120);

			wstring* target = static_cast<wstring*>(vecParam[i].Data);
			const vector<wstring>& vecFontName = FontMgr::GetInst()->GetFontNames();

			string Key = "##FontSelection";
			Key += ID + GetUIKey();

			string currentStyle = WStringToUtf8(*target);
			if (currentStyle.empty()) currentStyle = "None";

			if (ImGui::BeginCombo(Key.c_str(), currentStyle.c_str()))
			{
				bool isNoneSelected = (*target == L"None" || target->empty());
				if (ImGui::Selectable("None", isNoneSelected))
				{
					*target = L"None";
					// [추가] 폰트가 바뀌면 해당 스크립트의 문자열 입력 버퍼를 강제로 동기화하도록 제거
					// 다음 Tick에서 WSTRING 케이스가 새로운 target 값을 기준으로 버퍼를 다시 생성합니다.
					m_WStringInputBuffer.erase(vecParam[i].Data);
				}

				for (const auto& fontName : vecFontName)
				{
					string name = WStringToUtf8(fontName);
					bool isSelected = (fontName == *target);
					if (ImGui::Selectable(name.c_str(), isSelected))
					{
						*target = fontName;
						// [추가] 폰트가 바뀌면 해당 스크립트의 문자열 입력 버퍼를 강제로 동기화하도록 제거
						// 다음 Tick에서 WSTRING 케이스가 새로운 target 값을 기준으로 버퍼를 다시 생성합니다.
						m_WStringInputBuffer.erase(vecParam[i].Data);
					}
				}
				ImGui::EndCombo();
			}
			AddItemHeight();
		}
		break;
		case SCRIPT_PARAM::FONT_ALIGN:
		{
			// 1. 라벨 출력 (기존 방식과 동일)
			ImGui::Text(string(vecParam[i].Desc.begin(), vecParam[i].Desc.end()).c_str());
			ImGui::SameLine(120);

			// 2. 타겟 데이터 포인터 캐스팅
			FONT_ALIGN* target = static_cast<FONT_ALIGN*>(vecParam[i].Data);

			// 3. ImGui 고유 키 생성
			string Key = "##FontAlignSelection";
			Key += ID + GetUIKey();

			// 4. 표시할 정렬 옵션 이름들
			const char* alignNames[] = { "Left", "Center", "Right" };
    
			// 현재 선택된 정렬의 인덱스 추출 (enum class를 int로 변환)
			int currentIdx = static_cast<int>(*target);
    
			// 안전 장치: 범위를 벗어나면 Left(0)로 고정
			if (currentIdx < 0 || currentIdx > 2) currentIdx = 0;

			// 5. 콤보 박스 그리기
			if (ImGui::BeginCombo(Key.c_str(), alignNames[currentIdx]))
			{
				for (int n = 0; n < IM_ARRAYSIZE(alignNames); n++)
				{
					bool isSelected = (currentIdx == n);
					if (ImGui::Selectable(alignNames[n], isSelected))
					{
						// 선택된 항목의 인덱스를 다시 FONT_ALIGN enum으로 캐스팅하여 저장
						*target = static_cast<FONT_ALIGN>(n);
					}

					// 콤보 박스를 열었을 때 현재 선택된 항목으로 포커스 이동
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			AddItemHeight();
		}
			break;
		case SCRIPT_PARAM::MATRIX:
			break;
		case SCRIPT_PARAM::TEXTURE:
		{
			ImGui::Text(string(vecParam[i].Desc.begin(), vecParam[i].Desc.end()).c_str());

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
		case SCRIPT_PARAM::SOUND: // Sound 멤버변수 Init
		{
			ImGui::Text(string(vecParam[i].Desc.begin(), vecParam[i].Desc.end()).c_str());
			string Key = "##Texture";
			Key += ID + GetUIKey();

			Ptr<ASound> pSound = *static_cast<Ptr<ASound>*>(vecParam[i].Data); 
			wstring SoundKey = pSound ? pSound->GetKey() : L"None";
			ImGui::InputText(Key.c_str(), string(SoundKey.begin(), SoundKey.end()).data(), SoundKey.length() + 1, ImGuiInputTextFlags_ReadOnly);
			
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* PayLoad = ImGui::AcceptDragDropPayload("Content"))
				{
					if (!TreeUI::IsPayloadMultiData(PayLoad))
					{
						DWORD_PTR data = *static_cast<DWORD_PTR*>(PayLoad->Data);
						Ptr<Asset> pAsset = reinterpret_cast<Asset*>(data);

						if (ASSET_TYPE::SOUND == pAsset->GetType())
						{
							ASound* ReceivedSound = static_cast<ASound*>(pAsset.Get()); 
							*static_cast<Ptr<ASound>*>(vecParam[i].Data) = ReceivedSound;
						}
					}
				}
				ImGui::EndDragDropTarget();
			}
		}
			break;
		case SCRIPT_PARAM::ROUND_INFO_VECTOR:
		{
			ImGui::Text(string(vecParam[i].Desc.begin(), vecParam[i].Desc.end()).c_str());
			
			vector<RoundInfo>& vecRoundInfo = *static_cast<vector<RoundInfo>*>(vecParam[i].Data);

			string AddKey = "Add Round##" + GetUIKey();
			if (ImGui::Button(AddKey.c_str()))
			{
				vecRoundInfo.push_back(RoundInfo{});
			}

			for (size_t j = 0; j < vecRoundInfo.size(); ++j)
			{
				ImGui::PushID(static_cast<int>(j));
				char roundHeader[64];
				sprintf_s(roundHeader, "Round %d", (int)j);
				
				if (ImGui::TreeNode(roundHeader))
				{
					RoundInfo& info = vecRoundInfo[j];

					ImGui::Text("First Spawn Settings");
					for (auto& [type, countPair] : info.EachEnemyFirstSpawnCountMinMax)
					{
						string enemyName;
						switch (type)
						{
						case ENEMY_TYPE::ZOMBIE: enemyName	= "Zombie"; break;
						case ENEMY_TYPE::MUMMY: enemyName	= "Mummy"; break;
						case ENEMY_TYPE::RUNNER: enemyName	= "Runner"; break;
						case ENEMY_TYPE::VAMPIRE: enemyName = "Vampire"; break;
						case ENEMY_TYPE::DEVIL: enemyName	= "Devil"; break;
						default: enemyName					= "Unknown"; break;
						}

						ImGui::PushID((int)type);
						ImGui::Text(enemyName.c_str());
						ImGui::SameLine(100);
						
						int counts[2] = { (int)countPair.first, (int)countPair.second };
						if (ImGui::DragInt2("##counts", counts, 0.1f, 0, 1000))
						{
							countPair.first = (UINT)counts[0];
							countPair.second = (UINT)counts[1];
						}
						ImGui::PopID();
					}

					ImGui::Separator();
					ImGui::Text("Additional Spawn Settings");
					ImGui::DragFloat("Start Time", &info.AdditionalSpawnStartTime, 0.1f, 0.f, 1000.f);

					for (auto& [type, countPair] : info.EachEnemyAdditionalSpawnCountMinMax)
					{
						string enemyName;
						switch (type)
						{
						case ENEMY_TYPE::ZOMBIE: enemyName = "Zombie"; break;
						case ENEMY_TYPE::MUMMY: enemyName = "Mummy"; break;
						case ENEMY_TYPE::RUNNER: enemyName = "Runner"; break;
						case ENEMY_TYPE::VAMPIRE: enemyName = "Vampire"; break;
						case ENEMY_TYPE::DEVIL: enemyName = "Devil"; break;
						default: enemyName = "Unknown"; break;
						}

						ImGui::PushID((int)type + 10); // Offset to avoid ID collision
						ImGui::Text(enemyName.c_str());
						ImGui::SameLine(100);

						int counts[2] = { (int)countPair.first, (int)countPair.second };
						if (ImGui::DragInt2("##counts_add", counts, 0.1f, 0, 1000))
						{
							countPair.first = (UINT)counts[0];
							countPair.second = (UINT)counts[1];
						}
						ImGui::PopID();
					}

					if (ImGui::Button("Delete This Round"))
					{
						vecRoundInfo.erase(vecRoundInfo.begin() + j);
						ImGui::TreePop();
						ImGui::PopID();
						AddItemHeight();
						break; // List modified
					}

					ImGui::TreePop();
				}
				ImGui::PopID();
				AddItemHeight();
			}
		}
			break;
		case SCRIPT_PARAM::GAME_OBJ_REF_HOLDER:
		{
			ImGui::Text(string(vecParam[i].Desc.begin(), vecParam[i].Desc.end()).c_str());
			
			GameObjectRefHolder* ObjRefHolder = static_cast<GameObjectRefHolder*>(vecParam[i].Data);

			ImGui::SameLine(150);
			const wstring ObjectName = ObjRefHolder->GetGameObject() != nullptr ? ObjRefHolder->GetGameObject()->GetName() : L"<None>";
			const string ObjectNameStr = string(ObjectName.begin(), ObjectName.end()) + "##REF_HOLDER";
			
			ImGui::InputText("##REF_HOLDER", string(ObjectName.begin(), ObjectName.end()).data(), ObjectName.length() + 1, ImGuiInputTextFlags_ReadOnly);
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("Outliner"))
				{
					if (!TreeUI::IsPayloadMultiData(Payload))
					{
						DWORD_PTR Data = *static_cast<DWORD_PTR*>(Payload->Data);
						if (Ptr<GameObject> ReceivedObj = reinterpret_cast<GameObject*>(Data))
							ObjRefHolder->SetGameObject(ReceivedObj);
					}
				}
				ImGui::EndDragDropTarget();
			}
		}
			break;
		case SCRIPT_PARAM::VEC_GAME_OBJ_REF_HOLDER:
		{
			ImGui::Text(string(vecParam[i].Desc.begin(), vecParam[i].Desc.end()).c_str());
			// vector<RoundInfo>& vecRoundInfo = *static_cast<vector<RoundInfo>*>(vecParam[i].Data);
			
			vector<GameObjectRefHolder>& vecObjRefHolder = *static_cast<vector<GameObjectRefHolder>*>(vecParam[i].Data);
			
			string AddKey = "Add Element##" + GetUIKey();
			if (ImGui::Button(AddKey.c_str()))
				vecObjRefHolder.push_back(move(GameObjectRefHolder()));

			int TempCount{};
			for (auto it = vecObjRefHolder.begin(); it != vecObjRefHolder.end(); )
			{
				GameObjectRefHolder& ObjRefHolder = *it;
				const wstring ObjectName = ObjRefHolder.GetGameObject() != nullptr ? ObjRefHolder.GetGameObject()->GetName() : L"<None>";
				const string ObjectNameStr = string(ObjectName.begin(), ObjectName.end()) + "##REF_HOLDER";

				const string Tempkey = "##REF_HOLDER" + to_string(TempCount);  
				ImGui::InputText(Tempkey.c_str(), string(ObjectName.begin(), ObjectName.end()).data(), ObjectName.length() + 1, ImGuiInputTextFlags_ReadOnly);
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("Outliner"))
					{
						if (!TreeUI::IsPayloadMultiData(Payload))
						{
							DWORD_PTR Data = *static_cast<DWORD_PTR*>(Payload->Data);
							if (Ptr<GameObject> ReceivedObj = reinterpret_cast<GameObject*>(Data))
								ObjRefHolder.SetGameObject(ReceivedObj);
						}
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::SameLine();
				const string RemoveBtnKey = "Remove##" + to_string(TempCount); 
				if (ImGui::Button(RemoveBtnKey.c_str()))
					it = vecObjRefHolder.erase(it);
				else ++it;
				
				++TempCount;
			}
		}
			break;
		default:
			break;
		}
		ImGui::EndDisabled();
	}
}

void ScriptUI::TickBackgroundTileEditingUI()
{
	if (m_TargetScript->GetScriptType() != SCRIPT_TYPE::BACKGROUNDTILE) return;

	s_BackgroundTileEditingTarget = static_cast<CBackgroundTile*>(m_TargetScript.Get());

	ImGui::SeparatorText("BackgroundTile Brushes");
	AddItemHeight();

	TILE_EDIT_BRUSH currentBrush = s_BackgroundTileBrush;
	if (ImGui::RadioButton("None##123", currentBrush == TILE_EDIT_BRUSH::NONE)) s_BackgroundTileBrush = TILE_EDIT_BRUSH::NONE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Cell: Taken", currentBrush == TILE_EDIT_BRUSH::CELL_TAKEN)) s_BackgroundTileBrush = TILE_EDIT_BRUSH::CELL_TAKEN;
	ImGui::SameLine();
	if (ImGui::RadioButton("Cell: Empty", currentBrush == TILE_EDIT_BRUSH::CELL_EMPTY)) s_BackgroundTileBrush = TILE_EDIT_BRUSH::CELL_EMPTY;
	ImGui::SameLine();
	if (ImGui::RadioButton("Spawn: Add", currentBrush == TILE_EDIT_BRUSH::SPAWN_ADD)) s_BackgroundTileBrush = TILE_EDIT_BRUSH::SPAWN_ADD;
	ImGui::SameLine();
	if (ImGui::RadioButton("Spawn: Remove", currentBrush == TILE_EDIT_BRUSH::SPAWN_REMOVE)) s_BackgroundTileBrush = TILE_EDIT_BRUSH::SPAWN_REMOVE;
	AddItemHeight();

	ImGui::SeparatorText("Spawn Destinations Editing");
	AddItemHeight();

	int currentSpawnLoc = (int)s_BackgroundTileSelectedSpawnLoc;
	if (ImGui::RadioButton("None##456", currentSpawnLoc == FIRST_SPAWN_LOC_END)) s_BackgroundTileSelectedSpawnLoc = FIRST_SPAWN_LOC_END;
	ImGui::SameLine();
	if (ImGui::RadioButton("LOC1", currentSpawnLoc == FIRST_SPAWN_LOC1)) s_BackgroundTileSelectedSpawnLoc = FIRST_SPAWN_LOC1;
	ImGui::SameLine();
	if (ImGui::RadioButton("LOC2", currentSpawnLoc == FIRST_SPAWN_LOC2)) s_BackgroundTileSelectedSpawnLoc = FIRST_SPAWN_LOC2;
	ImGui::SameLine();
	if (ImGui::RadioButton("LOC3", currentSpawnLoc == FIRST_SPAWN_LOC3)) s_BackgroundTileSelectedSpawnLoc = FIRST_SPAWN_LOC3;
	ImGui::SameLine();
	if (ImGui::RadioButton("LOC4", currentSpawnLoc == FIRST_SPAWN_LOC4)) s_BackgroundTileSelectedSpawnLoc = FIRST_SPAWN_LOC4;
	ImGui::SameLine();
	if (ImGui::RadioButton("LOC5", currentSpawnLoc == FIRST_SPAWN_LOC5)) s_BackgroundTileSelectedSpawnLoc = FIRST_SPAWN_LOC5;
	ImGui::SameLine();
	if (ImGui::RadioButton("LOC6", currentSpawnLoc == FIRST_SPAWN_LOC6)) s_BackgroundTileSelectedSpawnLoc = FIRST_SPAWN_LOC6;
	AddItemHeight();

	ImGui::Text("LMB Click / Drag: Paint with selected brush");
	AddItemHeight();
	ImGui::Text("RMB Click: Remove from Spawn Loc (Manual)");
	AddItemHeight();

	bool enabled = s_BackgroundTileCellEditingEnabled;
	if (ImGui::Checkbox("Enable BackgroundTile editing in Main View", &enabled))
		s_BackgroundTileCellEditingEnabled = enabled;
	AddItemHeight();

	ImGui::Text("Spawn Dest Cells are displayed in MAGENTA");
	AddItemHeight();

	ImGui::Separator();
	AddItemHeight();

	ImGui::Text("Grid uses Render_Debug path (F9: debug render on/off)");
	AddItemHeight();
}

void ScriptUI::AddItemHeight()
{
	ImVec2 vSize = ImGui::GetItemRectSize();
	m_ItemHeight += vSize.y + 200.f;
}

void ScriptUI::OnRemoveComponentConfirmed(bool _Confirmed)
{
	if (_Confirmed)
	{
		GetTargetObject()->RemoveScript(m_TargetScript);
		EditorMgr::GetInst()->SetTargetObjectToInspectors(GetTargetObject());
	}
}
