#include "StdAfx.h"
#include "Adapters_ContactList.h"

CAdapter_Common::CAdapter_Common(SListView* pOwner)
: m_pOwner(pOwner)
, m_evtItemClick(&CAdapter_Common::OnEventItemPanelClick, this)
, m_evtItemDBClick(&CAdapter_Common::OnEventItemPanelDBClick, this)
{
	SASSERT(m_pOwner);
	m_pOwner->GetEventSet()->subscribeEvent(&CAdapter_Common::OnEventLvSelChangeing, this);
}

CAdapter_Common::~CAdapter_Common(void)
{
}

void CAdapter_Common::getView(int position, SWindow* pItem, pugi::xml_node xmlTemplate)
{
	if (0 == pItem->GetChildrenCount())
	{
		pItem->InitFromXml(xmlTemplate);
		pItem->GetEventSet()->subscribeEvent(EventItemPanelClick::EventID, m_evtItemClick);
		pItem->GetEventSet()->subscribeEvent(EventItemPanelRclick::EventID, m_evtItemDBClick);
	}

	size_t sPos = static_cast<size_t>(position);
	if (sPos >= m_vecItemInfo.size())
		return;

	ContactListsItemData* pInfo = m_vecItemInfo[sPos];
	if (NULL == pInfo)
		return;

	/*
	//设置可见数据
	//face
	SImageWnd* pItemFace = pItem->FindChildByName2<SImageWnd>(L"lasttalk_face");
	SASSERT(pItemFace);
	SStringW sstrFace;
	switch (pInfo->nType)
	{
	case 0://filehelper
		sstrFace = L"skin_filehelper";
		break;
	case 1://personal
		sstrFace = L"skin_personal";
		break;
	case 2://group
		sstrFace = L"skin_group";
		break;
	case 3://订阅号
		sstrFace = L"skin_dyh";
		break;
	case 4://新闻
		sstrFace = L"skin_news";
		break;
	case 5://公众号
		sstrFace = L"skin_gzh";
		break;
	default:
		break;
	}
	pItemFace->SetAttribute(L"skin", sstrFace, TRUE);

	//name
	SStatic* pItemName = pItem->FindChildByName2<SStatic>(L"lasttalk_name");
	SASSERT(pItemName);
	SStringW sstrName;
	switch (pInfo->nType)
	{
	case 0://filehelper
		sstrName = L"SOUI文件传输助手";
		break;
	case 1://personal
		sstrName = L"SOUI个人好友名";
		break;
	case 2://group
		sstrName = L"SOUI群聊名";
		break;
	case 3://订阅号
		sstrName = L"SOUI订阅号";
		break;
	case 4://新闻
		sstrName = L"SOUI新闻";
		break;
	case 5://公众号
		sstrName = L"SOUI公众号";
		break;
	default:
		break;
	}
	pItemName->SetWindowText(sstrName);
	*/
}

int CAdapter_Common::getCount()
{
	return static_cast<int>(m_vecItemInfo.size());
}

SStringT CAdapter_Common::getColunName(int iCol) const
{
	return L"col_nick";
}

void CAdapter_Common::AddItem(const int& nType, const std::string& strID)
{
	ContactListsItemData* pItemData = new ContactListsItemData;
	pItemData->nType = nType;
	pItemData->strID = strID;

	m_vecItemInfo.push_back(pItemData);
	notifyDataSetChanged();
}

bool CAdapter_Common::OnEventItemPanelClick(EventArgs* e)
{
	EventItemPanelClick* pEvtItemClick = sobj_cast<EventItemPanelClick>(e);
	if (NULL == pEvtItemClick)
		return true;

	SItemPanel* pItem = sobj_cast<SItemPanel>(pEvtItemClick->sender);

	int nItem = static_cast<int>(pItem->GetItemIndex());
	ContactListsItemData* pInfo = m_vecItemInfo[nItem];
	if (NULL != pInfo)
	{
		//m_pCB->OnLasttalkListItemClick(pInfo->nType, pInfo->strID);
	}

	return true;
}

bool CAdapter_Common::OnEventItemPanelDBClick(EventArgs* e)
{
	EventItemPanelDbclick* pEvtItemDBClick = sobj_cast<EventItemPanelDbclick>(e);
	if(NULL == pEvtItemDBClick) return true;

	SItemPanel* pItem = sobj_cast<SItemPanel>(pEvtItemDBClick->sender);
	if(NULL == pItem) return true;

	int nItem = static_cast<int>(pItem->GetItemIndex());
	ContactListsItemData* pInfo = m_vecItemInfo[nItem];
	if (NULL != pInfo)
	{
		//m_pCB->OnLasttalkListItemRClick(pInfo->nType, pInfo->strID);
	}

	return true;
}

bool CAdapter_Common::OnEventLvSelChangeing(EventLVSelChanging* pEvt)
{
	if(NULL == pEvt)
		return true;

	pEvt->bubbleUp = false;
	if(-1 == pEvt->iNewSel)				// 不能设置 -1 
		pEvt->bCancel = TRUE;

	return true;
}