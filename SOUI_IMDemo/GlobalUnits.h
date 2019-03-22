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

public:
	static CGlobalUnits* GetInstance();

public:
	void OperateShamDate();		//处理假数据

public:
	std::string GenerateUUID();
};

#define GlobalUnits  CGlobalUnits::GetInstance();

#endif