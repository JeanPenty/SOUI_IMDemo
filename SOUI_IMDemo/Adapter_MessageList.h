#ifndef __ADAPTER_MESSAGELIST_H__
#define __ADAPTER_MESSAGELIST_H__

#pragma once
#include <helper/SAdapterBase.h>
#include <vector>

#include "GlobalUnits.h"


struct MessageListItemData
{
	int nType;			//0文件传输助手，1个人聊天，2多人聊天，3订阅号，4新闻，5公众号
	std::string strID;
};

class CAdapter_MessageList : public SAdapterBase
{
public:
	struct IListener
	{
		virtual void MessageListItemClick(int nType, const std::string& strID) = 0;
		virtual void MessageListItemRClick(int nType, const std::string& strID) = 0;
	};
public:
	CAdapter_MessageList(SListView* pOwner, IListener* pListener);
	~CAdapter_MessageList(void);

public:
	void AddItem(const int& nType, const std::string& strID);

	void MoveItemToTop(const std::string& strID);
	void EnsureVisable(const std::string& strID);
	void SetCurSel(const std::string& strID);
	int GetItemIndexByID(const std::string& strID);
protected:
	virtual void getView(int position, SWindow* pItem, pugi::xml_node xmlTemplate);\
	virtual int getCount();
	virtual SStringT getColunName(int iCol) const;

	bool OnEventLvSelChangeing(EventLVSelChanging* pEvt);
	bool OnEventItemPanelClick(EventArgs* e);
	bool OnEventItemPanelRClick(EventArgs* e);

private:
	std::vector<MessageListItemData*> m_vecItemInfo;
	SListView*						  m_pOwner;

	IListener*							m_pListener;

	MemberFunctionSlot<CAdapter_MessageList, EventArgs>			m_evtItemClick;
	MemberFunctionSlot<CAdapter_MessageList, EventArgs>			m_evtItemRClick;
};

#endif