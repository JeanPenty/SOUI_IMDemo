#include "StdAfx.h"
#include "SnapshotDlg.h"
#include "../FileHelper.h"

#include "WordSizeAdapter.h"

CSnapshotDlg::CSnapshotDlg()
: SHostDialog(UIRES.LAYOUT.XML_DLG_SNAPSHOT)
{
	m_mapColorInfo.insert(std::make_pair(1, RGB(0,0,0)));
	m_mapColorInfo.insert(std::make_pair(2, RGB(127,127,127)));
	m_mapColorInfo.insert(std::make_pair(3, RGB(136,0,21)));
	m_mapColorInfo.insert(std::make_pair(4, RGB(237,28,36)));
	m_mapColorInfo.insert(std::make_pair(5, RGB(255,127,39)));
	m_mapColorInfo.insert(std::make_pair(6, RGB(255,242,0)));
	m_mapColorInfo.insert(std::make_pair(7, RGB(34,177,76)));
	m_mapColorInfo.insert(std::make_pair(8, RGB(0,162,232)));
	m_mapColorInfo.insert(std::make_pair(9, RGB(63,72,204)));
	m_mapColorInfo.insert(std::make_pair(10, RGB(163,73,164)));
	m_mapColorInfo.insert(std::make_pair(11, RGB(255,255,255)));
	m_mapColorInfo.insert(std::make_pair(12, RGB(195,195,195)));
	m_mapColorInfo.insert(std::make_pair(13, RGB(185,122,87)));
	m_mapColorInfo.insert(std::make_pair(14, RGB(255,174,201)));
	m_mapColorInfo.insert(std::make_pair(15, RGB(255,201,14)));
	m_mapColorInfo.insert(std::make_pair(16, RGB(239,228,176)));
	m_mapColorInfo.insert(std::make_pair(17, RGB(181,230,29)));
	m_mapColorInfo.insert(std::make_pair(18, RGB(153,217,234)));
	m_mapColorInfo.insert(std::make_pair(19, RGB(112,146,190)));
	m_mapColorInfo.insert(std::make_pair(20, RGB(200,191,231)));

	m_nSelectSize = 1;
}

CSnapshotDlg::~CSnapshotDlg(void)
{
}
//定义一个宏方便调试
#ifdef _DEBUG
//#define MDEBUG
#endif // _DEBUG

BOOL CSnapshotDlg::OnInitDialog(HWND wnd, LPARAM lInitParam)
{
	//SetMsgHandled(FALSE);
	DEVMODE dm;
	ZeroMemory(&dm, sizeof(dm));
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);
	int nxScreen, nyScreen;
#ifndef MDEBUG
 	nxScreen = dm.dmPelsWidth;
 	nyScreen = dm.dmPelsHeight;
#else
	nxScreen = 800;
	nyScreen = 1080;
#endif // !MDEBUG

	::SetWindowPos(this->m_hWnd, HWND_TOPMOST, 0,0, nxScreen, nyScreen, SWP_SHOWWINDOW);
	SetForegroundWindow(this->m_hWnd);
	SOUI::CRect rc(0,0, nxScreen, nyScreen);
	CDC       hScrDC, hMemDC; 
	HBITMAP   hBitmap, hOldBitmap; 
	int       nX, nY, nX2, nY2;  
	int       nWidth, nHeight;
	hScrDC = CreateDC(L"DISPLAY", NULL, NULL, NULL);
	hMemDC = CreateCompatibleDC(hScrDC);
	nX = rc.left;
	nY = rc.top;
	nX2 = rc.right;
	nY2 = rc.bottom;
	if (nX < 0)
		nX = 0;
	if (nY < 0)
		nY = 0;
	if (nX2 > dm.dmPelsWidth)
		nX2 = dm.dmPelsWidth;
	if (nY2 > dm.dmPelsHeight)
		nY2 = dm.dmPelsHeight;
	nWidth = nX2 - nX;
	nHeight = nY2 - nY;
	hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
	BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScrDC, nX, nY, SRCCOPY | CAPTUREBLT);
	hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);

	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetBmpResource(new CBitmap(hBitmap));
	pSnapshot->SetScreenSize(nxScreen, nyScreen);

	pSnapshot->GetEventSet()->subscribeEvent(&CSnapshotDlg::OnEventCapturing, this);
	pSnapshot->GetEventSet()->subscribeEvent(&CSnapshotDlg::OnEventRectMoving, this);
	pSnapshot->GetEventSet()->subscribeEvent(&CSnapshotDlg::OnEventRectCaptured, this);
	pSnapshot->GetEventSet()->subscribeEvent(&CSnapshotDlg::OnEventRectDbClk, this);
	pSnapshot->GetEventSet()->subscribeEvent(&CSnapshotDlg::OnEventReCaptured, this);

	SComboView* pWordSizeCbxView = FindChildByName2<SComboView>(L"cbx_wordsize");
	SASSERT(pWordSizeCbxView);
	SListView* pListview = pWordSizeCbxView->GetListView();
	SASSERT(pListview);
	CWordSizeAdapter* pWordSizeAdapter = new CWordSizeAdapter(this);
	pListview->SetAdapter(pWordSizeAdapter);
	pWordSizeAdapter->Release();

	pWordSizeAdapter->AddWordSize(L"10");
	pWordSizeAdapter->AddWordSize(L"12");
	pWordSizeAdapter->AddWordSize(L"14");
	pWordSizeAdapter->AddWordSize(L"16");
	pWordSizeAdapter->AddWordSize(L"18");
	pWordSizeAdapter->AddWordSize(L"20");

	pWordSizeCbxView->SetCurSel(4);
	pSnapshot->SetFontSize(18);


	EnumEncoder(m_encoderList);

	return TRUE;
}

void CSnapshotDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_ESCAPE)
		EndDialog(IDCANCEL);
	else
		SetMsgHandled(FALSE);
}

void CSnapshotDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	if (pSnapshot->CanRevokeOperate())
	{
		pSnapshot->RevokeOperate();		
	}
	else
		EndDialog(IDCANCEL);
}

