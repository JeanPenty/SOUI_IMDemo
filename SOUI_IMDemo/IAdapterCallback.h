#ifndef __IADAPTERCALLBACK_H__
#define __IADAPTERCALLBACK_H__

#pragma once

struct IAdapterLasttalk_Callback 
{
	virtual void OnLasttalkListItemClick(int nType, const std::string& strID) = 0;
	virtual void OnLasttalkListItemRClick(int nType, const std::string& strID) = 0;
};

#endif