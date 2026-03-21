#include "pch.h"
#include "SpriteUI.h"

#include "GameEngine/07.EditorUI/07.TreeUI/TreeUI.h"

SpriteUI::SpriteUI()
    :AssetUI(ASSET_TYPE::SPRITE)
{
}

SpriteUI::~SpriteUI()
{
}

void SpriteUI::Tick_UI()
{
	AssetUI::Tick_UI();
	
	ImGui::Separator();

	Ptr<ASprite> pSprite = static_cast<ASprite*>(GetTargetAsset().Get());
	
	// Sprite 에 Atlas 텍스쳐가 세팅되어있는지 확인
	Ptr<ATexture> pAtlas = pSprite->GetAtlas();
	const bool IsAtlas = pAtlas.Get();

	ImGui::Text("Atlas Preview");
	
	// Atlas Preview (OutputTitle 바로 아래 상단)
	if (IsAtlas)
	{
		//const float AtlasW = pAtlas->GetWidth();
		//const float AtlasH = pAtlas->GetHeight();
		const float MaxAtlasSize = 260.f;
		const float MaxDim = max(pAtlas->GetWidth(), pAtlas->GetHeight());
		const float AtlasScale = (MaxDim > 0.f && MaxDim > MaxAtlasSize) ? (MaxAtlasSize / MaxDim) : 1.f;

		ImGui::ImageWithBg
		(
			pAtlas->GetSRV().Get(),
			ImVec2(pAtlas->GetWidth() * AtlasScale, pAtlas->GetHeight() * AtlasScale),
			Vec2(0.f, 0.f), Vec2(1.f, 1.f),
			ImVec4(0.0f, 0.0f, 0.0f, 1.0f)
		);
	}
	else ImGui::Text("Atlas Preview : None");
	
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("Content"))
		{
			if (!TreeUI::IsPayloadMultiData(Payload))
			{
				DWORD_PTR data = *static_cast<DWORD_PTR*>(Payload->Data);
				Ptr<Asset> pAsset = reinterpret_cast<Asset*>(data);

				if (pAsset->GetType() == ASSET_TYPE::TEXTURE)
					pSprite->SetAtlas(static_cast<ATexture*>(pAsset.Get()));
			}
		}
		ImGui::EndDragDropTarget();
	}
	
	// Atlas 이름
	string AtlasName = !IsAtlas ? "None" : string(pAtlas->GetKey().begin(), pAtlas->GetKey().end()); 
		
	ImGui::Text("Atlas Key");
	ImGui::SameLine(120);
	ImGui::InputText("##AtlasName", AtlasName.data(), AtlasName.length() + 1, ImGuiInputTextFlags_ReadOnly);

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Separator();
	

	// Sprite UV 정보
	const Vec2 LeftTopUV		= pSprite->GetLeftTopUV();
	const Vec2 SliceUV			= pSprite->GetSliceUV();
	const Vec2 BackgroundUV		= pSprite->GetBackgroundUV();
	const Vec2 OffsetUV			= pSprite->GetOffsetUV();
	
	// Sprite Preview (실제로 샘플링되는 영역)
	if (IsAtlas) DrawSpritePreview(pSprite, m_PreviewZoom, m_MinPreviewZoom, m_MaxPreviewZoom);
	
	ImGui::Text("LeftTop");
	ImGui::BeginDisabled(!IsAtlas);
	{		
		if (IsAtlas)
		{
			int pixel[2] = { static_cast<int>(LeftTopUV.x * pAtlas->GetWidth())
						  ,  static_cast<int>(LeftTopUV.y * pAtlas->GetHeight()) };

			if (ImGui::InputInt2("##LeftTop", pixel))
			{
				pSprite->SetLeftTopUV
				(
					Vec2(pixel[0] / pAtlas->GetWidth(), pixel[1] / pAtlas->GetHeight())
				);
			}
		}
		else
		{
			int pixel[2] = { 0, 0 };
			ImGui::InputInt2("##LeftTop", pixel);
		}		
	}	
	ImGui::EndDisabled();

	ImGui::Text("Slice");	
	ImGui::BeginDisabled(!IsAtlas);
	{		
		if (IsAtlas)
		{
			int pixel[2] = { static_cast<int>(SliceUV.x * pAtlas->GetWidth())
						  ,  static_cast<int>(SliceUV.y * pAtlas->GetHeight()) };

			if (ImGui::InputInt2("##Slice", pixel))
			{
				pSprite->SetSliceUV
				(
					Vec2(pixel[0] / pAtlas->GetWidth(), pixel[1] / pAtlas->GetHeight())
				);
			}
		}
		else
		{
			int pixel[2] = { 0, 0 };
			ImGui::InputInt2("##Slice", pixel);
		}		
	}	
	ImGui::EndDisabled();


	ImGui::Text("Background");	
	ImGui::BeginDisabled(!IsAtlas);
	{		
		if (IsAtlas)
		{
			int pixel[2] = { static_cast<int>(BackgroundUV.x * pAtlas->GetWidth())
						  ,  static_cast<int>(BackgroundUV.y * pAtlas->GetHeight()) };

			if (ImGui::InputInt2("##Background", pixel))
			{
				pSprite->SetBackgroundUV
				(
					Vec2(pixel[0] / pAtlas->GetWidth(), pixel[1] / pAtlas->GetHeight())
				);
			}
		}
		else
		{
			int pixel[2] = { 0, 0 };
			ImGui::InputInt2("##Background", pixel);
		}		
	}	
	ImGui::EndDisabled();

	ImGui::Text("Offset");	
	ImGui::BeginDisabled(!IsAtlas);
	{		
		if (IsAtlas)
		{
			int pixel[2] = { static_cast<int>(OffsetUV.x * pAtlas->GetWidth())
						  ,  static_cast<int>(OffsetUV.y * pAtlas->GetHeight()) };

			if (ImGui::InputInt2("##Offset", pixel))
			{
				pSprite->SetOffsetUV
				(
					Vec2(pixel[0] / pAtlas->GetWidth(), pixel[1] / pAtlas->GetHeight())
				);
			}
		}
		else
		{
			int pixel[2] = { 0, 0 };
			ImGui::InputInt2("##Offset", pixel);
		}		
	}	
	ImGui::EndDisabled();
	
	SaveButton();
}