bool CSnapshotDlg::OnEventCapturing(EventCapturing* pEvt)
{
	SStatic* pText = FindChildByName2<SStatic>(L"text_title");
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	
	SASSERT(pText);
	SASSERT(pSnapshot);
	

	if (!pText->IsVisible())
		pText->SetVisible(TRUE, FALSE);	

	SOUI::CRect rcCap = pSnapshot->GetCapRect();
	SStringW sstrTitle;
	sstrTitle.Format(L"起始位置：%d.%d  区域大小：%d × %d", rcCap.left, rcCap.top, rcCap.Width(), rcCap.Height());
	pText->SetWindowText(sstrTitle);
	
	SOUI::CRect rcText = pText->GetWindowRect();
	SOUI::CRect rcWnd = GetWindowRect();

	int nX = rcCap.left;
	int nY = rcCap.top - rcText.Height() - 1;
	if (nY < 0)
		nY = rcCap.top ;

	if (rcWnd.right - nX < rcText.Width())
		nX = rcWnd.right - rcText.Width() - 1;

	SStringW sstrTitlePos;
	sstrTitlePos.Format(L"%d,%d", nX, nY);
	pText->SetAttribute(L"pos", sstrTitlePos);
	
	return true;
}

bool CSnapshotDlg::OnEventReCaptured(EventReCap * pEvt)
{
	SWindow* pOperateBar = FindChildByName2<SWindow>(L"operate_bar");	
	SASSERT(pOperateBar);	
	if (pOperateBar->IsVisible())
		pOperateBar->SetVisible(FALSE, TRUE);
	SWindow* pAttrBar = FindChildByName2<SWindow>(L"attrbar");
	SASSERT(pAttrBar);
	SWindow* pWordAttr = pAttrBar->FindChildByName2<SWindow>(L"word_attrbar");
	SWindow* pOtherAttr = pAttrBar->FindChildByName2<SWindow>(L"other_attrbar");
	SASSERT(pWordAttr);
	SASSERT(pOtherAttr);

	if (pAttrBar->IsVisible())
		pAttrBar->SetVisible(FALSE, TRUE);
	if (pWordAttr->IsVisible())
		pWordAttr->SetVisible(FALSE, TRUE);
	if (pOtherAttr->IsVisible())
		pOtherAttr->SetVisible(FALSE, TRUE);


	return true;
}

bool CSnapshotDlg::OnEventRectMoving(EventRectMoving* pEvt)
{
	SStatic* pText = FindChildByName2<SStatic>(L"text_title");
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SWindow* pOperateBar = FindChildByName2<SWindow>(L"operate_bar");
	SASSERT(pText);
	SASSERT(pSnapshot);
	SASSERT(pOperateBar);

	SOUI::CRect rcText = pText->GetWindowRect();
	SOUI::CRect rcWnd = GetWindowRect();
	SOUI::CRect rcCap = pSnapshot->GetCapRect();

	SStringW sstrTitle;
	sstrTitle.Format(L"起始位置：%d.%d  区域大小：%d × %d", rcCap.left, rcCap.top, rcCap.Width(), rcCap.Height());
	pText->SetWindowText(sstrTitle);

	int nX = rcCap.left;
	int nY = rcCap.top - rcText.Height() - 1;
	if (nY < 0)
		nY = rcCap.top ;

	if (rcWnd.right - nX < rcText.Width())
		nX = rcWnd.right - rcText.Width() - 1;

	SStringW sstrTitlePos;
	sstrTitlePos.Format(L"%d,%d", nX, nY);
	pText->SetAttribute(L"pos", sstrTitlePos);

	SOUI::CRect rcOperateBar = pOperateBar->GetWindowRect();
	int nOperateBarX = rcCap.right - rcOperateBar.Width();
	if (nOperateBarX < rcWnd.left)
		nOperateBarX = rcWnd.left;

	//modify by yangjinpeng 2018-07-23	优化AttrBar的位置
	//begin
	int nOperateBarY = 0;	
	if ((rcWnd.bottom - rcCap.bottom - 2 - 55) > rcOperateBar.Height())
		nOperateBarY = rcCap.bottom + 2;
	else if ((rcCap.top - rcWnd.top - 2) > rcOperateBar.Height())	
		nOperateBarY = rcCap.top - rcOperateBar.Height() - 2;
	else
		nOperateBarY = rcCap.top + 2;

// 	int nOperateBarY = 0;	
// 	if ((rcWnd.bottom - rcCap.bottom - 2) > rcOperateBar.Height())			// bottom
// 		nOperateBarY = rcCap.bottom + 2;
// 	else if ((rcCap.top - rcWnd.top - 2) > rcOperateBar.Height())				// top  有 空间
// 		nOperateBarY = rcCap.top - rcOperateBar.Height() - 2;
// 	else // 右上角 内 显示
// 		nOperateBarY = rcCap.top + 2;
	//end

	SStringW ssOperateBarPos;
	ssOperateBarPos.Format(_T("%d,%d"), nOperateBarX, nOperateBarY);
	pOperateBar->SetAttribute(L"pos", ssOperateBarPos, FALSE);

	return true;
}

