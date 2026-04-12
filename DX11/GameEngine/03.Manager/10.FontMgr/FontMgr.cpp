#include "pch.h"
#include "FontMgr.h"
#include "GameEngine/03.Manager/01.PathMgr/PathMgr.h"
#include "GameEngine/02.Device/Device.h"

#include <filesystem>
#pragma comment(lib, "dwrite.lib")

namespace fs = std::filesystem;

// ... (기존에 작성하신 SwapUint16, GetNameFromTable, GetFontNameFromDWrite, GetFontNameFromGDI 함수들은 변경 없이 그대로 유지합니다.) ...

static wstring GetNameFromDWriteStrings(IDWriteLocalizedStrings* pNames)
{
    if (!pNames) return L"";
    
    UINT32 index = 0;
    BOOL exists = FALSE;
    
    pNames->FindLocaleName(L"en-us", &index, &exists);
    if (!exists) pNames->FindLocaleName(L"ko-kr", &index, &exists);
    if (!exists) index = 0; // 없으면 무조건 0번 인덱스

    UINT32 length = 0;
    pNames->GetStringLength(index, &length);
    std::vector<wchar_t> buffer(length + 1);
    pNames->GetString(index, buffer.data(), length + 1);
    
    return wstring(buffer.data());
}

// DirectWrite를 사용하여 폰트 파일로부터 정확한 Family Name을 추출합니다.
static wstring GetFontNameFromDWrite(const wstring& _filePath)
{
    IDWriteFactory* pDWriteFactory = nullptr;
    HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&pDWriteFactory));
    if (FAILED(hr)) return L"";

    IDWriteFontFile* pFontFile = nullptr;
    hr = pDWriteFactory->CreateFontFileReference(_filePath.c_str(), nullptr, &pFontFile);
    if (FAILED(hr))
    {
       pDWriteFactory->Release();
       return L"";
    }

    // -------------------------------------------------------------------
    // [수정 1] Analyze()를 통해 폰트 파일의 정확한 Face Type을 알아냅니다.
    // DWRITE_FONT_FACE_TYPE_UNKNOWN으로 인한 생성 실패를 방지합니다.
    // -------------------------------------------------------------------
    BOOL isSupported = FALSE;
    DWRITE_FONT_FILE_TYPE fileType;
    DWRITE_FONT_FACE_TYPE faceType;
    UINT32 numberOfFaces = 0;
    
    hr = pFontFile->Analyze(&isSupported, &fileType, &faceType, &numberOfFaces);
    if (FAILED(hr) || !isSupported)
    {
        pFontFile->Release();
        pDWriteFactory->Release();
        return L""; // 지원하지 않는 포맷이거나 손상된 폰트 파일
    }

    IDWriteFontFace* pFontFace = nullptr;
    // 알아낸 faceType을 넘겨줍니다.
    hr = pDWriteFactory->CreateFontFace(faceType, 1, &pFontFile, 0, DWRITE_FONT_SIMULATIONS_NONE, &pFontFace);
    if (FAILED(hr))
    {
       pFontFile->Release();
       pDWriteFactory->Release();
       return L"";
    }

    wstring resultName = L"";

    IDWriteFontFace3* pFontFace3 = nullptr;
    hr = pFontFace->QueryInterface(__uuidof(IDWriteFontFace3), reinterpret_cast<void**>(&pFontFace3));

    if (SUCCEEDED(hr))
    {
        IDWriteLocalizedStrings* pFamilyNames = nullptr;
        BOOL exists = FALSE;

        // 1. Typographic Family Names 먼저 시도
        hr = pFontFace3->GetInformationalStrings(DWRITE_INFORMATIONAL_STRING_TYPOGRAPHIC_FAMILY_NAMES, &pFamilyNames, &exists);
        
        // 2. 실패하거나 없으면 표준 Win32 Family Names 시도
        if (FAILED(hr) || !exists)
        {
           hr = pFontFace3->GetInformationalStrings(DWRITE_INFORMATIONAL_STRING_WIN32_FAMILY_NAMES, &pFamilyNames, &exists);
        }

        // -------------------------------------------------------------------
        // [수정 2] 이름 개수(GetCount)를 확인하고 안전하게 첫 번째 이름을 가져옵니다.
        // -------------------------------------------------------------------
        if (SUCCEEDED(hr) && exists && pFamilyNames->GetCount() > 0)
        {
           UINT32 index = 0;
           BOOL localeExists = FALSE;
           
           // 영문(en-us) 이름 최우선 검색
           hr = pFamilyNames->FindLocaleName(L"en-us", &index, &localeExists);
           
           if (FAILED(hr) || !localeExists)
              hr = pFamilyNames->FindLocaleName(L"ko-kr", &index, &localeExists);
              
           // [핵심] en-us, ko-kr 둘 다 없어도 무조건 첫 번째(0번) 언어의 이름을 사용!
           if (!localeExists)
              index = 0;

           UINT32 length = 0;
           hr = pFamilyNames->GetStringLength(index, &length);
           if (SUCCEEDED(hr))
           {
              // new/delete 대신 std::vector를 사용하여 메모리 누수 원천 차단
              std::vector<wchar_t> nameBuffer(length + 1);
              hr = pFamilyNames->GetString(index, nameBuffer.data(), length + 1);
              if (SUCCEEDED(hr))
              {
                  resultName = nameBuffer.data();
              }
           }
           pFamilyNames->Release();
        }
        pFontFace3->Release();
    }

    pFontFace->Release();
    pFontFile->Release();
    pDWriteFactory->Release();

    return resultName;
}

