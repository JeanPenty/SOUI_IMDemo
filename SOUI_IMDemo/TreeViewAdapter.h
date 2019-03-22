#pragma once

#include "GlobalUnits.h"

#include <helper/SAdapterBase.h>
struct TreeItemData
{
	TreeItemData():bGroup(false){}
	std::string strID;
	INT32 gid;			//用户的组ID
	SStringT strImg ;	//用户图像
	SStringT strName;	//用户名
	bool bGroup;
};
class CContactTreeViewAdapter :public STreeAdapterBase<TreeItemData>
{
public:
	struct IListener
	{
		virtual void ContactItemClick(int nGID, const std::string& strID) = 0;
		virtual void ContactItemDBClick(int nGID, const std::string& strID) = 0;
		virtual void ContactItemRClick(int nGID, const std::string& strID) = 0;
	};
public:	
	CContactTreeViewAdapter(IListener* pListener) 
	{
		m_pListener = pListener;

		TreeItemData data;
		data.strName = L"新朋友";		
		data.gid = 1;
		data.bGroup = true;

		HSTREEITEM hRoot = InsertItem(data);
		SetItemExpanded(hRoot, TRUE);

		data.bGroup = FALSE;			
		data.strID = "new_friends";
		data.strName = L"新的朋友";
		data.strImg = L"skin_personal";
		InsertItem(data, hRoot);
		

		TreeItemData data1;
		data1.strName = L"公众号";		
		data1.gid = 2;
		data1.bGroup = true;
		HSTREEITEM hRoot1 = InsertItem(data1);
		SetItemExpanded(hRoot1, TRUE);

		data1.bGroup = false;			
		data1.strID = "gh";
		data1.strName = L"测试公众号";
		data1.strImg = L"skin_gzh";
		InsertItem(data1, hRoot1);

		TreeItemData data2;
		data2.strName = L"企业号";
		data2.gid = 3;		
		data2.bGroup = TRUE;
		HSTREEITEM hRoot2 = InsertItem(data2);
		SetItemExpanded(hRoot2, TRUE);

		data2.bGroup = false;			
		data2.strID = "company";
		data2.strName = L"企业号测试";
		data2.strImg = L"skin_dyh";
		InsertItem(data2, hRoot2);

		TreeItemData data3;
		data3.strName = L"群聊";
		data3.gid = 4;
		data3.bGroup = TRUE;
		HSTREEITEM hRoot3 = InsertItem(data3);
		SetItemExpanded(hRoot3, TRUE);

		GroupsMap::iterator iterGrp = CGlobalUnits::GetInstance()->m_mapGroups.begin();
		for (; iterGrp != CGlobalUnits::GetInstance()->m_mapGroups.end(); iterGrp++)
		{
			data3.bGroup = false;			
			data3.strID = iterGrp->second.m_strGroupID;
			SStringW sstrGroupName = S_CA2W(iterGrp->second.m_strGroupName.c_str());
			data3.strName = sstrGroupName;
			data3.strImg = L"skin_group";
			InsertItem(data3, hRoot3);
		}

		TreeItemData data4;
		data4.strName = L"好友";
		data4.gid = 5;
		data4.bGroup = TRUE;
		HSTREEITEM hRoot4 = InsertItem(data4);
		SetItemExpanded(hRoot4, TRUE);

		PersonalsMap::iterator iterPersonal = CGlobalUnits::GetInstance()->m_mapPersonals.begin();
		for (; iterPersonal != CGlobalUnits::GetInstance()->m_mapPersonals.end(); iterPersonal++)
		{
			data4.bGroup = false;
			data4.strID = iterPersonal->second.m_strID;
			SStringW sstrName = S_CA2W(iterPersonal->second.m_strName.c_str());
			data4.strName = sstrName;
			data4.strImg = L"skin_personal";
			InsertItem(data4, hRoot4);
		}
		
// 		data4.strName = L"#";
// 		data4.gid = 4+'A'+26;
// 		data4.bGroup = TRUE;
// 		HSTREEITEM hRoot5 = InsertItem(data4);
// 		SetItemExpanded(hRoot5, TRUE);
// 		for (int i = 0; i < 10; i++)
// 		{
// 			data4.bGroup = false;
// 			data4.uid = GetUid();
// 			data4.strName=SStringT().Format(L"#_%d",data4.uid);
// 			InsertItem(data4, hRoot5);
// 		}

	}