bool CSnapshotDlg::OnEventRectCaptured(EventRectCaptured* pEvt)
{
	SStatic* pText = FindChildByName2<SStatic>(L"text_title");
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SWindow* pOperateBar = FindChildByName2<SWindow>(L"operate_bar");
	SASSERT(pText);
	SASSERT(pSnapshot);
	SASSERT(pOperateBar);

	if (!pText->IsVisible())
		pText->SetVisible(TRUE, FALSE);
	if (!pOperateBar->IsVisible())
		pOperateBar->SetVisible(TRUE, FALSE);

	SOUI::CRect rcCap = pSnapshot->GetCapRect();
	SStringW sstrTitle;
	sstrTitle.Format(L"起始位置：%d.%d  区域大小：%d × %d", rcCap.left, rcCap.top, rcCap.Width(), rcCap.Height());
	pText->SetWindowText(sstrTitle);

	SOUI::CRect rcText = pText->GetWindowRect();
	SOUI::CRect rcWnd = GetWindowRect();

	int nX = rcCap.left;
	int nY = rcCap.top - rcText.Height() - 1;
	if (nY < 0)
		nY = rcCap.top;

	if (rcWnd.right - nX < rcText.Width())
		nX = rcWnd.right - rcText.Width() - 1;

	SStringW sstrTitlePos;
	sstrTitlePos.Format(L"%d,%d", nX, nY);
	pText->SetAttribute(L"pos", sstrTitlePos);

	SOUI::CRect rcOperateBar = pOperateBar->GetWindowRect();
	int nOperateBarX = rcCap.right - rcOperateBar.Width();
	if (nOperateBarX < rcWnd.left)
		nOperateBarX = rcWnd.left;

	//modify by yangjinpeng 2018-07-23	优化AttrBar的位置
	//begin
	int nOperateBarY = 0;
	if ((rcWnd.bottom - rcCap.bottom - 2) > rcOperateBar.Height())
		nOperateBarY = rcCap.bottom + 2;
	else if ((rcCap.top - rcWnd.top - 2) > rcOperateBar.Height())
		nOperateBarY = rcCap.top - rcOperateBar.Height() - 2;
	else
		nOperateBarY = rcCap.top + 2;

	SStringW ssOperateBarPos;
	ssOperateBarPos.Format(_T("%d,%d"), nOperateBarX, nOperateBarY);
	pOperateBar->SetAttribute(L"pos", ssOperateBarPos, FALSE);

	return true;
}

bool CSnapshotDlg::OnEventRectDbClk(EventRectDbClk* pEvt)
{
// 	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
// 	SASSERT(pSnapshot);
// 	pSnapshot->SaveCapBmpToClipboard();
// 
// 	EventSnapshotFinish* e = new EventSnapshotFinish(this);
// 	SNotifyCenter::getSingleton().FireEventAsync(e);
// 	pEvt->Release();
	
	EndDialog(IDOK);
	return true;
}

void CSnapshotDlg::OnBnClickRect()
{
	SImageButton* pImgBtnRect = FindChildByName2<SImageButton>(L"btn_rect");
	SImageButton* pImgBtnEllipse = FindChildByName2<SImageButton>(L"btn_ellipse");
	SImageButton* pImgBtnArrow = FindChildByName2<SImageButton>(L"btn_arrow");
	SImageButton* pImgBtnDoodle = FindChildByName2<SImageButton>(L"btn_doodle");
	SImageButton* pImgBtnWord = FindChildByName2<SImageButton>(L"btn_word");
	SImageButton* pImgBtnMask = FindChildByName2<SImageButton>(L"btn_mask");
	SASSERT(pImgBtnRect);
	SASSERT(pImgBtnEllipse);
	SASSERT(pImgBtnArrow);
	SASSERT(pImgBtnDoodle);
	SASSERT(pImgBtnMask);
	SASSERT(pImgBtnWord);

	pImgBtnRect->SetCheck(TRUE);
	pImgBtnEllipse->SetCheck(FALSE);
	pImgBtnArrow->SetCheck(FALSE);
	pImgBtnDoodle->SetCheck(FALSE);
	pImgBtnMask->SetCheck(FALSE);
	pImgBtnWord->SetCheck(FALSE);

	SWindow* pOperateBar = FindChildByName2<SWindow>(L"operate_bar");
	SASSERT(pOperateBar);
	SWindow* pAttrBar = FindChildByName2<SWindow>(L"attrbar");
	SASSERT(pAttrBar);
	SWindow* pWordAttr = pAttrBar->FindChildByName2<SWindow>(L"word_attrbar");
	SWindow* pOtherAttr = pAttrBar->FindChildByName2<SWindow>(L"other_attrbar");
	SASSERT(pWordAttr);
	SASSERT(pOtherAttr);

	if (!pAttrBar->IsVisible())
		pAttrBar->SetVisible(TRUE, FALSE);
	if (pWordAttr->IsVisible())
		pWordAttr->SetVisible(FALSE, FALSE);
	if (!pOtherAttr->IsVisible())
		pOtherAttr->SetVisible(TRUE, FALSE);

	SImageButton* pImgBtnSmallDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_smalldot");
	SImageButton* pImgBtnMiddleDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_middledot");
	SImageButton* pImgBtnBigDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_bigdot");
	SASSERT(pImgBtnSmallDot);
	SASSERT(pImgBtnMiddleDot);
	SASSERT(pImgBtnBigDot);
	switch (m_nSelectSize)
	{
	case 1:
		{
			pImgBtnSmallDot->SetCheck(TRUE);
			pImgBtnMiddleDot->SetCheck(FALSE);
			pImgBtnBigDot->SetCheck(FALSE);
		}
		break;
	case 2:
		{
			pImgBtnSmallDot->SetCheck(FALSE);
			pImgBtnMiddleDot->SetCheck(TRUE);
			pImgBtnBigDot->SetCheck(FALSE);
		}
		break;
	case 5:
		{
			pImgBtnSmallDot->SetCheck(FALSE);
			pImgBtnMiddleDot->SetCheck(FALSE);
			pImgBtnBigDot->SetCheck(TRUE);
		}
		break;
	}

	//modify by yangjinpeng 2018-07-23	优化AttrBar的位置
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);

	SOUI::CRect rcWnd = GetWindowRect();
	SOUI::CRect rcOperateBar = pOperateBar->GetWindowRect();
	SOUI::CRect rcAttrBar = pAttrBar->GetWindowRect();
	SOUI::CRect rcCap = pSnapshot->GetCapRect();

	int nAttrBarX = rcOperateBar.right - rcAttrBar.Width();
	int nAttrBarY;

	if (rcOperateBar.bottom < rcCap.top)    //右上角
	{
		if ((rcOperateBar.top - 2 - rcAttrBar.Height()) > 0)
			nAttrBarY = rcOperateBar.top - 2 - rcAttrBar.Height();
		else
			nAttrBarY = rcOperateBar.bottom + 2;
	}
	else
		nAttrBarY = rcOperateBar.bottom + 2;

	SStringW ssAttrBarPos;
	ssAttrBarPos.Format(_T("%d,%d"), nAttrBarX, nAttrBarY);
	pAttrBar->SetAttribute(L"pos", ssAttrBarPos, FALSE);

	pSnapshot->SetOperateType(1);
}

