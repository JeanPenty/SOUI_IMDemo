#ifndef __ADAPTERS_CONTACTLIST_H__
#define __ADAPTERS_CONTACTLIST_H__

#pragma once
#include <helper/SAdapterBase.h>
#include <vector>

struct ContactListsItemData
{
	int nType;			//0新的好友，1公众号，2企业号，3群聊，4个人
	std::string strID;
};

class CAdapter_Common : public SAdapterBase
{
public:
	CAdapter_Common(SListView* pOwner);
	~CAdapter_Common(void);

public:
	void AddItem(const int& nType, const std::string& strID);

protected:
	virtual void getView(int position, SWindow* pItem, pugi::xml_node xmlTemplate);
	virtual int getCount();
	virtual SStringT getColunName(int iCol) const;

	bool OnEventLvSelChangeing(EventLVSelChanging* pEvt);
	bool OnEventItemPanelClick(EventArgs* e);
	bool OnEventItemPanelDBClick(EventArgs* e);

private:
	std::vector<ContactListsItemData*> m_vecItemInfo;
	SListView*						  m_pOwner;

	MemberFunctionSlot<CAdapter_Common, EventArgs>			m_evtItemClick;
	MemberFunctionSlot<CAdapter_Common, EventArgs>			m_evtItemDBClick;
};

#endif