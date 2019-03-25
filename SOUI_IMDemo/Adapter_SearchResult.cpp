#include "StdAfx.h"
#include "Adapter_SearchResult.h"

CAdapter_SearchResult::CAdapter_SearchResult(SListView* pOwner, IListener* pListener)
: m_pOwner(pOwner)
, m_pListener(pListener)
, m_evtItemDBClick(&CAdapter_SearchResult::OnEventItemPanelDBClick, this)
{
	SASSERT(m_pOwner);
	m_pOwner->GetEventSet()->subscribeEvent(&CAdapter_SearchResult::OnEventLvSelChangeing, this);
}

CAdapter_SearchResult::~CAdapter_SearchResult(void)
{
}

void CAdapter_SearchResult::getView(int position, SWindow* pItem, pugi::xml_node xmlTemplate)
{
	if (0 == pItem->GetChildrenCount())
	{
		pItem->InitFromXml(xmlTemplate);
		pItem->GetEventSet()->subscribeEvent(EventItemPanelDbclick::EventID, m_evtItemDBClick);
	}

	size_t sPos = static_cast<size_t>(position);
	if (sPos >= m_vecItemInfo.size())
		return;

	SearchResultItemData* pInfo = m_vecItemInfo[sPos];
	if (NULL == pInfo)
		return;

	//设置可见数据
	//face
	SImageWnd* pItemFace = pItem->FindChildByName2<SImageWnd>(L"item_face");
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
	SStatic* pItemName = pItem->FindChildByName2<SStatic>(L"item_name");
	SASSERT(pItemName);
	SStringW sstrName;
	switch (pInfo->nType)
	{
	case 0://filehelper
		sstrName = L"文件传输助手";
		break;
	case 1://personal
		{
			std::string strName = "";
			PersonalsMap::iterator iter = CGlobalUnits::GetInstance()->m_mapPersonals.find(pInfo->strID);
			if (iter != CGlobalUnits::GetInstance()->m_mapPersonals.end())
				strName = iter->second.m_strName;
			else
				strName = pInfo->strID;

			sstrName = S_CA2W(strName.c_str());
		}
		break;
	case 2://group
		{
			std::string strName = "";
			GroupsMap::iterator iter = CGlobalUnits::GetInstance()->m_mapGroups.find(pInfo->strID);
			if (iter != CGlobalUnits::GetInstance()->m_mapGroups.end())
				strName = iter->second.m_strGroupName;
			else
				strName = pInfo->strID;

			sstrName = S_CA2W(strName.c_str());
		}
		break;
	case 3://订阅号
		sstrName = L"订阅号";
		break;
	case 4://新闻
		sstrName = L"新闻";
		break;
	case 5://公众号
		sstrName = L"公众号";
		break;
	default:
		break;
	}
	pItemName->SetWindowText(sstrName);
}

int CAdapter_SearchResult::getCount()
{
	return static_cast<int>(m_vecItemInfo.size());
}

SStringT CAdapter_SearchResult::getColunName(int iCol) const
{
	return L"col_nick";
}

void CAdapter_SearchResult::AddItem(const int& nType, const std::string& strID)
{
	SearchResultItemData* pItemData = new SearchResultItemData;
	pItemData->nType = nType;
	pItemData->strID = strID;

	m_vecItemInfo.push_back(pItemData);
	notifyDataSetChanged();
}

void CAdapter_SearchResult::DeleteAllItem()
{
	m_vecItemInfo.clear();

	notifyDataSetChanged();
}

bool CAdapter_SearchResult::OnEventItemPanelDBClick(EventArgs* e)
{
	EventItemPanelDbclick* pEvtItemClick = sobj_cast<EventItemPanelDbclick>(e);
	if (NULL == pEvtItemClick)
		return true;

	SItemPanel* pItem = sobj_cast<SItemPanel>(pEvtItemClick->sender);

	int nItem = static_cast<int>(pItem->GetItemIndex());
	SearchResultItemData* pInfo = m_vecItemInfo[nItem];
	if (NULL != pInfo)
	{
		m_pListener->SearchResultItemDBClick(pInfo->nType, pInfo->strID);
	}

	return true;
}

bool CAdapter_SearchResult::OnEventLvSelChangeing(EventLVSelChanging* pEvt)
{
	if(NULL == pEvt)
		return true;

	pEvt->bubbleUp = false;
	if(-1 == pEvt->iNewSel)				// 不能设置 -1 
		pEvt->bCancel = TRUE;

	return true;
}