// GDI를 사용하여 폰트 파일로부터 Face Name(Family Name)을 추출합니다. (백업용)
static wstring GetFontNameFromGDI(const wstring& _filePath)
{
    // 문서화되지 않은 API의 함수 포인터 타입 정의
    typedef BOOL(WINAPI* GETFONTRESOURCEINFOW)(LPCWSTR, LPDWORD, LPVOID, DWORD);
    
    static GETFONTRESOURCEINFOW pGetFontResourceInfoW = nullptr;
    static bool bTriedLoad = false;

    // 프로그램 실행 중 딱 한 번만 gdi32.dll에서 함수 주소를 가져옵니다.
    if (!bTriedLoad)
    {
        HMODULE hGdi32 = GetModuleHandleW(L"gdi32.dll");
        if (!hGdi32) 
        {
            hGdi32 = LoadLibraryW(L"gdi32.dll");
        }
        
        if (hGdi32)
        {
            pGetFontResourceInfoW = (GETFONTRESOURCEINFOW)GetProcAddress(hGdi32, "GetFontResourceInfoW");
        }
        bTriedLoad = true;
    }

    // 함수를 불러오지 못했다면 포기 (빨간 줄이나 크래시 방지)
    if (!pGetFontResourceInfoW)
    {
        return L"";
    }

    wchar_t szFontName[LF_FACESIZE] = {};
    DWORD dwSize = sizeof(szFontName);

    // 2: GFR_FACENAME - 폰트의 실제 이름을 가져옵니다.
    if (pGetFontResourceInfoW(_filePath.c_str(), &dwSize, szFontName, 2))
    {
        return szFontName;
    }

    return L"";
}

FontMgr::FontMgr()
    : m_FW1Factory(nullptr)
    , m_FontWrapper(nullptr)
    , m_pDWriteFactory5(nullptr)
    , m_pCustomCollection(nullptr)
{
}

FontMgr::~FontMgr()
{
    for (const auto& path : m_vecFontPath)
    {
       RemoveFontResourceEx(path.c_str(), FR_PRIVATE, 0);
    }

    // 캐싱된 텍스트 포맷들 해제
    for (auto& pair : m_mapTextFormat)
    {
        if (pair.second) pair.second->Release();
    }
    m_mapTextFormat.clear();

    // DWrite 리소스 해제
    if (m_pCustomCollection) m_pCustomCollection->Release();
    if (m_pDWriteFactory5) m_pDWriteFactory5->Release();

    if (m_FontWrapper) m_FontWrapper->Release();
    if (m_FW1Factory) m_FW1Factory->Release();
}

