#include "StdAfx.h"
#include "Adapter_MessageList.h"

CAdapter_MessageList::CAdapter_MessageList(SListView* pOwner, IListener* pListener)
: m_pOwner(pOwner)
, m_pListener(pListener)
, m_evtItemClick(&CAdapter_MessageList::OnEventItemPanelClick, this)
, m_evtItemRClick(&CAdapter_MessageList::OnEventItemPanelRClick, this)
{
	SASSERT(m_pOwner);
	m_pOwner->GetEventSet()->subscribeEvent(&CAdapter_MessageList::OnEventLvSelChangeing, this);
}

CAdapter_MessageList::~CAdapter_MessageList(void)
{
}

void CAdapter_MessageList::getView(int position, SWindow* pItem, pugi::xml_node xmlTemplate)
{
	if (0 == pItem->GetChildrenCount())
	{
		pItem->InitFromXml(xmlTemplate);
		pItem->GetEventSet()->subscribeEvent(EventItemPanelClick::EventID, m_evtItemClick);
		pItem->GetEventSet()->subscribeEvent(EventItemPanelRclick::EventID, m_evtItemRClick);
	}

	size_t sPos = static_cast<size_t>(position);
	if (sPos >= m_vecItemInfo.size())
		return;

	MessageListItemData* pInfo = m_vecItemInfo[sPos];
	if (NULL == pInfo)
		return;

	//���ÿɼ�����
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
	case 3://���ĺ�
		sstrFace = L"skin_dyh";
		break;
	case 4://����
		sstrFace = L"skin_news";
		break;
	case 5://���ں�
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
		sstrName = L"�ļ���������";
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
	case 3://���ĺ�
		sstrName = L"���ĺ�";
		break;
	case 4://����
		sstrName = L"����";
		break;
	case 5://���ں�
		sstrName = L"���ں�";
		break;
	default:
		break;
	}
	pItemName->SetWindowText(sstrName);

	//��Ϣ����
	SStatic* pItemMessageContent = pItem->FindChildByName2<SStatic>(L"lasttalk_message_content");
	SASSERT(pItemMessageContent);
	pItemMessageContent->SetWindowText(L"SOUI_IMDemo");

	//��Ϣʱ������
	SStatic* pItemMessageTime = pItem->FindChildByName2<SStatic>(L"lasttalk_message_time");
	SASSERT(pItemMessageTime);
	pItemMessageTime->SetWindowText(L"2018/12/17");
}

int CAdapter_MessageList::getCount()
{
	return static_cast<int>(m_vecItemInfo.size());
}

SStringT CAdapter_MessageList::getColunName(int iCol) const
{
	return L"col_nick";
}

void CAdapter_MessageList::AddItem(const int& nType, const std::string& strID)
{
	MessageListItemData* pItemData = new MessageListItemData;
	pItemData->nType = nType;
	pItemData->strID = strID;

	m_vecItemInfo.push_back(pItemData);
	notifyDataSetChanged();
}

void CAdapter_MessageList::MoveItemToTop(const std::string& strID)
{
	int nIndex = GetItemIndexByID(strID);
	if (-1 != nIndex)
	{
		size_t sIndex = static_cast<size_t>(nIndex);
		if(sIndex < 0 || sIndex >= m_vecItemInfo.size())
			return ;
		MessageListItemData* pItemData = m_vecItemInfo[nIndex];
		if (pItemData)
		{
			m_vecItemInfo.erase(m_vecItemInfo.begin() + sIndex);
			m_vecItemInfo.insert(m_vecItemInfo.begin(), pItemData);
		}
	}
}

void CAdapter_MessageList::EnsureVisable(const std::string& strID)
{
	int nIndex = GetItemIndexByID(strID);
	if (-1 != nIndex)
		m_pOwner->EnsureVisible(nIndex);
}

void CAdapter_MessageList::SetCurSel(const std::string& strID)
{
	int nIndex = GetItemIndexByID(strID);
	if (-1 != nIndex)
		m_pOwner->SetSel(nIndex, TRUE);
}

int CAdapter_MessageList::GetItemIndexByID(const std::string& strID)
{
	int nIndex = -1;
	for (int i = 0; i < m_vecItemInfo.size(); i++)
	{
		if (m_vecItemInfo[i]->strID == strID)
		{
			nIndex = i;
			break;
		}
	}
	return nIndex;
}

bool CAdapter_MessageList::OnEventItemPanelClick(EventArgs* e)
{
	EventItemPanelClick* pEvtItemClick = sobj_cast<EventItemPanelClick>(e);
	if (NULL == pEvtItemClick)
		return true;

	SItemPanel* pItem = sobj_cast<SItemPanel>(pEvtItemClick->sender);

	int nItem = static_cast<int>(pItem->GetItemIndex());
	MessageListItemData* pInfo = m_vecItemInfo[nItem];
	if (NULL != pInfo)
	{
		m_pListener->MessageListItemClick(pInfo->nType, pInfo->strID);
	}

	return true;
}

bool CAdapter_MessageList::OnEventItemPanelRClick(EventArgs* e)
{
	EventItemPanelRclick* pEvtItemRClick = sobj_cast<EventItemPanelRclick>(e);
	if(NULL == pEvtItemRClick) return true;

	SItemPanel* pItem = sobj_cast<SItemPanel>(pEvtItemRClick->sender);
	if(NULL == pItem) return true;

	int nItem = static_cast<int>(pItem->GetItemIndex());
	MessageListItemData* pInfo = m_vecItemInfo[nItem];
	if (NULL != pInfo)
	{
		m_pListener->MessageListItemRClick(pInfo->nType, pInfo->strID);
	}

	return true;
}

bool CAdapter_MessageList::OnEventLvSelChangeing(EventLVSelChanging* pEvt)
{
	if(NULL == pEvt)
		return true;

	pEvt->bubbleUp = false;
	if(-1 == pEvt->iNewSel)				// �������� -1 
		pEvt->bCancel = TRUE;

	return true;
}