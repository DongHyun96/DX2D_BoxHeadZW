#include "pch.h"
#include "PathMgr.h"

vector<wstring> g_vecName;
vector<wstring> g_vecIncludePath;
vector<bool>	g_vecIsAbstract;

string RemoveComments(const string& _src)
{
	string strResult = {};
	strResult.reserve(_src.size());

	bool bLineComment = false;
	bool bBlockComment = false;

	for (size_t i = 0; i < _src.size(); ++i)
	{
		const char ch = _src[i];
		const char chNext = (i + 1 < _src.size()) ? _src[i + 1] : '\0';

		if (bLineComment)
		{
			if (ch == '\n')
			{
				bLineComment = false;
				strResult.push_back(ch);
			}
			continue;
		}

		if (bBlockComment)
		{
			if (ch == '*' && chNext == '/')
			{
				bBlockComment = false;
				++i;
			}
			continue;
		}

		if (ch == '/' && chNext == '/')
		{
			bLineComment = true;
			++i;
			continue;
		}

		if (ch == '/' && chNext == '*')
		{
			bBlockComment = true;
			++i;
			continue;
		}

		strResult.push_back(ch);
	}

	return strResult;
}

string CompressWhitespace(const string& _src)
{
	string strResult = {};
	strResult.reserve(_src.size());

	bool bPrevSpace = false;
	for (char ch : _src)
	{
		const bool bSpace = (ch == ' '
			|| ch == '\t'
			|| ch == '\n'
			|| ch == '\r'
			|| ch == '\f'
			|| ch == '\v');

		if (bSpace)
		{
			if (!bPrevSpace)
			{
				strResult.push_back(' ');
				bPrevSpace = true;
			}
		}
		else
		{
			strResult.push_back(ch);
			bPrevSpace = false;
		}
	}

	return strResult;
}

bool IsAbstractScriptHeader(const wstring& _strHeaderPath)
{
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _strHeaderPath.c_str(), L"rb");
	if (nullptr == pFile)
		return false;

	if (0 != fseek(pFile, 0, SEEK_END))
	{
		fclose(pFile);
		return false;
	}

	const long iFileSize = ftell(pFile);
	if (iFileSize <= 0)
	{
		fclose(pFile);
		return false;
	}

	rewind(pFile);

	string strContent = {};
	strContent.resize(static_cast<size_t>(iFileSize));

	const size_t iReadSize = fread(&strContent[0], 1, strContent.size(), pFile);
	fclose(pFile);

	strContent.resize(iReadSize);
	strContent = RemoveComments(strContent);
	strContent = CompressWhitespace(strContent);

	// CScript 상속 여부 확인
	if (string::npos == strContent.find("public CScript")) return false;

	// 순수 가상 함수(Pure Virtual Function) 존재 여부 확인
	// 정규표현식 패턴: 'virtual' 키워드 + (; 또는 { 가 아닌 문자들) + '= 0;' (공백은 유연하게 허용)
	try 
	{
		std::regex pureVirtualRegex(R"(virtual[^;{]+=\s*0\s*;)");
        
		// 정규식과 매칭되는 패턴이 하나라도 있으면 추상 클래스로 판별
		if (std::regex_search(strContent, pureVirtualRegex))
		{
			return true;
		}
	}
	catch (const std::regex_error& /*e*/) 
	{
		// 정규식 파싱 중 오류 발생 시 예외 처리 (안전망)
		return false;
	}

	return false;
}

void ScanScripts(const wstring& _strRootPath, const wstring& _strCurrentPath, const vector<wstring>& _vecExcept)
{
	WIN32_FIND_DATA tData = {};
	HANDLE handle = FindFirstFile((_strCurrentPath + L"\\*").c_str(), &tData);

	if (INVALID_HANDLE_VALUE == handle)
		return;

	do
	{
		if (tData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (wcscmp(tData.cFileName, L".") && wcscmp(tData.cFileName, L".."))
			{
				ScanScripts(_strRootPath, _strCurrentPath + L"\\" + tData.cFileName, _vecExcept);
			}
		}
		else
		{
			wstring strFileName = tData.cFileName;
			if (strFileName.size() > 2 && strFileName.substr(strFileName.size() - 2) == L".h")
			{
				// 접두사 C 또는 c로 시작하는 파일만 스크립트 파일로 간주한다
				if (strFileName[0] != L'C' && strFileName[0] != 'c')
					continue;

				bool bException = false;
				for (size_t i = 0; i < _vecExcept.size(); ++i)
				{
					if (!wcscmp(tData.cFileName, _vecExcept[i].c_str()))
					{
						bException = true;
						break;
					}
				}

				if (!bException)
				{
					wstring strPureName = strFileName.substr(0, strFileName.size() - 2);
					g_vecName.push_back(strPureName);

					wstring strFullPath = _strCurrentPath + L"\\" + strFileName;
					wstring strRelPath = strFullPath.substr(_strRootPath.length());

					if (!strRelPath.empty() && (strRelPath[0] == L'\\' || strRelPath[0] == L'/'))
						strRelPath = strRelPath.substr(1);

					g_vecIncludePath.push_back(strRelPath.substr(0, strRelPath.size() - 2));
					g_vecIsAbstract.push_back(IsAbstractScriptHeader(strFullPath));
				}
			}
		}
	} while (FindNextFile(handle, &tData));

	FindClose(handle);
}