void CSnapshotDlg::OnBnClickEllipse()
{
	SImageButton* pImgBtnRect = FindChildByName2<SImageButton>(L"btn_rect");
	SImageButton* pImgBtnEllipse = FindChildByName2<SImageButton>(L"btn_ellipse");
	SImageButton* pImgBtnArrow = FindChildByName2<SImageButton>(L"btn_arrow");
	SImageButton* pImgBtnDoodle = FindChildByName2<SImageButton>(L"btn_doodle");
	SImageButton* pImgBtnWord = FindChildByName2<SImageButton>(L"btn_word");
	SImageButton* pImgBtnMask = FindChildByName2<SImageButton>(L"btn_mask");
	SASSERT(pImgBtnRect);
	SASSERT(pImgBtnEllipse);
	SASSERT(pImgBtnArrow);
	SASSERT(pImgBtnDoodle);
	SASSERT(pImgBtnMask);
	SASSERT(pImgBtnWord);

	pImgBtnRect->SetCheck(FALSE);
	pImgBtnEllipse->SetCheck(TRUE);
	pImgBtnArrow->SetCheck(FALSE);
	pImgBtnDoodle->SetCheck(FALSE);
	pImgBtnMask->SetCheck(FALSE);
	pImgBtnWord->SetCheck(FALSE);

	SWindow* pOperateBar = FindChildByName2<SWindow>(L"operate_bar");
	SASSERT(pOperateBar);
	SWindow* pAttrBar = FindChildByName2<SWindow>(L"attrbar");
	SASSERT(pAttrBar);
	SWindow* pWordAttr = pAttrBar->FindChildByName2<SWindow>(L"word_attrbar");
	SWindow* pOtherAttr = pAttrBar->FindChildByName2<SWindow>(L"other_attrbar");
	SASSERT(pWordAttr);
	SASSERT(pOtherAttr);

	if (!pAttrBar->IsVisible())
		pAttrBar->SetVisible(TRUE, FALSE);
	if (pWordAttr->IsVisible())
		pWordAttr->SetVisible(FALSE, FALSE);
	if (!pOtherAttr->IsVisible())
		pOtherAttr->SetVisible(TRUE, FALSE);

	SImageButton* pImgBtnSmallDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_smalldot");
	SImageButton* pImgBtnMiddleDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_middledot");
	SImageButton* pImgBtnBigDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_bigdot");
	SASSERT(pImgBtnSmallDot);
	SASSERT(pImgBtnMiddleDot);
	SASSERT(pImgBtnBigDot);
	switch (m_nSelectSize)
	{
	case 1:
		{
			pImgBtnSmallDot->SetCheck(TRUE);
			pImgBtnMiddleDot->SetCheck(FALSE);
			pImgBtnBigDot->SetCheck(FALSE);
		}
		break;
	case 2:
		{
			pImgBtnSmallDot->SetCheck(FALSE);
			pImgBtnMiddleDot->SetCheck(TRUE);
			pImgBtnBigDot->SetCheck(FALSE);
		}
		break;
	case 5:
		{
			pImgBtnSmallDot->SetCheck(FALSE);
			pImgBtnMiddleDot->SetCheck(FALSE);
			pImgBtnBigDot->SetCheck(TRUE);
		}
		break;
	}

	//modify by yangjinpeng 2018-07-23	优化AttrBar的位置
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);

	SOUI::CRect rcWnd = GetWindowRect();
	SOUI::CRect rcOperateBar = pOperateBar->GetWindowRect();
	SOUI::CRect rcAttrBar = pAttrBar->GetWindowRect();
	SOUI::CRect rcCap = pSnapshot->GetCapRect();

	int nAttrBarX = rcOperateBar.right - rcAttrBar.Width();
	int nAttrBarY;

	if (rcOperateBar.bottom < rcCap.top)    //右上角
	{
		if ((rcOperateBar.top - 2 - rcAttrBar.Height()) > 0)
			nAttrBarY = rcOperateBar.top - 2 - rcAttrBar.Height();
		else
			nAttrBarY = rcOperateBar.bottom + 2;
	}
	else
		nAttrBarY = rcOperateBar.bottom + 2;

	SStringW ssAttrBarPos;
	ssAttrBarPos.Format(_T("%d,%d"), nAttrBarX, nAttrBarY);
	pAttrBar->SetAttribute(L"pos", ssAttrBarPos, FALSE);
	pSnapshot->SetOperateType(2);
}

