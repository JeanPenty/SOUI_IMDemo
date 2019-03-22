#ifndef __COMMONFUNCTION_H__
#define __COMMONFUNCTION_H__

#pragma once
#include "stdafx.h"
#include <Rpc.h>

void ConvertUtf16ToUtf8(const std::wstring& strUtf16, std::string& strUtf8)
{
	int len = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)strUtf16.c_str(), -1, NULL, 0, NULL, NULL); 
	char *pszUtf8=new char[len + 1];
	memset(pszUtf8, 0, len + 1);
	WideCharToMultiByte (CP_UTF8, 0, (LPCWSTR)strUtf16.c_str(), -1, pszUtf8, len, NULL,NULL);

	strUtf8 = pszUtf8;
	delete[] pszUtf8;
}

void ConvertUtf8ToUtf16(const std::string& strUtf8, std::wstring& strUtf16)
{
	int len=MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)strUtf8.c_str(), -1, NULL,0);
	wchar_t * pUtf16 = new wchar_t[len+1];
	memset(pUtf16, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)strUtf8.c_str(), -1, (LPWSTR)pUtf16, len);

	strUtf16 = pUtf16;
	delete[] pUtf16;
}

void ConvertGBKToUtf8(std::string& strGBKUtf8)
{
	int len=MultiByteToWideChar(CP_ACP, 0, (LPCSTR)strGBKUtf8.c_str(), -1, NULL,0);
	wchar_t * pUtf16 = new wchar_t[len+1];
	memset(pUtf16, 0, len * 2 + 2);
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)strGBKUtf8.c_str(), -1, (LPWSTR)pUtf16, len);

	len = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)pUtf16, -1, NULL, 0, NULL, NULL); 
	char *pUtf8=new char[len + 1];
	memset(pUtf8, 0, len + 1);
	WideCharToMultiByte (CP_UTF8, 0, (LPCWSTR)pUtf16, -1, pUtf8, len, NULL,NULL);

	strGBKUtf8 = pUtf8;
	delete[] pUtf8;
	delete[] pUtf16;
}

void ConvertUtf8ToGBK(std::string& strUtf8GBK)
{
	int len=MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)strUtf8GBK.c_str(), -1, NULL,0);
	wchar_t * pUtf16 = new wchar_t[len+1];
	memset(pUtf16, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)strUtf8GBK.c_str(), -1, (LPWSTR)pUtf16, len);

	len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)pUtf16, -1, NULL, 0, NULL, NULL); 
	char *pGBK=new char[len + 1];
	memset(pGBK, 0, len + 1);
	WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)pUtf16, -1, pGBK, len, NULL,NULL);

	strUtf8GBK = pGBK;
	delete[] pGBK;
	delete[] pUtf16;
}
#endif