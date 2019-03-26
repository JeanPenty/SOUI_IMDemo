#ifndef __IWORDSIZE_ADAPTER_CALLBACK_H__
#define __IWORDSIZE_ADAPTER_CALLBACK_H__

#pragma once

struct IWordSizeAdapterCallback
{
	virtual void OnClickItem(int nIndex) = 0;
};

#endif