void CSnapshotDlg::OnBnClickArrow()
{
	SImageButton* pImgBtnRect = FindChildByName2<SImageButton>(L"btn_rect");
	SImageButton* pImgBtnEllipse = FindChildByName2<SImageButton>(L"btn_ellipse");
	SImageButton* pImgBtnArrow = FindChildByName2<SImageButton>(L"btn_arrow");
	SImageButton* pImgBtnDoodle = FindChildByName2<SImageButton>(L"btn_doodle");
	SImageButton* pImgBtnWord = FindChildByName2<SImageButton>(L"btn_word");
	SImageButton* pImgBtnMask = FindChildByName2<SImageButton>(L"btn_mask");
	SASSERT(pImgBtnRect);
	SASSERT(pImgBtnEllipse);
	SASSERT(pImgBtnArrow);
	SASSERT(pImgBtnDoodle);
	SASSERT(pImgBtnMask);
	SASSERT(pImgBtnWord);

	pImgBtnRect->SetCheck(FALSE);
	pImgBtnEllipse->SetCheck(FALSE);
	pImgBtnArrow->SetCheck(TRUE);
	pImgBtnDoodle->SetCheck(FALSE);
	pImgBtnMask->SetCheck(FALSE);
	pImgBtnWord->SetCheck(FALSE);

	SWindow* pOperateBar = FindChildByName2<SWindow>(L"operate_bar");
	SASSERT(pOperateBar);
	SWindow* pAttrBar = FindChildByName2<SWindow>(L"attrbar");
	SASSERT(pAttrBar);
	SWindow* pWordAttr = pAttrBar->FindChildByName2<SWindow>(L"word_attrbar");
	SWindow* pOtherAttr = pAttrBar->FindChildByName2<SWindow>(L"other_attrbar");
	SASSERT(pWordAttr);
	SASSERT(pOtherAttr);

	if (!pAttrBar->IsVisible())
		pAttrBar->SetVisible(TRUE, FALSE);
	if (pWordAttr->IsVisible())
		pWordAttr->SetVisible(FALSE, FALSE);
	if (!pOtherAttr->IsVisible())
		pOtherAttr->SetVisible(TRUE, FALSE);

	SImageButton* pImgBtnSmallDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_smalldot");
	SImageButton* pImgBtnMiddleDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_middledot");
	SImageButton* pImgBtnBigDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_bigdot");
	SASSERT(pImgBtnSmallDot);
	SASSERT(pImgBtnMiddleDot);
	SASSERT(pImgBtnBigDot);
	switch (m_nSelectSize)
	{
	case 1:
		{
			pImgBtnSmallDot->SetCheck(TRUE);
			pImgBtnMiddleDot->SetCheck(FALSE);
			pImgBtnBigDot->SetCheck(FALSE);
		}
		break;
	case 2:
		{
			pImgBtnSmallDot->SetCheck(FALSE);
			pImgBtnMiddleDot->SetCheck(TRUE);
			pImgBtnBigDot->SetCheck(FALSE);
		}
		break;
	case 5:
		{
			pImgBtnSmallDot->SetCheck(FALSE);
			pImgBtnMiddleDot->SetCheck(FALSE);
			pImgBtnBigDot->SetCheck(TRUE);
		}
		break;
	}

	//modify by yangjinpeng 2018-07-23	优化AttrBar的位置
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);

	SOUI::CRect rcWnd = GetWindowRect();
	SOUI::CRect rcOperateBar = pOperateBar->GetWindowRect();
	SOUI::CRect rcAttrBar = pAttrBar->GetWindowRect();
	SOUI::CRect rcCap = pSnapshot->GetCapRect();

	int nAttrBarX = rcOperateBar.right - rcAttrBar.Width();
	int nAttrBarY;

	if (rcOperateBar.bottom < rcCap.top)    //右上角
	{
		if ((rcOperateBar.top - 2 - rcAttrBar.Height()) > 0)
			nAttrBarY = rcOperateBar.top - 2 - rcAttrBar.Height();
		else
			nAttrBarY = rcOperateBar.bottom + 2;
	}
	else
		nAttrBarY = rcOperateBar.bottom + 2;

	SStringW ssAttrBarPos;
	ssAttrBarPos.Format(_T("%d,%d"), nAttrBarX, nAttrBarY);
	pAttrBar->SetAttribute(L"pos", ssAttrBarPos, FALSE);
	pSnapshot->SetOperateType(3);
}

void CSnapshotDlg::OnBnClickDoodle()
{
	SImageButton* pImgBtnRect = FindChildByName2<SImageButton>(L"btn_rect");
	SImageButton* pImgBtnEllipse = FindChildByName2<SImageButton>(L"btn_ellipse");
	SImageButton* pImgBtnArrow = FindChildByName2<SImageButton>(L"btn_arrow");
	SImageButton* pImgBtnDoodle = FindChildByName2<SImageButton>(L"btn_doodle");
	SImageButton* pImgBtnWord = FindChildByName2<SImageButton>(L"btn_word");
	SImageButton* pImgBtnMask = FindChildByName2<SImageButton>(L"btn_mask");
	SASSERT(pImgBtnRect);
	SASSERT(pImgBtnEllipse);
	SASSERT(pImgBtnArrow);
	SASSERT(pImgBtnDoodle);
	SASSERT(pImgBtnMask);
	SASSERT(pImgBtnWord);

	pImgBtnRect->SetCheck(FALSE);
	pImgBtnEllipse->SetCheck(FALSE);
	pImgBtnArrow->SetCheck(FALSE);
	pImgBtnDoodle->SetCheck(TRUE);
	pImgBtnMask->SetCheck(FALSE);
	pImgBtnWord->SetCheck(FALSE);

	SWindow* pOperateBar = FindChildByName2<SWindow>(L"operate_bar");
	SASSERT(pOperateBar);
	SWindow* pAttrBar = FindChildByName2<SWindow>(L"attrbar");
	SASSERT(pAttrBar);
	SWindow* pWordAttr = pAttrBar->FindChildByName2<SWindow>(L"word_attrbar");
	SWindow* pOtherAttr = pAttrBar->FindChildByName2<SWindow>(L"other_attrbar");
	SASSERT(pWordAttr);
	SASSERT(pOtherAttr);

	if (!pAttrBar->IsVisible())
		pAttrBar->SetVisible(TRUE, FALSE);
	if (pWordAttr->IsVisible())
		pWordAttr->SetVisible(FALSE, FALSE);
	if (!pOtherAttr->IsVisible())
		pOtherAttr->SetVisible(TRUE, FALSE);

	SImageButton* pImgBtnSmallDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_smalldot");
	SImageButton* pImgBtnMiddleDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_middledot");
	SImageButton* pImgBtnBigDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_bigdot");
	SASSERT(pImgBtnSmallDot);
	SASSERT(pImgBtnMiddleDot);
	SASSERT(pImgBtnBigDot);
	switch (m_nSelectSize)
	{
	case 1:
		{
			pImgBtnSmallDot->SetCheck(TRUE);
			pImgBtnMiddleDot->SetCheck(FALSE);
			pImgBtnBigDot->SetCheck(FALSE);
		}
		break;
	case 2:
		{
			pImgBtnSmallDot->SetCheck(FALSE);
			pImgBtnMiddleDot->SetCheck(TRUE);
			pImgBtnBigDot->SetCheck(FALSE);
		}
		break;
	case 5:
		{
			pImgBtnSmallDot->SetCheck(FALSE);
			pImgBtnMiddleDot->SetCheck(FALSE);
			pImgBtnBigDot->SetCheck(TRUE);
		}
		break;
	}

	//modify by yangjinpeng 2018-07-23	优化AttrBar的位置
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);

	SOUI::CRect rcWnd = GetWindowRect();
	SOUI::CRect rcOperateBar = pOperateBar->GetWindowRect();
	SOUI::CRect rcAttrBar = pAttrBar->GetWindowRect();
	SOUI::CRect rcCap = pSnapshot->GetCapRect();

	int nAttrBarX = rcOperateBar.right - rcAttrBar.Width();
	int nAttrBarY;

	if (rcOperateBar.bottom < rcCap.top)    //右上角
	{
		if ((rcOperateBar.top - 2 - rcAttrBar.Height()) > 0)
			nAttrBarY = rcOperateBar.top - 2 - rcAttrBar.Height();
		else
			nAttrBarY = rcOperateBar.bottom + 2;
	}
	else
		nAttrBarY = rcOperateBar.bottom + 2;

	SStringW ssAttrBarPos;
	ssAttrBarPos.Format(_T("%d,%d"), nAttrBarX, nAttrBarY);
	pAttrBar->SetAttribute(L"pos", ssAttrBarPos, FALSE);
	pSnapshot->SetOperateType(4);
}

