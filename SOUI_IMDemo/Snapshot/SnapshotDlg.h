#pragma once

#include "SSnapshotCtrl.h"
#include "IWordSizeAdapterCallback.h"

#include "../EventsDefine.h"

class CSnapshotDlg : public SHostDialog
 	, public IWordSizeAdapterCallback
{

	struct EncoderInf {
		EncoderInf(std::wstring _name, CLSID _id):name(_name), id(_id){}
		EncoderInf() {}
		std::wstring name;
		CLSID id;
	};
public:	
	~CSnapshotDlg(void);
	CSnapshotDlg(void);

public:
	virtual void OnClickItem(int nIndex);	
protected:
	SStringT & CreateNewFileName(SStringT &filename);

	void EnumEncoder(std::vector<EncoderInf>& encoderList);

	std::wstring &GetEncoderStringFilter(std::wstring &strFilter);

	bool GetEncodeCLSID(int filterIdx, CLSID & clsId);

	bool OnEventCapturing(EventCapturing* pEvt);
	bool OnEventReCaptured(EventReCap* pEvt);
	bool OnEventRectMoving(EventRectMoving* pEvt);
	bool OnEventRectCaptured(EventRectCaptured* pEvt);
	bool OnEventRectDbClk(EventRectDbClk* pEvt);

	void OnBnClickRect();
	void OnBnClickEllipse();
	void OnBnClickArrow();
	void OnBnClickDoodle();
	void OnBnClickMask();
	void OnBnClickWord();
	void OnBnClickRevoke();
	void OnBnClickCopy();
	
	void OnBnClickSave();
	void OnBnClickCancel();
	void OnBnClickFinish();

	void OnBnClickSmallDot();
	void OnBnClickMiddleDot();
	void OnBnClickBigDot();

	void OnBnClickC1();
	void OnBnClickC2();
	void OnBnClickC3();
	void OnBnClickC4();
	void OnBnClickC5();
	void OnBnClickC6();
	void OnBnClickC7();
	void OnBnClickC8();
	void OnBnClickC9();
	void OnBnClickC10();
	void OnBnClickC11();
	void OnBnClickC12();
	void OnBnClickC13();
	void OnBnClickC14();
	void OnBnClickC15();
	void OnBnClickC16();
	void OnBnClickC17();
	void OnBnClickC18();
	void OnBnClickC19();
	void OnBnClickC20();
protected:
	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(L"btn_rect", OnBnClickRect)
		EVENT_NAME_COMMAND(L"btn_ellipse", OnBnClickEllipse)
		EVENT_NAME_COMMAND(L"btn_arrow", OnBnClickArrow)
		EVENT_NAME_COMMAND(L"btn_doodle", OnBnClickDoodle)
		EVENT_NAME_COMMAND(L"btn_mask", OnBnClickMask)
		EVENT_NAME_COMMAND(L"btn_word", OnBnClickWord)
		EVENT_NAME_COMMAND(L"btn_revoke", OnBnClickRevoke)
		EVENT_NAME_COMMAND(L"btn_copy", OnBnClickCopy)
		EVENT_NAME_COMMAND(L"btn_save", OnBnClickSave)
		EVENT_NAME_COMMAND(L"btn_cancel", OnBnClickCancel)
		EVENT_NAME_COMMAND(L"btn_finish", OnBnClickFinish)
		EVENT_NAME_COMMAND(L"btn_smalldot", OnBnClickSmallDot)
		EVENT_NAME_COMMAND(L"btn_middledot", OnBnClickMiddleDot)
		EVENT_NAME_COMMAND(L"btn_bigdot", OnBnClickBigDot)
		EVENT_NAME_COMMAND(L"btn_c1", OnBnClickC1)
		EVENT_NAME_COMMAND(L"btn_c2", OnBnClickC2)
		EVENT_NAME_COMMAND(L"btn_c3", OnBnClickC3)
		EVENT_NAME_COMMAND(L"btn_c4", OnBnClickC4)
		EVENT_NAME_COMMAND(L"btn_c5", OnBnClickC5)
		EVENT_NAME_COMMAND(L"btn_c6", OnBnClickC6)
		EVENT_NAME_COMMAND(L"btn_c7", OnBnClickC7)
		EVENT_NAME_COMMAND(L"btn_c8", OnBnClickC8)
		EVENT_NAME_COMMAND(L"btn_c9", OnBnClickC9)
		EVENT_NAME_COMMAND(L"btn_c10", OnBnClickC10)
		EVENT_NAME_COMMAND(L"btn_c11", OnBnClickC11)
		EVENT_NAME_COMMAND(L"btn_c12", OnBnClickC12)
		EVENT_NAME_COMMAND(L"btn_c13", OnBnClickC13)
		EVENT_NAME_COMMAND(L"btn_c14", OnBnClickC14)
		EVENT_NAME_COMMAND(L"btn_c15", OnBnClickC15)
		EVENT_NAME_COMMAND(L"btn_c16", OnBnClickC16)
		EVENT_NAME_COMMAND(L"btn_c17", OnBnClickC17)
		EVENT_NAME_COMMAND(L"btn_c18", OnBnClickC18)
		EVENT_NAME_COMMAND(L"btn_c19", OnBnClickC19)
		EVENT_NAME_COMMAND(L"btn_c20", OnBnClickC20)
	EVENT_MAP_END()

protected:
	BOOL OnInitDialog(HWND wnd, LPARAM lInitParam);
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnRButtonUp(UINT nFlags, CPoint point);

	BEGIN_MSG_MAP_EX(CSnapshotDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_KEYDOWN(OnKeyDown)
		MSG_WM_RBUTTONUP(OnRButtonUp)
		CHAIN_MSG_MAP(SHostDialog)
	END_MSG_MAP()

private:
	std::map<int, COLORREF>	m_mapColorInfo;
	int m_nSelectSize;	
private:
	void SetSelectedColor(int nIndex);
	std::vector<EncoderInf> m_encoderList;
};
