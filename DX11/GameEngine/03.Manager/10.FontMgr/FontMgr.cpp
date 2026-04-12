#include "pch.h"
#include "FontMgr.h"

#include "GameEngine/02.Device/Device.h"

FontMgr::FontMgr()
	: m_FW1Factory(nullptr)
	, m_FontWrapper(nullptr)
{
}

FontMgr::~FontMgr()
{
	if (nullptr != m_FW1Factory)
		m_FW1Factory->Release();

	if (nullptr != m_FontWrapper)
		m_FontWrapper->Release();
}

void FontMgr::Init()
{
	if (FAILED(FW1CreateFactory(FW1_VERSION, &m_FW1Factory)))
	{
		assert(nullptr);
	}

	if (FAILED(m_FW1Factory->CreateFontWrapper(DEVICE, L"Arial", &m_FontWrapper)))
	{
		assert(nullptr);
	}
}

void FontMgr::DrawFont(const wchar_t* _pStr, float _fPosX, float _fPosY, float _fFontSize, UINT _Color)
{
	m_FontWrapper->DrawString(
		CONTEXT,
		_pStr,				// String
		_fFontSize,			// Font size
		_fPosX,				// Window X position
		_fPosY,				// Window Y position
		_Color,				// Text color, 0xAaBbGgRr
		FW1_RESTORESTATE    // Flags (for example FW1_RESTORESTATE to keep context states unchanged)
	);
}

void FontMgr::DrawFont(const wchar_t* _pStr, float _fPosX, float _fPosY, float _fFontSize, const Vec4& _Color)
{
	UINT color = FONT_RGBA((BYTE)(_Color.x * 255.f), (BYTE)(_Color.y * 255.f), (BYTE)(_Color.z * 255.f), (BYTE)(_Color.w * 255.f));
	DrawFont(_pStr, _fPosX, _fPosY, _fFontSize, color);
}

void FontMgr::DrawFont(const wchar_t* _pStr, const wchar_t* _pFontName, float _fPosX, float _fPosY, float _fFontSize, UINT _Color)
{
	m_FontWrapper->DrawString(
		CONTEXT,
		_pStr,				// String
		_pFontName,			// Font family
		_fFontSize,			// Font size
		_fPosX,				// Window X position
		_fPosY,				// Window Y position
		_Color,				// Text color, 0xAaBbGgRr
		FW1_RESTORESTATE    // Flags
	);
}

void FontMgr::DrawFont(const wchar_t* _pStr, const wchar_t* _pFontName, float _fPosX, float _fPosY, float _fFontSize, const Vec4& _Color)
{
	DrawFont
	(
		_pStr,
		_pFontName,
		_fPosX, _fPosY,
		_fFontSize,
		FONT_RGBA((BYTE)(_Color.x * 255.f), (BYTE)(_Color.y * 255.f), (BYTE)(_Color.z * 255.f), (BYTE)(_Color.w * 255.f))
	);
}