void CSnapshotDlg::OnBnClickMask()
{
	SImageButton* pImgBtnRect = FindChildByName2<SImageButton>(L"btn_rect");
	SImageButton* pImgBtnEllipse = FindChildByName2<SImageButton>(L"btn_ellipse");
	SImageButton* pImgBtnArrow = FindChildByName2<SImageButton>(L"btn_arrow");
	SImageButton* pImgBtnDoodle = FindChildByName2<SImageButton>(L"btn_doodle");
	SImageButton* pImgBtnWord = FindChildByName2<SImageButton>(L"btn_word");
	SImageButton* pImgBtnMask = FindChildByName2<SImageButton>(L"btn_mask");
	SASSERT(pImgBtnRect);
	SASSERT(pImgBtnEllipse);
	SASSERT(pImgBtnArrow);
	SASSERT(pImgBtnDoodle);
	SASSERT(pImgBtnMask);
	SASSERT(pImgBtnWord);

	pImgBtnRect->SetCheck(FALSE);
	pImgBtnEllipse->SetCheck(FALSE);
	pImgBtnArrow->SetCheck(FALSE);
	pImgBtnDoodle->SetCheck(FALSE);
	pImgBtnMask->SetCheck(TRUE);
	pImgBtnWord->SetCheck(FALSE);

	SWindow* pOperateBar = FindChildByName2<SWindow>(L"operate_bar");
	SASSERT(pOperateBar);
	SWindow* pAttrBar = FindChildByName2<SWindow>(L"attrbar");
	SASSERT(pAttrBar);
	SWindow* pWordAttr = pAttrBar->FindChildByName2<SWindow>(L"word_attrbar");
	SWindow* pOtherAttr = pAttrBar->FindChildByName2<SWindow>(L"other_attrbar");
	SASSERT(pWordAttr);
	SASSERT(pOtherAttr);

	if (!pAttrBar->IsVisible())
		pAttrBar->SetVisible(TRUE, FALSE);
	if (pWordAttr->IsVisible())
		pWordAttr->SetVisible(FALSE, FALSE);
	if (!pOtherAttr->IsVisible())
		pOtherAttr->SetVisible(TRUE, FALSE);

	SImageButton* pImgBtnSmallDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_smalldot");
	SImageButton* pImgBtnMiddleDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_middledot");
	SImageButton* pImgBtnBigDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_bigdot");
	SASSERT(pImgBtnSmallDot);
	SASSERT(pImgBtnMiddleDot);
	SASSERT(pImgBtnBigDot);
	switch (m_nSelectSize)
	{
	case 1:
		{
			pImgBtnSmallDot->SetCheck(TRUE);
			pImgBtnMiddleDot->SetCheck(FALSE);
			pImgBtnBigDot->SetCheck(FALSE);
		}
		break;
	case 2:
		{
			pImgBtnSmallDot->SetCheck(FALSE);
			pImgBtnMiddleDot->SetCheck(TRUE);
			pImgBtnBigDot->SetCheck(FALSE);
		}
		break;
	case 5:
		{
			pImgBtnSmallDot->SetCheck(FALSE);
			pImgBtnMiddleDot->SetCheck(FALSE);
			pImgBtnBigDot->SetCheck(TRUE);
		}
		break;
	}

	//modify by yangjinpeng 2018-07-23	优化AttrBar的位置
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);

	SOUI::CRect rcWnd = GetWindowRect();
	SOUI::CRect rcOperateBar = pOperateBar->GetWindowRect();
	SOUI::CRect rcAttrBar = pAttrBar->GetWindowRect();
	SOUI::CRect rcCap = pSnapshot->GetCapRect();

	int nAttrBarX = rcOperateBar.right - rcAttrBar.Width();
	int nAttrBarY;

	if (rcOperateBar.bottom < rcCap.top)    //右上角
	{
		if ((rcOperateBar.top - 2 - rcAttrBar.Height()) > 0)
			nAttrBarY = rcOperateBar.top - 2 - rcAttrBar.Height();
		else
			nAttrBarY = rcOperateBar.bottom + 2;
	}
	else
		nAttrBarY = rcOperateBar.bottom + 2;

	SStringW ssAttrBarPos;
	ssAttrBarPos.Format(_T("%d,%d"), nAttrBarX, nAttrBarY);
	pAttrBar->SetAttribute(L"pos", ssAttrBarPos, FALSE);
	pSnapshot->SetOperateType(5);
}