int main()
{
	CPathMgr::init();
	wstring strProjPath = CPathMgr::GetProjectPath();
	wstring strCppPath = strProjPath + L"DX11\\Source\\ScriptMgr.cpp";
	wstring strHeaderPath = strProjPath + L"DX11\\Source\\ScriptMgr.h";
	wstring strScriptCode = strProjPath + L"DX11\\Source\\Scripts";

	FILE* pExceptList = nullptr;
	_wfopen_s(&pExceptList, L"exeptlist.txt", L"r");

	vector<wstring> vecExcept = {};
	if (nullptr != pExceptList)
	{
		wchar_t szName[255] = L"";
		while (fwscanf_s(pExceptList, L"%s", szName, 255) != -1)
		{
			vecExcept.push_back(szName);
		}
		fclose(pExceptList);
	}

	ScanScripts(strScriptCode, strScriptCode, vecExcept);

	FILE* pFile = nullptr;

	_wfopen_s(&pFile, strHeaderPath.c_str(), L"w");
	fwprintf_s(pFile, L"#pragma once\n\n#include <vector>\n#include <string>\n\nenum SCRIPT_TYPE\n{\n");

	for (UINT i = 0; i < g_vecName.size(); ++i)
	{
		wstring strScriptUpperName = L"";
		for (UINT j = 1; j < g_vecName[i].size(); ++j)
		{
			strScriptUpperName += toupper(g_vecName[i][j]);
		}

		fwprintf_s(pFile, L"\t%s,\n", strScriptUpperName.c_str());
	}
	fwprintf_s(pFile, L"\tEND,\n};\n\nusing namespace std;\nclass CScript;\n\nclass ScriptMgr\n{\npublic:\n");
	fwprintf_s(pFile, L"\tstatic void GetScriptInfo(vector<wstring>& _vec);\n\tstatic CScript * GetScript(const wstring& _strScriptName);\n");
	fwprintf_s(pFile, L"\tstatic CScript * GetScript(UINT _iScriptType);\n\tstatic const wchar_t * GetScriptName(CScript * _pScript);\n};\n");

	fclose(pFile);

	_wfopen_s(&pFile, strCppPath.c_str(), L"w");
	fwprintf_s(pFile, L"#include \"pch.h\"\n#include \"ScriptMgr.h\"\n\n");

	for (UINT i = 0; i < g_vecIncludePath.size(); ++i)
	{
		fwprintf_s(pFile, L"#include \"Source/Scripts/%s.h\"\n", g_vecIncludePath[i].c_str());
	}

	fwprintf_s(pFile, L"\nvoid ScriptMgr::GetScriptInfo(vector<wstring>& _vec)\n{\n");
	for (UINT i = 0; i < g_vecName.size(); ++i)
	{
		if (g_vecIsAbstract[i])
			continue;

		fwprintf_s(pFile, L"\t_vec.push_back(L\"%s\");\n", g_vecName[i].c_str());
	}
	fwprintf_s(pFile, L"}\n\n");

	fwprintf_s(pFile, L"CScript * ScriptMgr::GetScript(const wstring& _strScriptName)\n{\n");
	for (UINT i = 0; i < g_vecName.size(); ++i)
	{
		fwprintf_s(pFile, L"\tif (L\"%s\" == _strScriptName)\n", g_vecName[i].c_str());

		if (g_vecIsAbstract[i])
			fwprintf_s(pFile, L"\t\treturn nullptr;\n");
		else
			fwprintf_s(pFile, L"\t\treturn new %s;\n", g_vecName[i].c_str());
	}
	fwprintf_s(pFile, L"\treturn nullptr;\n}\n\n");

	fwprintf_s(pFile, L"CScript * ScriptMgr::GetScript(UINT _iScriptType)\n{\n\tswitch (_iScriptType)\n\t{\n");
	for (UINT i = 0; i < g_vecName.size(); ++i)
	{
		wstring strScriptUpperName = L"";
		for (UINT j = 1; j < g_vecName[i].size(); ++j)
			strScriptUpperName += toupper(g_vecName[i][j]);

		if (g_vecIsAbstract[i])
			fwprintf_s(pFile, L"\tcase (UINT)SCRIPT_TYPE::%s:\n\t\treturn nullptr;\n", strScriptUpperName.c_str());
		else
			fwprintf_s(pFile, L"\tcase (UINT)SCRIPT_TYPE::%s:\n\t\treturn new %s;\n", strScriptUpperName.c_str(), g_vecName[i].c_str());
	}
	fwprintf_s(pFile, L"\t}\n\treturn nullptr;\n}\n\n");

	fwprintf_s(pFile, L"const wchar_t * ScriptMgr::GetScriptName(CScript * _pScript)\n{\n\tswitch ((SCRIPT_TYPE)_pScript->GetScriptType())\n\t{\n");
	for (UINT i = 0; i < g_vecName.size(); ++i)
	{
		wstring strScriptUpperName = L"";
		for (UINT j = 1; j < g_vecName[i].size(); ++j)
			strScriptUpperName += toupper(g_vecName[i][j]);

		fwprintf_s(pFile, L"\tcase SCRIPT_TYPE::%s:\n\t\treturn L\"%s\";\n", strScriptUpperName.c_str(), g_vecName[i].c_str());
	}
	fwprintf_s(pFile, L"\t}\n\treturn nullptr;\n}");

	fclose(pFile);

	return 0;
}