void SpriteUI::DrawSpritePreview(const Ptr<ASprite>& _Sprite, float& _PreviewZoom, const float& _MinPreviewZoom, const float& _MaxPreviewZoom)
{
	if (!_Sprite) return;
	
	// Sprite UV 정보
	const Vec2 LeftTopUV		= _Sprite->GetLeftTopUV();
	const Vec2 SliceUV			= _Sprite->GetSliceUV();
	const Vec2 BackgroundUV		= _Sprite->GetBackgroundUV();
	const Vec2 OffsetUV			= _Sprite->GetOffsetUV();
	
	ImGui::SetNextItemWidth(80.f);
	ImGui::InputFloat("Zoom##SpritePreview", &_PreviewZoom, 0.1f, 0.5f, "%.2f");
	_PreviewZoom = max(_MinPreviewZoom, min(_PreviewZoom, _MaxPreviewZoom));
	ImGui::SameLine();
	
	if (ImGui::Button("Reset##SpritePreview")) _PreviewZoom = 1.0f;
	
	const float AtlasW = _Sprite->GetAtlas()->GetWidth();
	const float AtlasH = _Sprite->GetAtlas()->GetHeight();

	Vec2 BgUV = BackgroundUV;
	if (BgUV.x <= 0.f || BgUV.y <= 0.f)
		BgUV = SliceUV; // fallback

	// flipbook.fx 와 동일한 샘플링 기준
	Vec2 LeftTop	= LeftTopUV + SliceUV * 0.5f - BgUV * 0.5f;
	Vec2 UV0 		= LeftTop - OffsetUV;
	Vec2 UV1 		= LeftTop + BgUV - OffsetUV;

	const float PreviewW = BgUV.x * AtlasW;
	const float PreviewH = BgUV.y * AtlasH;

	if (PreviewW > 0.f && PreviewH > 0.f)
	{
		const float MaxSpriteSize = 200.f;
		const float MaxDim = (PreviewW > PreviewH)		? PreviewW : PreviewH;
		const float Scale = (MaxDim > MaxSpriteSize)	? (MaxSpriteSize / MaxDim) : 1.f;

		float finalScale = Scale * _PreviewZoom;
		finalScale = max(0.01f, finalScale); // 안전장치
		
		ImGui::Text("Sprite Preview");
		ImGui::ImageWithBg
		(
			_Sprite->GetAtlas()->GetSRV().Get(),
			ImVec2(PreviewW * finalScale, PreviewH * finalScale),
			Vec2(UV0.x, UV0.y),
			Vec2(UV1.x, UV1.y),
			ImVec4(0.0f, 0.0f, 0.0f, 1.0f)
		);

		ImGui::Spacing();
	}
}
