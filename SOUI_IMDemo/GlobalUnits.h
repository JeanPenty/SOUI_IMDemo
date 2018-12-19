#ifndef __GLOBALUNITS_H__
#define __GLOBALUNITS_H__

#pragma once

#include "public.h"

class CGlobalUnits
{
public:
	CGlobalUnits(void);
	~CGlobalUnits(void);

public:
	static CGlobalUnits* GetInstance();
};

#define GlobalUnits  CGlobalUnits::GetInstance();

#endif