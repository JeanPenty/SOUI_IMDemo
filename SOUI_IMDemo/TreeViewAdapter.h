#pragma once

#include <helper/SAdapterBase.h>
struct TreeItemData
{
	TreeItemData():bGroup(false){}
	DWORD uid;//用于每个用户的特定ID
	INT32 gid;//用户的组ID
	SStringT strImg ;//用户图像
	SStringT strName;//用户名
	bool bGroup;
};
class CFriendTreeViewAdapter :public STreeAdapterBase<TreeItemData>
{
public:	
	DWORD GetUid()
	{
		static DWORD uid = 0;
		return ++uid;
	}
	CFriendTreeViewAdapter() {
		TreeItemData data;
		data.strName = L"新朋友";		
		data.gid = 1;
		data.bGroup = true;

		HSTREEITEM hRoot = InsertItem(data);
		SetItemExpanded(hRoot, TRUE);

		data.bGroup = FALSE;			
		data.uid = 1;
		data.strName=L"新的朋友";
		InsertItem(data, hRoot);
		

		TreeItemData data1;
		data1.strName = L"公众号";		
		data1.gid = 2;
		data1.bGroup = true;
		HSTREEITEM hRoot1 = InsertItem(data1);
		SetItemExpanded(hRoot1, TRUE);
		for (int i = 0; i < 6; i++)
		{
			data1.bGroup = false;			
			data1.uid = GetUid();
			data1.strName=SStringT().Format(L"公众号_%d",data1.uid);
			InsertItem(data1, hRoot1);
		}

		TreeItemData data2;
		data2.strName = L"企业号";
		data2.gid = 3;		
		data2.bGroup = TRUE;
		HSTREEITEM hRoot2 = InsertItem(data2);
		SetItemExpanded(hRoot2, TRUE);
		for (int i = 0; i < 2; i++)
		{
			data2.bGroup = false;			
			data2.uid = GetUid();
			data2.strName=SStringT().Format(L"企业号_%d",data2.uid);
			InsertItem(data2, hRoot2);
		}

		TreeItemData data3;
		data3.strName = L"群聊";
		data3.gid = 4;
		data3.bGroup = TRUE;
		HSTREEITEM hRoot3 = InsertItem(data3);
		SetItemExpanded(hRoot3, TRUE);
		for (int i = 0; i < 3; i++)
		{
			data3.bGroup = false;			
			data3.uid = GetUid();
			data3.strName=SStringT().Format(L"群聊_%d",data3.uid);
			InsertItem(data3, hRoot3);
		}

		TreeItemData data4;
		data4.strName = L"A";
		data4.gid = 4+1;
		data4.bGroup = TRUE;
		HSTREEITEM hRoot4 = InsertItem(data4);
		SetItemExpanded(hRoot4, TRUE);
		for (int i = 0; i < 10; i++)
		{
			data4.bGroup = false;
			data4.uid = GetUid();
			data4.strName=SStringT().Format(L"A_%d",data4.uid);
			InsertItem(data4, hRoot4);
		}
		
		data4.strName = L"#";
		data4.gid = 4+'A'+26;
		data4.bGroup = TRUE;
		HSTREEITEM hRoot5 = InsertItem(data4);
		SetItemExpanded(hRoot5, TRUE);
		for (int i = 0; i < 10; i++)
		{
			data4.bGroup = false;
			data4.uid = GetUid();
			data4.strName=SStringT().Format(L"#_%d",data4.uid);
			InsertItem(data4, hRoot5);
		}

	}

	~CFriendTreeViewAdapter() {}

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
		if(itemType==1)
		{
			pItem->GetEventSet()->subscribeEvent(EVT_CMD, Subscriber(&CFriendTreeViewAdapter::OnItemPanleClick, this));		
		}
		else 
		{
			
			pItem->FindChildByName(L"hr")->SetVisible(ii.data.gid!=1);		
		}
		pItem->FindChildByName(L"name")->SetWindowText(ii.data.strName);
	}

	bool OnItemPanleClick(EventArgs *pEvt)
	{
		SItemPanel *pItemPanel = sobj_cast<SItemPanel>(pEvt->sender);
		SASSERT(pItemPanel);

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
};