void FontMgr::Init()
{
    // -------------------------------------------------------------
    // 1. DirectWrite Factory 5 생성 (최신 DWrite 기능 사용)
    // -------------------------------------------------------------
    HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory5), reinterpret_cast<IUnknown**>(&m_pDWriteFactory5));
    if (FAILED(hr)) assert(false);

    IDWriteFontSetBuilder1* pFontSetBuilder = nullptr;
    m_pDWriteFactory5->CreateFontSetBuilder(&pFontSetBuilder);

    wstring strFontFolderPath = CONTENT_PATH + L"Font\\";

    if (fs::exists(strFontFolderPath) && fs::is_directory(strFontFolderPath))
    {
       for (const auto& entry : fs::recursive_directory_iterator(strFontFolderPath))
       {
          if (entry.is_regular_file())
          {
             wstring ext = entry.path().extension().wstring();
             for (auto& c : ext) c = towlower(c);

             if (ext == L".ttf" || ext == L".otf")
             {
                wstring filePath = entry.path().wstring();
                wstring relativePath = filePath.substr(CONTENT_PATH.length());

                // [추가됨] DirectWrite Custom Collection에 폰트 파일 추가
                IDWriteFontFile* pFontFile = nullptr;
                hr = m_pDWriteFactory5->CreateFontFileReference(filePath.c_str(), nullptr, &pFontFile);
                if (SUCCEEDED(hr) && pFontFile)
                {
                    pFontSetBuilder->AddFontFile(pFontFile);
                    pFontFile->Release();
                }

                // 기존 로직 유지
                wstring strFamilyName = GetFontNameFromDWrite(filePath);
                if (strFamilyName.empty())
                {
                   strFamilyName = GetFontNameFromGDI(filePath);
                }

                if (!strFamilyName.empty())
                {
                   LoadFont(relativePath, strFamilyName);
                }
                else
                {
                   LoadFont(relativePath, entry.path().stem().wstring());
                }
             }
          }
       }
    }
    
    // -------------------------------------------------------------
    // 2. Custom Font Collection 생성
    // -------------------------------------------------------------
    IDWriteFontSet* pFontSet = nullptr;
    if (SUCCEEDED(pFontSetBuilder->CreateFontSet(&pFontSet)))
    {
        m_pDWriteFactory5->CreateFontCollectionFromFontSet(pFontSet, &m_pCustomCollection);
        pFontSet->Release();
    }
    pFontSetBuilder->Release();

    // -------------------------------------------------------------
    // [핵심 변경] 완성된 컬렉션에서 Family Name과 Face Name(Bold 등)을 추출하여 맵핑
    // -------------------------------------------------------------
    if (m_pCustomCollection)
    {
        m_vecFontName.clear(); // 안전을 위해 초기화

        UINT32 familyCount = m_pCustomCollection->GetFontFamilyCount();
        for (UINT32 i = 0; i < familyCount; ++i)
        {
            IDWriteFontFamily* pFamily = nullptr;
            if (SUCCEEDED(m_pCustomCollection->GetFontFamily(i, &pFamily)))
            {
                // 1. 패밀리 이름 가져오기 (예: "Pretendard")
                IDWriteLocalizedStrings* pFamilyNames = nullptr;
                pFamily->GetFamilyNames(&pFamilyNames);
                wstring familyName = GetNameFromDWriteStrings(pFamilyNames);
                pFamilyNames->Release();

                // 2. 해당 패밀리에 속한 폰트 스타일(Face) 순회
                UINT32 fontCount = pFamily->GetFontCount();
                for (UINT32 j = 0; j < fontCount; ++j)
                {
                    IDWriteFont* pFont = nullptr;
                    if (SUCCEEDED(pFamily->GetFont(j, &pFont)))
                    {
                        // 스타일 이름 가져오기 (예: "Bold", "Thin")
                        IDWriteLocalizedStrings* pFaceNames = nullptr;
                        pFont->GetFaceNames(&pFaceNames);
                        wstring faceName = GetNameFromDWriteStrings(pFaceNames);
                        pFaceNames->Release();

                        // UI에 표시할 Full Name 만들기 (예: "Pretendard Bold")
                        wstring fullName = familyName;
                        if (faceName != L"Regular" && faceName != L"Normal") 
                        {
                            fullName += L" " + faceName;
                        }

                        // 구조체에 저장
                        CustomFontInfo info;
                        info.FamilyName = familyName;
                        info.Weight = pFont->GetWeight();
                        info.Style = pFont->GetStyle();

                        // 맵에 등록 및 UI 리스트에 추가 (중복 방지)
                        if (m_mapFontInfo.find(fullName) == m_mapFontInfo.end())
                        {
                            m_mapFontInfo[fullName] = info;
                            m_vecFontName.push_back(fullName);
                        }

                        pFont->Release();
                    }
                }
                pFamily->Release();
            }
        }
    }

    // -------------------------------------------------------------
    // 3. FW1(DirectWrite) 팩토리와 래퍼를 생성
    // -------------------------------------------------------------
    if (FAILED(FW1CreateFactory(FW1_VERSION, &m_FW1Factory)))
    {
       assert(nullptr);
    }

    if (FAILED(m_FW1Factory->CreateFontWrapper(DEVICE, L"Arial", &m_FontWrapper)))
    {
       assert(nullptr);
    }
}

void FontMgr::LoadFont(const wstring& _FontPath, const wstring& _FontName)
{
    // (이전 코드와 동일하므로 생략하지 않고 그대로 사용)
    wstring strFontPath = CONTENT_PATH;
    strFontPath += _FontPath;

    for (const auto& path : m_vecFontPath)
    {
       if (path == strFontPath) return;
    }

    int iResult = AddFontResourceEx(strFontPath.c_str(), FR_PRIVATE, 0);
    if (0 == iResult) assert(nullptr);

    m_vecFontPath.push_back(strFontPath);
    m_vecFontName.push_back(_FontName);

    DebugUtil::AddDebugLog(L"[FontMgr] Loaded Font: " + _FontName + L" (" + _FontPath + L")");
}

