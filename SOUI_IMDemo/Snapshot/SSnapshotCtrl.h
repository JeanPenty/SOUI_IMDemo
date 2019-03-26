#pragma once
#include <core/swnd.h>
#include "CEdit9527.h"


class EventCapturing : public TplEventArgs<EventCapturing>
{
	SOUI_CLASS_NAME(EventCapturing, L"on_capturing")
public:
	EventCapturing(SObject *pSender)
		: TplEventArgs<EventCapturing>(pSender) {}
	enum { EventID = EVT_EXTERNAL_BEGIN + 51 };
	SOUI::CPoint pt;
};

class EventRectMoving : public TplEventArgs<EventRectMoving>
{
	SOUI_CLASS_NAME(EventRectMoving, L"on_rectmoving")
public:
	EventRectMoving(SObject *pSender)
		: TplEventArgs<EventRectMoving>(pSender) {}
	enum { EventID = EVT_EXTERNAL_BEGIN + 52 };
	SOUI::CPoint pt;
};

class EventRectCaptured : public TplEventArgs<EventRectCaptured>
{
	SOUI_CLASS_NAME(EventRectCaptured, L"on_rectcaptured")
public:
	EventRectCaptured(SObject *pSender)
		: TplEventArgs<EventRectCaptured>(pSender) {}
	enum { EventID = EVT_EXTERNAL_BEGIN + 53 };
	SOUI::CPoint pt;
};

class EventRectDbClk : public TplEventArgs<EventRectDbClk>
{
	SOUI_CLASS_NAME(EventRectDbClk, L"on_recvdbclk")
public:
	EventRectDbClk(SObject* pSender)
		: TplEventArgs<EventRectDbClk>(pSender) {}
	enum { EventID = EVT_EXTERNAL_BEGIN + 54 };
};

class EventReCap : public TplEventArgs<EventReCap>
{
	SOUI_CLASS_NAME(EventReCap, L"on_recap")
public:
	EventReCap(SObject* pSender)
		: TplEventArgs<EventReCap>(pSender) {}
	enum { EventID = EVT_EXTERNAL_BEGIN + 55 };
};

class SSnapshotCtrl : public SWindow,public IEditHost
{
	SOUI_CLASS_NAME(SSnapshotCtrl, L"SnapshotCtrl")
public:
	SSnapshotCtrl(void);
	~SSnapshotCtrl(void);

	enum EcPosType
	{
		Null = -1,
		TopLeft,
		TopCenter,
		TopRight,
		RightCenter,
		BottomRight,
		BottomCenter,
		BottomLeft,
		LeftCenter,
		SelectRect,
		Opt1,
		Opt2,
		Opt3
	};

public:
	virtual bool canProcessMsg();
	void SetBmpResource(CBitmap* pBmp);
	void SetScreenSize(int nScreenX, int nScreenY);	
	void SaveCapBmpToClipboard();
	void SaveCapBmpToFile(LPCTSTR wstrSavePath, CLSID &encoderClsid);
	int GetEncoderClsid(const WCHAR * format, CLSID * pClsid);
	void SetOperateType(int nOperateType = -1);
	void SetPenColor(const COLORREF& color);
	void SetPenSize(int nPenSize = 1);
	void SetFontSize(int size);
	SOUI::CRect GetCapRect() { return m_rcCapture; }
	//增加一个方法来获取edit可以使用的最大大小
	int GetEtMaxWid(CRect &etRc);
	int GetEtMaxWid(int etLeft);
	int GetEtMaxHei(CRect &etRc);
	int GetEtMaxHei(int  etTop);
	void GetEtMovePos(CPoint &etPos, int etWid, int etHei);
	void RevokeOperate();
	bool CanRevokeOperate();
protected:
	void OnPaint(IRenderTarget *pRT);	

	void OnLButtonDown(UINT nFlags, SOUI::CPoint point);
	void OnLButtonUp(UINT nFlags, SOUI::CPoint point);
	void OnLButtonDblClk(UINT nFlags, SOUI::CPoint point);
	void OnMouseMove(UINT nFlags, SOUI::CPoint point);
	virtual BOOL OnSetCursor(const CPoint &pt);
private:
	void CalcGrayAreaRect(SOUI::CRect rcArea,
		RectF& rcLeft,
		RectF& rcTop,
		RectF& rcRight,
		RectF& rcBottom);
	void CalcPos();
	EcPosType HitPos(const SOUI::CPoint& pt);	
	HBITMAP CopyCurBitmap(int nx, int ny, int nWidth, int nHeight);
	void DrawRectangle(IRenderTarget *pRT, const SOUI::CRect& rcRectangle);
	void DrawEllipse(IRenderTarget* pRT, const SOUI::CRect& rcEllipse);
	void DrawArrow(IRenderTarget* pRT, const SOUI::CPoint& point);
	void LineArrow(HDC pDC, SOUI::CPoint p1, SOUI::CPoint p2, double theta, double alpha, int length, COLORREF clr, int size);
	void DrawDoodle(IRenderTarget* pRT, const std::vector<SOUI::CPoint> vecPoints);
	void DrawMask(IRenderTarget* pRT, const std::vector<Gdiplus::Point> vecPoints);
protected:
	SOUI_MSG_MAP_BEGIN()
		MSG_WM_PAINT_EX(OnPaint)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
		MSG_WM_MOUSEMOVE(OnMouseMove)
	SOUI_MSG_MAP_END()

private:
	CBitmap *					m_pBitmap;
	Gdiplus::Bitmap*			m_MaskBitmap;
	int							m_nScreenX, m_nScreenY;
	std::vector<CBitmap*>		m_vecBitmap;

	SOUI::CPoint				m_ptDown;			//鼠标按下的位置
	SOUI::CPoint				m_pt;
	std::vector<SOUI::CPoint>	m_vecDoodlePoints;
	std::vector<Gdiplus::Point>	m_vecMaskPoints;

	bool						m_bSelected;
	bool						m_bSelectOperate;

	SOUI::CRect					m_rcCapture;

	HCURSOR						m_hCurSelect;
	HCURSOR						m_hCurLeft, m_hCurRight, m_hCurTop, m_hCurBottom,
								m_hCurTopLeft, m_hCurTopRight, m_hCurBottomLeft, m_hCurBottomRight,
								m_hCurHand, m_hCurArrow;
	HCURSOR						m_hCurOpt1, m_hCurOpt2, m_hCurOpt3[3];
	SOUI::CRect					m_rcPos[8];
	COLORREF					m_crBorder;
	EcPosType					m_eDraging;

	int							m_nOperateType;
	int							m_nPenSize;
	COLORREF					m_crPen;
	bool						m_bDrawOperate;
	SOUI::CRect					m_rcRectangle;
	SOUI::CRect					m_rcEllipse;
	CPoint						m_clickPoint;
	BOOL						m_ClickTwo;
	int							m_FontSize;
};