void CSnapshotDlg::OnBnClickWord()
{
	SImageButton* pImgBtnRect = FindChildByName2<SImageButton>(L"btn_rect");
	SImageButton* pImgBtnEllipse = FindChildByName2<SImageButton>(L"btn_ellipse");
	SImageButton* pImgBtnArrow = FindChildByName2<SImageButton>(L"btn_arrow");
	SImageButton* pImgBtnDoodle = FindChildByName2<SImageButton>(L"btn_doodle");
	SImageButton* pImgBtnWord = FindChildByName2<SImageButton>(L"btn_word");
	SImageButton* pImgBtnMask = FindChildByName2<SImageButton>(L"btn_mask");
	
	SWindow* pOperateBar = FindChildByName2<SWindow>(L"operate_bar");
	SASSERT(pOperateBar);
	SWindow* pAttrBar = FindChildByName2<SWindow>(L"attrbar");
	SASSERT(pAttrBar);
	SWindow* pWordAttr = pAttrBar->FindChildByName2<SWindow>(L"word_attrbar");
	SWindow* pOtherAttr = pAttrBar->FindChildByName2<SWindow>(L"other_attrbar");
	SASSERT(pWordAttr);
	SASSERT(pOtherAttr);

	if (!pAttrBar->IsVisible())
		pAttrBar->SetVisible(TRUE, FALSE);
	if (pOtherAttr->IsVisible())
		pOtherAttr->SetVisible(FALSE, FALSE);
	if (!pWordAttr->IsVisible())
		pWordAttr->SetVisible(TRUE, FALSE);

	//modify by yangjinpeng 2018-07-23	优化AttrBar的位置
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);

	SOUI::CRect rcWnd = GetWindowRect();
	SOUI::CRect rcOperateBar = pOperateBar->GetWindowRect();
	SOUI::CRect rcAttrBar = pAttrBar->GetWindowRect();
	SOUI::CRect rcCap = pSnapshot->GetCapRect();

	int nAttrBarX = rcOperateBar.right - rcAttrBar.Width();
	int nAttrBarY;

	if (rcOperateBar.bottom < rcCap.top)    //右上角
	{
		if ((rcOperateBar.top - 2 - rcAttrBar.Height()) > 0)
			nAttrBarY = rcOperateBar.top - 2 - rcAttrBar.Height();
		else
			nAttrBarY = rcOperateBar.bottom + 2;
	}
	else
		nAttrBarY = rcOperateBar.bottom + 2;

	SStringW ssAttrBarPos;
	ssAttrBarPos.Format(_T("%d,%d"), nAttrBarX, nAttrBarY);
	pAttrBar->SetAttribute(L"pos", ssAttrBarPos, FALSE);

	SASSERT(pImgBtnRect);
	SASSERT(pImgBtnEllipse);
	SASSERT(pImgBtnArrow);
	SASSERT(pImgBtnDoodle);
	SASSERT(pImgBtnMask);
	SASSERT(pImgBtnWord);

	pImgBtnRect->SetCheck(FALSE);
	pImgBtnEllipse->SetCheck(FALSE);
	pImgBtnArrow->SetCheck(FALSE);
	pImgBtnDoodle->SetCheck(FALSE);
	pImgBtnMask->SetCheck(FALSE);
	pImgBtnWord->SetCheck(TRUE);

	pSnapshot->SetOperateType(6);
}

void CSnapshotDlg::OnBnClickRevoke()
{
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->RevokeOperate();
}

void CSnapshotDlg::OnBnClickCopy()
{
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SaveCapBmpToClipboard();
	EndDialog(IDOK);
}

