#include "StdAfx.h"
#include "GlobalUnits.h"

CGlobalUnits::CGlobalUnits(void)
{
}

CGlobalUnits::~CGlobalUnits(void)
{
}


CGlobalUnits* CGlobalUnits::GetInstance()
{
	static CGlobalUnits _Instance;
	return &_Instance;
}