	~CContactTreeViewAdapter() {}

	virtual void getView(SOUI::HTREEITEM loc, SWindow * pItem, pugi::xml_node xmlTemplate)
	{
		ItemInfo & ii = m_tree.GetItemRef((HSTREEITEM)loc);
		int itemType = getViewType(loc);
		if (pItem->GetChildrenCount() == 0)
		{
			switch (itemType)
			{
			case 0:xmlTemplate = xmlTemplate.child(L"item_group");
				break;
			case 1:xmlTemplate = xmlTemplate.child(L"item_data");
				break;
			}
			pItem->InitFromXml(xmlTemplate);
			if(itemType==0)
			{//不让它点	
				pItem->GetEventSet()->setMutedState(true);			
			}
		}
		if(itemType == 1)
		{
			pItem->GetEventSet()->subscribeEvent(EVT_ITEMPANEL_CLICK, 
				Subscriber(&CContactTreeViewAdapter::OnItemPanelClick, this));	
			pItem->GetEventSet()->subscribeEvent(EVT_ITEMPANEL_DBCLICK, 
				Subscriber(&CContactTreeViewAdapter::OnItemPanelDBClick, this));	
			pItem->GetEventSet()->subscribeEvent(EVT_ITEMPANEL_RCLICK, 
				Subscriber(&CContactTreeViewAdapter::OnItemPanelRClick, this));	
			pItem->FindChildByName2<SImageWnd>(L"face")->SetAttribute(L"skin", ii.data.strImg, FALSE);
		}
		else 
		{
			pItem->FindChildByName(L"hr")->SetVisible(ii.data.gid != 1);		
		}
		pItem->FindChildByName(L"name")->SetWindowText(ii.data.strName);
	}

	bool OnItemPanelClick(EventArgs *e)
	{
		EventItemPanelClick* pEvt = sobj_cast<EventItemPanelClick>(e);
		if (NULL == pEvt) return true;

		SItemPanel* pItem = sobj_cast<SItemPanel>(pEvt->sender);
		if (NULL == pItem) return true;

		int hItem = static_cast<SOUI::HTREEITEM>(pItem->GetItemIndex());
		ItemInfo& ii = m_tree.GetItemRef((HSTREEITEM)hItem);
		if (!ii.data.bGroup)
		{
			m_pListener->ContactItemClick(ii.data.gid, ii.data.strID);
		}

		return true;
	}	

	bool OnItemPanelDBClick(EventArgs* e)
	{
		EventItemPanelDbclick* pEvt = sobj_cast<EventItemPanelDbclick>(e);
		if (NULL == pEvt) return true;

		SItemPanel* pItem = sobj_cast<SItemPanel>(pEvt->sender);
		if (NULL == pItem) return true;

		int hItem = static_cast<SOUI::HTREEITEM>(pItem->GetItemIndex());
		ItemInfo& ii = m_tree.GetItemRef((HSTREEITEM)hItem);
		if (!ii.data.bGroup)
		{
			m_pListener->ContactItemDBClick(ii.data.gid, ii.data.strID);
		}

		return true;
	}

	bool OnItemPanelRClick(EventArgs *e)
	{
		EventItemPanelRclick* pEvt = sobj_cast<EventItemPanelRclick>(e);
		if (NULL == pEvt) return true;

		SItemPanel* pItem = sobj_cast<SItemPanel>(pEvt->sender);
		if (NULL == pItem) return true;

		int hItem = static_cast<SOUI::HTREEITEM>(pItem->GetItemIndex());
		ItemInfo& ii = m_tree.GetItemRef((HSTREEITEM)hItem);
		if (!ii.data.bGroup)
		{
			m_pListener->ContactItemRClick(ii.data.gid, ii.data.strID);
		}

		return true;
	}

	virtual int getViewType(SOUI::HTREEITEM hItem) const
	{
		ItemInfo & ii = m_tree.GetItemRef((HSTREEITEM)hItem);
		if (ii.data.bGroup) return 0;
		else return 1;
	}

	virtual int getViewTypeCount() const
	{
		return 2;
	}

private:
	IListener*		m_pListener;
};