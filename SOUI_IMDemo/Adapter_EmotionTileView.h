#ifndef __ADAPTER_EMOTION_TILEVIEW_H__
#define __ADAPTER_EMOTION_TILEVIEW_H__
#pragma once
#include <vector>

#include <helper/SAdapterBase.h>
#include "GlobalUnits.h"

struct ItemData
{
	std::string m_strID;
};

class CAdapter_EmotionTileView : public SAdapterBase
{
public:
	struct IListener
	{
		virtual void OnEmotionItemClick(const std::string& strID) = 0;
	};
public:
	CAdapter_EmotionTileView(IListener* pListener)
		:m_pListener(pListener)
	{
		
	}
	~CAdapter_EmotionTileView(){}

protected:
	virtual int getCount(){
		return static_cast<int>(m_vecItems.size());
	}

	virtual void getView(int position, SWindow * pItem, pugi::xml_node xmlTemplate)
	{
		if (0 == pItem->GetChildrenCount())
			pItem->InitFromXml(xmlTemplate);

		size_t sPos = static_cast<size_t>(position);
		if (sPos >= m_vecItems.size()) return;

		ItemData* pItemData = m_vecItems[sPos];
		if (NULL == pItemData) return;

		SImageWnd* pImg = pItem->FindChildByName2<SImageWnd>(L"emotion");
		if (NULL != pImg)
		{
			pItem->GetEventSet()->
				subscribeEvent(EventItemPanelClick::EventID,
				Subscriber(&CAdapter_EmotionTileView::OnEvtItemPanelClick, this));

			std::map<std::string, IBitmap*>::iterator iter = 
				CGlobalUnits::GetInstance()->m_mapFace.find(pItemData->m_strID);
			if (iter != CGlobalUnits::GetInstance()->m_mapFace.end())
			{
				pImg->SetImage(iter->second);
			}
		}
	}

	bool OnEvtItemPanelClick(EventArgs* e)
	{
		EventItemPanelClick* pItemClickEvt = sobj_cast<EventItemPanelClick>(e);
		if (NULL == pItemClickEvt)
			return false;

		SItemPanel* pItem = sobj_cast<SItemPanel>(pItemClickEvt->sender);
		if (pItem)
		{ 
			int nItem = static_cast<int>(pItem->GetItemIndex());
			ItemData* pData = m_vecItems[nItem];
			if (NULL != pData)
			{
				m_pListener->OnEmotionItemClick(pData->m_strID);
				return true;
			}
		}
		return false;
	}

public:
	void AddItem(const std::string& strID)
	{
		ItemData* pItemFaceData = new ItemData;
		pItemFaceData->m_strID = strID;

		m_vecItems.push_back(pItemFaceData);
		notifyDataSetChanged();
	}

private:
	IListener*		m_pListener;

	std::vector<ItemData*>	m_vecItems;				// Êý¾Ý
};

#endif