// 기본 폰트 이름 없이 그리는 함수 (기존 로직 유지)
void FontMgr::DrawFont(const wchar_t* _pStr, float _fPosX, float _fPosY, float _fFontSize, UINT _Color)
{
    m_FontWrapper->DrawString(CONTEXT, _pStr, _fFontSize, _fPosX, _fPosY, _Color, FW1_RESTORESTATE);
}

void FontMgr::DrawFont(const wchar_t* _pStr, float _fPosX, float _fPosY, float _fFontSize, const Vec4& _Color)
{
    UINT color = FONT_RGBA((BYTE)(_Color.x * 255.f), (BYTE)(_Color.y * 255.f), (BYTE)(_Color.z * 255.f), (BYTE)(_Color.w * 255.f));
    DrawFont(_pStr, _fPosX, _fPosY, _fFontSize, color);
}

// ---------------------------------------------------------------------------------
// [핵심 변경] 커스텀 컬렉션과 스타일을 적용한 DrawFont
// ---------------------------------------------------------------------------------
void FontMgr::DrawFont(const wchar_t* _pStr, const wchar_t* _pFontName, float _fPosX, float _fPosY, float _fFontSize, UINT _Color, DWRITE_FONT_WEIGHT _Weight, DWRITE_FONT_STYLE _Style)
{
    if (!m_pDWriteFactory5 || !m_pCustomCollection) return;

    wstring keyName = _pFontName; // CText가 넘겨준 "Pretendard Bold"
    wstring targetFamily = _pFontName;
    DWRITE_FONT_WEIGHT targetWeight = DWRITE_FONT_WEIGHT_NORMAL;
    DWRITE_FONT_STYLE targetStyle = DWRITE_FONT_STYLE_NORMAL;

    // 맵에서 매칭되는 정보가 있다면 추출
    auto iter = m_mapFontInfo.find(keyName);
    if (iter != m_mapFontInfo.end())
    {
        targetFamily = iter->second.FamilyName; // 실제 DWrite 생성 시에는 Family Name 필요
        targetWeight = iter->second.Weight;
        targetStyle = iter->second.Style;
    }

    // -----------------------------------------------------------
    // 이후는 이전 답변에서 작성했던 '텍스트 포맷 캐싱 및 레이아웃' 로직과 완벽히 동일합니다.
    // -----------------------------------------------------------
    wchar_t szFormatKey[256];
    swprintf_s(szFormatKey, L"%s_%.1f_%d_%d", targetFamily.c_str(), _fFontSize, targetWeight, targetStyle);
    wstring formatKey = szFormatKey;

    IDWriteTextFormat* pTextFormat = nullptr;
    auto iterFormat = m_mapTextFormat.find(formatKey);
    
    if (iterFormat != m_mapTextFormat.end())
    {
        pTextFormat = iterFormat->second;
    }
    else
    {
        // 이때 패밀리 이름과 정확한 굵기/스타일을 적용
        HRESULT hr = m_pDWriteFactory5->CreateTextFormat(
            targetFamily.c_str(), 
            m_pCustomCollection,     
            targetWeight,
            targetStyle,
            DWRITE_FONT_STRETCH_NORMAL,
            _fFontSize,
            L"ko-kr",
            &pTextFormat
        );

        if (FAILED(hr)) return;
        m_mapTextFormat[formatKey] = pTextFormat;
    }

    // 레이아웃 렌더링
    IDWriteTextLayout* pTextLayout = nullptr;
    UINT32 textLen = (UINT32)wcslen(_pStr);
    
    if (SUCCEEDED(m_pDWriteFactory5->CreateTextLayout(_pStr, textLen, pTextFormat, 4096.0f, 4096.0f, &pTextLayout)))
    {
        m_FontWrapper->DrawTextLayout(CONTEXT, pTextLayout, _fPosX, _fPosY, _Color, FW1_RESTORESTATE);
        pTextLayout->Release(); 
    }
}

void FontMgr::DrawFont(const wchar_t* _pStr, const wchar_t* _pFontName, float _fPosX, float _fPosY, float _fFontSize, const Vec4& _Color, DWRITE_FONT_WEIGHT _Weight, DWRITE_FONT_STYLE _Style)
{
    DrawFont
    (
       _pStr, _pFontName, _fPosX, _fPosY, _fFontSize,
       FONT_RGBA((BYTE)(_Color.x * 255.f), (BYTE)(_Color.y * 255.f), (BYTE)(_Color.z * 255.f), (BYTE)(_Color.w * 255.f)),
       _Weight, _Style
    );
}