SStringT& CSnapshotDlg::CreateNewFileName(SStringT &filename)
{
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	filename.Format(_T("老杨截图%4d%02d%02d%02d%02d%02d%03d"), sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
	return filename;
}

void CSnapshotDlg::EnumEncoder(std::vector<EncoderInf> &encoderList)
{
	encoderList.clear();
	//编码器总数
	UINT num;
	//保存编码器信息需要的空间大小
	UINT size;
	ImageCodecInfo *pImageCodecInfo;
	GetImageEncodersSize(&num, &size);
	//获取编码器数组
	pImageCodecInfo = (ImageCodecInfo*)malloc(size);
	GetImageEncoders(num, size, pImageCodecInfo);
	encoderList.resize(num);
	std::wstring des;
	for (UINT j = 0; j < num; j++) {
		des = pImageCodecInfo[j].FormatDescription;
		des.push_back(L'\0');
		des+=pImageCodecInfo[j].FilenameExtension;
		des.push_back(L'\0');
		encoderList.insert(encoderList.begin(),EncoderInf(des, pImageCodecInfo[j].Clsid));
	}
	//reverse(encoderList.begin(), encoderList.end());

	free(pImageCodecInfo);
}

std::wstring &CSnapshotDlg::GetEncoderStringFilter(std::wstring &strFilter)
{
	std::vector<EncoderInf>::iterator iter;
	iter = m_encoderList.begin();
	while (iter != m_encoderList.end()) {
		strFilter += iter->name;
		iter++;
	}
	strFilter.push_back(L'\0');

	return strFilter;
}

bool CSnapshotDlg::GetEncodeCLSID(int filterIdx, CLSID &clsId)
{
	if (filterIdx > m_encoderList.size())
		return false;
	clsId = m_encoderList[filterIdx-1].id;
	return true;
}

void CSnapshotDlg::OnBnClickSave()
{	
	SStringT sstrFileName;
	std::wstring strFilter;
	CFileDialogEx saveDlg(FALSE, _T("PNG"), CreateNewFileName(sstrFileName), 6, GetEncoderStringFilter(strFilter).c_str());
	
	if (saveDlg.DoModal() == IDOK)
	{
		sstrFileName = saveDlg.m_szFileName;
		CLSID clsId;

		if (GetEncodeCLSID(saveDlg.m_ofn.nFilterIndex,clsId)) {

			SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
			SASSERT(pSnapshot);
			pSnapshot->SaveCapBmpToFile(sstrFileName, clsId);
		}
		EndDialog(IDOK);
	}
}

void CSnapshotDlg::OnBnClickCancel()
{
	EndDialog(IDOK);
}

void CSnapshotDlg::OnBnClickFinish()
{
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SaveCapBmpToClipboard();

	EventSnapshotFinish* pEvt = new EventSnapshotFinish(this);
	SNotifyCenter::getSingleton().FireEventAsync(pEvt);
	pEvt->Release();

	EndDialog(IDOK);
}

void CSnapshotDlg::OnBnClickSmallDot()
{
	SWindow* pOtherAttr = FindChildByName2<SWindow>(L"other_attrbar");
	SASSERT(pOtherAttr);
	SImageButton* pImgBtnSmallDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_smalldot");
	SImageButton* pImgBtnMiddleDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_middledot");
	SImageButton* pImgBtnBigDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_bigdot");
	SASSERT(pImgBtnSmallDot);
	SASSERT(pImgBtnMiddleDot);
	SASSERT(pImgBtnBigDot);
	m_nSelectSize = 1;
	pImgBtnSmallDot->SetCheck(TRUE);
	pImgBtnMiddleDot->SetCheck(FALSE);
	pImgBtnBigDot->SetCheck(FALSE);

	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenSize(m_nSelectSize);
}

void CSnapshotDlg::OnBnClickMiddleDot()
{
	SWindow* pOtherAttr = FindChildByName2<SWindow>(L"other_attrbar");
	SASSERT(pOtherAttr);
	SImageButton* pImgBtnSmallDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_smalldot");
	SImageButton* pImgBtnMiddleDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_middledot");
	SImageButton* pImgBtnBigDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_bigdot");
	SASSERT(pImgBtnSmallDot);
	SASSERT(pImgBtnMiddleDot);
	SASSERT(pImgBtnBigDot);
	m_nSelectSize = 2;
	pImgBtnSmallDot->SetCheck(FALSE);
	pImgBtnMiddleDot->SetCheck(TRUE);
	pImgBtnBigDot->SetCheck(FALSE);

	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenSize(m_nSelectSize);
}

void CSnapshotDlg::OnBnClickBigDot()
{
	SWindow* pOtherAttr = FindChildByName2<SWindow>(L"other_attrbar");
	SASSERT(pOtherAttr);
	SImageButton* pImgBtnSmallDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_smalldot");
	SImageButton* pImgBtnMiddleDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_middledot");
	SImageButton* pImgBtnBigDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_bigdot");
	SASSERT(pImgBtnSmallDot);
	SASSERT(pImgBtnMiddleDot);
	SASSERT(pImgBtnBigDot);
	m_nSelectSize = 5;
	pImgBtnSmallDot->SetCheck(FALSE);
	pImgBtnMiddleDot->SetCheck(FALSE);
	pImgBtnBigDot->SetCheck(TRUE);

	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenSize(m_nSelectSize);
}

void CSnapshotDlg::OnBnClickC1()
{
	COLORREF& color = m_mapColorInfo[1];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(1);
}

void CSnapshotDlg::OnBnClickC2()
{
	COLORREF& color = m_mapColorInfo[2];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(2);
}

void CSnapshotDlg::OnBnClickC3()
{
	COLORREF& color = m_mapColorInfo[3];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(3);
}

void CSnapshotDlg::OnBnClickC4()
{
	COLORREF& color = m_mapColorInfo[4];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(4);
}

void CSnapshotDlg::OnBnClickC5()
{
	COLORREF& color = m_mapColorInfo[5];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(5);
}

void CSnapshotDlg::OnBnClickC6()
{
	COLORREF& color = m_mapColorInfo[6];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(6);
}

void CSnapshotDlg::OnBnClickC7()
{
	COLORREF& color = m_mapColorInfo[7];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(7);
}

void CSnapshotDlg::OnBnClickC8()
{
	COLORREF& color = m_mapColorInfo[8];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(8);
}

void CSnapshotDlg::OnBnClickC9()
{
	COLORREF& color = m_mapColorInfo[9];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(9);
}

void CSnapshotDlg::OnBnClickC10()
{
	COLORREF& color = m_mapColorInfo[10];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(10);
}

void CSnapshotDlg::OnBnClickC11()
{
	COLORREF& color = m_mapColorInfo[11];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(11);
}

void CSnapshotDlg::OnBnClickC12()
{
	COLORREF& color = m_mapColorInfo[12];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(12);
}

void CSnapshotDlg::OnBnClickC13()
{
	COLORREF& color = m_mapColorInfo[13];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(13);
}

void CSnapshotDlg::OnBnClickC14()
{
	COLORREF& color = m_mapColorInfo[14];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(14);
}

void CSnapshotDlg::OnBnClickC15()
{
	COLORREF& color = m_mapColorInfo[15];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(15);
}

void CSnapshotDlg::OnBnClickC16()
{
	COLORREF& color = m_mapColorInfo[16];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(16);
}

void CSnapshotDlg::OnBnClickC17()
{
	COLORREF& color = m_mapColorInfo[17];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(17);
}

void CSnapshotDlg::OnBnClickC18()
{
	COLORREF& color = m_mapColorInfo[18];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(18);
}

void CSnapshotDlg::OnBnClickC19()
{
	COLORREF& color = m_mapColorInfo[19];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(19);
}

void CSnapshotDlg::OnBnClickC20()
{
	COLORREF& color = m_mapColorInfo[20];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);
	SetSelectedColor(20);
}

void CSnapshotDlg::SetSelectedColor(int nIndex)
{
	SWindow* pAttrBar = FindChildByName2<SWindow>(L"attrbar");
	SASSERT(pAttrBar);
	SWindow* pWindow = FindChildByName2<SWindow>(L"selected_window");
	SASSERT(pWindow);
	SWindow* pSelectColor = pWindow->FindChildByName2<SWindow>(L"selected_color");
	SASSERT(pSelectColor);
	SStringW sstrColor;
	sstrColor.Format(L"RGB(%d,%d,%d)", GetRValue(m_mapColorInfo[nIndex]), GetGValue(m_mapColorInfo[nIndex]), GetBValue(m_mapColorInfo[nIndex]));
	pSelectColor->SetAttribute(L"colorBkgnd", sstrColor, FALSE);
}

void CSnapshotDlg::OnClickItem(int nIndex)
{
	SComboView* pWordSizeCbxView = FindChildByName2<SComboView>(L"cbx_wordsize");
	SASSERT(pWordSizeCbxView);
	if (-1 != nIndex)
		pWordSizeCbxView->SetCurSel(nIndex);

 	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
 	SASSERT(pSnapshot);
 	static const int sizelist[6] = {10,12,14,16,18,20};
 	pSnapshot->SetFontSize(sizelist[nIndex]);
}