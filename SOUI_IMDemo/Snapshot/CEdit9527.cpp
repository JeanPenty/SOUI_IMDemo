#include "stdafx.h"
#include "CEdit9527.h"
#include "SSnapshotCtrl.h"

namespace SOUI
{
	CEdit9527::CEdit9527() :m_bDraging(FALSE), m_iHei(0),m_iWid(0), bChanged(false)
	{
	}

	CEdit9527::~CEdit9527()
	{
	}

	void CEdit9527::SetHost(IEditHost * _ieditHost)
	{
		m_pHost = _ieditHost;
	}

	void CEdit9527::PaintToDC(HDC hdc)
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		RECTL rcL = { rcClient.left,rcClient.top,rcClient.right,rcClient.bottom };
		int nOldMode = ::SetGraphicsMode(hdc, GM_COMPATIBLE);
		m_pTxtHost->GetTextService()->TxDraw(
			DVASPECT_CONTENT,          // Draw Aspect
			/*-1*/0,                        // Lindex
			NULL,                    // Info for drawing optimazation
			NULL,                    // target device information
			hdc,            // Draw device HDC
			NULL,                        // Target device HDC
			&rcL,            // Bounding client rectangle
			NULL,             // Clipping rectangle for metafiles
			&rcClient,        // Update rectangle
			NULL,                        // Call back function
			NULL,                    // Call back parameter
			TXTVIEW_ACTIVE);
		::SetGraphicsMode(hdc, nOldMode);
	}

	void CEdit9527::SetFontSize(int size)
	{
		IFontPtr font = m_style.GetTextFont(0);
		if (abs(font->TextSize()) != size)
		{
			SStringT strDesc;
			strDesc.Format(_T("face:%s,size:%d"), font->FamilyName(), size);
			this->SetAttribute(L"font", strDesc);
			IFontPtr pFont = m_style.GetTextFont(0);
			if (SUCCEEDED(InitDefaultCharFormat(&m_cfDef, pFont)))
			{
				m_pTxtHost->GetTextService()->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE,
					TXTBIT_CHARFORMATCHANGE);
			}
		}
	}

	void CEdit9527::SetTextColor(COLORREF color)
	{
		m_style.SetTextColor(0, color);
		SetDefaultTextColor(m_style.GetTextColor(0));
	}

	BOOL CEdit9527::IsContainPoint(const POINT & pt, BOOL bClientOnly) const
	{
		if (!m_pHost->canProcessMsg())
			return FALSE;
		return __super::IsContainPoint(pt, bClientOnly);
	}

	LRESULT CEdit9527::OnCreate(LPVOID)
	{
		LRESULT bRet = __super::OnCreate(NULL);
		if (bRet == 0)
		{
			unsigned oldMask= SSendMessage(EM_GETEVENTMASK, 0, 0);
			SSendMessage(EM_SETEVENTMASK, 0, oldMask| ENM_REQUESTRESIZE | ENM_CHANGE);
			GetEventSet()->subscribeEvent(EVT_RE_NOTIFY, Subscriber(&CEdit9527::OnEditNotify, this));
			GetEventSet()->subscribeEvent(EVT_KILLFOCUS, Subscriber(&CEdit9527::OnKillFocus, this));
		}
		return bRet;
	}

	bool CEdit9527::OnKillFocus(EventArgs * e)
	{
		GetEventSet()->setMutedState(true);
		__super::OnKillFocus(m_swnd);
		if (GetWindowText().IsEmpty())
		{
#ifndef _DEBUG
			GetParent()->DestroyChild(this);
#endif // !_DEBUG
		}
		GetEventSet()->setMutedState(false);
		return true;
	}

	bool CEdit9527::OnEditNotify(EventArgs * e)
	{		
		EventRENotify *pEvtNotify = sobj_cast<EventRENotify>(e);
		switch (pEvtNotify->iNotify)
		{			
		case EN_CHANGE:
			bChanged = true;
			SSendMessage(EM_SETTARGETDEVICE, 0, 1);
			SSendMessage(EM_REQUESTRESIZE, 0, 0);
			break;
		case EN_REQUESTRESIZE:
			if (bChanged)
			{
				REQRESIZE * prqs = (REQRESIZE *)(pEvtNotify->pv);
				m_iHei = prqs->rc.bottom - prqs->rc.top;
				m_iWid = prqs->rc.right - prqs->rc.left;

				CRect Margin=GetStyle().GetMargin();
				m_iWid += Margin.left + Margin.right;
				m_iHei+= Margin.top + Margin.bottom;
				UpdataSize();
				bChanged = false;
			}
			break;
		}
		return true;
	}

	BOOL CEdit9527::OnSetCursor(const CPoint & pt)
	{
		if (!__super::OnSetCursor(pt))
		{
			SetCursor(GETRESPROVIDER->LoadCursor(IDC_SIZEALL));
		}
		return TRUE;
	}

	void CEdit9527::OnNcLButtonDown(UINT nHitTest, CPoint point)
	{
		__super::OnNcLButtonDown(nHitTest, point);
		SetCapture();
		m_bDraging = TRUE;
		m_ptClick = point;
		BringWindowToTop();
	}

	void CEdit9527::OnNcMouseMove(UINT nHitTest, CPoint point)
	{
		if (m_bDraging)
		{
			CRect rcWnd = GetWindowRect();
			CPoint pt = rcWnd.TopLeft() + (point - m_ptClick);
			//((SSnapshotCtrl*)GetParent())->GetEtMovePos(pt, rcWnd.Width(),rcWnd.Height());
			((SSnapshotCtrl*)GetParent())->GetEtMovePos(pt, 60, 60);
			//rcWnd.MoveToXY(pt);			
			//Move(rcWnd);
			SStringT pos;
			pos.Format(L"%d,%d", pt.x, pt.y);
			SetAttribute(L"pos", pos);
			int max_wid = ((SSnapshotCtrl*)GetParent())->GetEtMaxWid(pt.x);
			bool bUpdataRc = false;
			if (max_wid<rcWnd.Width())
			{
				SStringT width;
				width.Format(L"%d", max_wid);
				SetAttribute(L"width", width);
				//强制更新自己的m_rcWindow;
				//GetLayout()->LayoutChildren(GetParent());
				bUpdataRc = true;
			}

			int max_hei = ((SSnapshotCtrl*)GetParent())->GetEtMaxHei(pt.y);
			if (max_hei < rcWnd.Height())
			{
				SStringT height;
				height.Format(L"%d", max_hei);
				SetAttribute(L"height", height);
				//强制更新自己的m_rcWindow;
				//GetLayout()->LayoutChildren(GetParent());
				bUpdataRc = true;
			}
			if(bUpdataRc)
				GetLayout()->LayoutChildren(GetParent());
			SSendMessage(EM_SETTARGETDEVICE, 0, 1);
			bChanged = true;
			SSendMessage(EM_REQUESTRESIZE, 0, 0);

			m_ptClick = point;
			GetParent()->Invalidate();
		}
	}

	void CEdit9527::UpdataSize()
	{
		CRect rcWnd = GetWindowRect();
		if (rcWnd.Height() < m_iHei + 30)
		{
			int max_wid = ((SSnapshotCtrl*)GetParent())->GetEtMaxHei(rcWnd);
			int iMax;
			SStringT height;
			iMax = max(m_iHei + 30, 60);
			iMax = min(max_wid, iMax);
			height.Format(L"%d", iMax);
			SetAttribute(L"height", height);
		}
		else
		{
			SStringT height;
			int iMax;
			iMax = max(m_iHei + 30, 60);
			height.Format(L"%d", iMax);
			SetAttribute(L"height", height);
		}	
		//增大
		if (rcWnd.Width() < m_iWid + 30)
		{
			int max_wid = ((SSnapshotCtrl*)GetParent())->GetEtMaxWid(rcWnd);
			int iMax;
			SStringT width;
			iMax = max(m_iWid +30, 60);
			iMax = min(max_wid, iMax);
			width.Format(L"%d", iMax);
			SetAttribute(L"width", width);
			if (iMax == max_wid)
			{
				//强制更新自己的m_rcWindow;
				GetLayout()->LayoutChildren(GetParent());
				SSendMessage(EM_SETTARGETDEVICE, 0, 0);
			}
		}
		else
		{
			SStringT width;
			int iMax;
			iMax = max(m_iWid + 30, 60);
			width.Format(L"%d", iMax);
			SetAttribute(L"width", width);
			SSendMessage(EM_SETTARGETDEVICE, 0, 1);
		}
	}
	
	void CEdit9527::OnNcLButtonUp(UINT nHitTest, CPoint point)
	{
		m_bDraging = FALSE;
		ReleaseCapture();
		CRect parentRc = GetParent()->GetClientRect();
		CRect rcWnd = GetWindowRect();
		CPoint relpos = rcWnd.TopLeft() - parentRc.TopLeft();
		SStringT pos;
		pos.Format(L"%d,%d", relpos.x, relpos.y);
		SetAttribute(L"pos", pos);	
		
		//Move(NULL);
		SSendMessage(EM_SETTARGETDEVICE, 0, 1);
		
		bChanged = true;
		SSendMessage(EM_REQUESTRESIZE, 0, 0);
	}

	void CEdit9527::OnNcPaint(IRenderTarget * pRT)
	{
		if (!IsVisible(TRUE)) return;
		if (!m_style.GetMargin().IsRectNull() && IsFocused())
		{
			CAutoRefPtr<IPen> pen, oldpen;
			pRT->CreatePen(PS_DASHDOT, RGBA(0, 0, 0, 255), 1, &pen);
			pRT->SelectObject(pen, (IRenderObj**)&oldpen);
			CRect rcWindow = GetWindowRect();
			//CRect rcClient = GetClientRect();
			rcWindow.InflateRect(0, 0, 1, 1);
			pRT->DrawRectangle(rcWindow);
			pRT->SelectObject(oldpen, NULL);
		}
	}

	BOOL CEdit9527::OnEraseBkgnd(IRenderTarget * pRT)
	{
		return TRUE;
	}

	void CEdit9527::OnMouseHover(WPARAM wParam, CPoint ptPos)
	{
		SetFocus();
	}

	void CEdit9527::OnMouseLeave()
	{
		if (!m_bDraging)
			KillFocus();
	}
}