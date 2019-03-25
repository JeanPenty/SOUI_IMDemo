#ifndef __GLOBALUNITS_H__
#define __GLOBALUNITS_H__

#pragma once

#include "public.h"
#include "CommonFunction.h"

class CGlobalUnits
{
public:
	CGlobalUnits(void);
	~CGlobalUnits(void);

public:
	PersonalsMap	m_mapPersonals;
	GroupsMap		m_mapGroups;
	GroupMembersMap	m_mapGrpmbrs;
	GHsMap			m_mapGhs;

	std::multimap<std::wstring, std::wstring>	m_PinyinMap;
	SearchInfosMap	m_mapPinyinSearch;
	SearchInfosMap	m_mapChineseSearch;
public:
	static CGlobalUnits* GetInstance();

public:
	void OperateShamDate();		//处理假数据
	void OperatePinyinMap(const std::string& strPath);	//处理拼音表
	void OperateSerachIndex();

	std::wstring EncodeChinese(const std::wstring& wstrSrc);
	std::wstring DecodeChinese(const std::wstring& wstrSrc);
	BOOL GetSimpleAndFull(const std::wstring& wstrSrc, std::wstring &SimpleStr, std::wstring &FullStr);
	BOOL IsIncludeChinese(std::wstring& wstrSrc);
public:
	std::string GenerateUUID();
};

#define GlobalUnits  CGlobalUnits::GetInstance();

#endif