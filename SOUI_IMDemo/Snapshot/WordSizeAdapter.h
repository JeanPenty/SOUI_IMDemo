#ifndef __WORDSIZE_ADAPTER_H__
#define __WORDSIZE_ADAPTER_H__

#pragma once

#include <helper/SAdapterBase.h>
#include <string>
using namespace std;

#include "IWordSizeAdapterCallback.h"

class CWordSizeAdapter : public SAdapterBase
{
public:
	struct wordSize
	{
		std::wstring m_wstrWordSize;
	};
	

private:
	SArray<wordSize> m_arrWordSize;
	IWordSizeAdapterCallback* m_pCB;

public:
	CWordSizeAdapter(IWordSizeAdapterCallback* pCB){
		m_pCB = pCB;
	}
	~CWordSizeAdapter(){}

	virtual int getCount()
	{
		return m_arrWordSize.GetCount();
	}

	virtual void getView(int position, SWindow * pItem, pugi::xml_node xmlTemplate)
	{
		if (pItem->GetChildrenCount() == 0)
			pItem->InitFromXml(xmlTemplate);

		SWindow* pWordSize = pItem->FindChildByID(1);
		std::wstring wstrSelSize = m_arrWordSize.GetAt(position).m_wstrWordSize;
		pWordSize->SetWindowText(wstrSelSize.c_str());

		pItem->SetUserData(position);
		pItem->GetEventSet()->subscribeEvent(EventItemPanelClick::EventID, Subscriber(&CWordSizeAdapter::OnItemClick, this));
	}

	SStringT getItemDesc(int position)
	{
		return S_CW2T(m_arrWordSize.GetAt(position).m_wstrWordSize.c_str());
	}

	bool OnItemClick(EventArgs *pEvt)
	{
		EventItemPanelClick* pEvt_ItemPanel = sobj_cast<EventItemPanelClick>(pEvt);
		SASSERT(pEvt_ItemPanel);
		SWindow* pPanel = sobj_cast<SWindow>(pEvt_ItemPanel->sender);
		SOUI::CPoint pt(GET_X_LPARAM(pEvt_ItemPanel->lParam), GET_Y_LPARAM(pEvt_ItemPanel->lParam));
		SWND swnd = pPanel->SwndFromPoint(pt, FALSE);
		SWindow *pClicked = NULL;
		if (swnd)
		{
			pClicked = SWindowMgr::GetWindow(swnd);
		}

		int nIndex = pPanel->GetUserData();
		m_pCB->OnClickItem(nIndex);

		return true;
	}

	wordSize getItem(int position)
	{
		SASSERT(position >= 0 && position < (int)m_arrWordSize.GetCount());
		return m_arrWordSize[position];
	}

	void AddWordSize(const std::wstring& wstrWordSize)
	{
		if (L"" != wstrWordSize)
		{
			wordSize wordSize;
			wordSize.m_wstrWordSize = wstrWordSize;
			m_arrWordSize.Add(wordSize);
			this->notifyDataSetChanged();
		}
	}
};

#endif