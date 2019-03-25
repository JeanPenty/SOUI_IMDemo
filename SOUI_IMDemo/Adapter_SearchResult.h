#pragma once

#include <helper/SAdapterBase.h>
#include <vector>

#include "GlobalUnits.h"

struct SearchResultItemData
{
	int nType;			//1�������죬2�������죬3���ĺţ�4���ţ�5���ں�
	std::string strID;
};


class CAdapter_SearchResult : public SAdapterBase
{
public:
	struct IListener
	{
		virtual void SearchResultItemDBClick(int nType, const std::string& strID) = 0;
	};
public:
	CAdapter_SearchResult(SListView* pOwner, IListener* pListener);
	~CAdapter_SearchResult(void);

public:
	void AddItem(const int& nType, const std::string& strID);
	void DeleteAllItem();
protected:
	virtual void getView(int position, SWindow* pItem, pugi::xml_node xmlTemplate);\
		virtual int getCount();
	virtual SStringT getColunName(int iCol) const;

	bool OnEventLvSelChangeing(EventLVSelChanging* pEvt);
	bool OnEventItemPanelDBClick(EventArgs* e);

private:
	std::vector<SearchResultItemData*> m_vecItemInfo;
	SListView*						  m_pOwner;

	IListener*							m_pListener;

	MemberFunctionSlot<CAdapter_SearchResult, EventArgs>			m_evtItemDBClick;
};
