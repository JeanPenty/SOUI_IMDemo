
// ------------------------------------------------------------------------------
//
// SImRichEdit.h : interface of the SImRichEdit class
//
// ���������RichEdit�Ľӿڣ��Ŀؼ���Ҫ��ms�ṩ��RichEdit��ʵ�������¹���
//
// 1.����
// 2.ͷ��
// 3.����OLE��@�ˡ�ͼƬ���ļ����
// 4.��קЧ��
// 5.�ͼ�����Ľ���
// 
// ------------------------------------------------------------------------------
//
// Ϊ��ʵ�ָ�Ч��GIF����ˢ�£����GIF��ͼƬ���������⴦����ˢ��GIFʱ��
// ������RichEditˢ�£����Ǵ�����ʱDC��Ȼ������OLE��������ʱDC�ϣ����
// ��RichEdit�ϣ�ʵ��Ч��������GIF������£�CPU������4%��
//
// !!!�ر�ע�⣺���ڶ�GIF��ˢ���������⴦������ж���Ŀؼ���Ҫ������
// SimRichEdit���Ϸ�������toast��ʾ�����轫�ÿؼ���ΪSImRichEdit���ӿؼ�!!!
//
// ------------------------------------------------------------------------------
//
// QA/ע������:
// 
// 1. ��RichEdit��readonly������£����ܽ�����������
//
// 2. ���Ҫ��RichEdit��ʧȥ����ʱ��Ҫ��ʾѡ�и�������Ҫ����
//    TXTBIT_SAVESELECTION �Ҳ�������TXTBIT_HIDESELECTION
//
// 3. �������Ķ������һ���ǿ��У���Ҫ�ڸö�������ټ�һ�����У���������
//    ������ʱ����Ҫ�Ѷ���ӵ��Ǹ����а�����ȥ
//
// 4. SetIndents�ȽϷ�ʱ,�����þ�������
//
// 5.
//
// ------------------------------------------------------------------------------

#pragma once

#include <TOM.h>
#include "RichEditObj.h"
#include "RichEditOleBase.h"
#include "IRichEditObjHost.h"
#include "control/SRichEdit.h"
#include "RichEditOleCtrls.h"
#include "atl.mini/SComCli.h"

namespace SOUI
{

    class SImRichEdit : public SRichEdit, public IRichEditObjHost, public ITimelineHandler
    {
        struct UpdateContext
        {
            BOOL bReadOnly;
            BOOL bWordWrap;
            BOOL bHasScrollBar;
        };

        SOUI_CLASS_NAME(SImRichEdit, L"imrichedit");

    public:

        // ------------------------------------------------------------------------------
        //
        // public methods for user
        //
        // ------------------------------------------------------------------------------

        SImRichEdit();
        ~SImRichEdit();

        //
        // IRichEditObjHost methods
        //
        virtual ISwndContainer* GetHostContainer();
        virtual CRect           GetHostRect();
        virtual CRect           GetAdjustedRect();
        virtual int             GetCharCount();
        virtual void            DirectDraw(const CRect& rc);
        virtual void            DelayDraw(const CRect& rc);
        virtual HRESULT         SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *pRet);
        virtual ITextDocument*  GetTextDoc();
        virtual ITextServices*  GetTextServ();
        virtual BOOL            AcceptContent(RichFormatConv* conv);
        virtual bool            NotifyRichObjEvent(RichEditObj* obj, int subEvent, WPARAM wParam, LPARAM lParam);
        virtual void            EnableDraw(BOOL bEnable);
        virtual void            UpdateRichObj(RichEditObj* obj);
        virtual int             GetRemainingLength();
        virtual BOOL            IsEditable();
        virtual void            Activate();
        virtual UINT            InsertContent(LPCWSTR lpszContent, UINT uInsertAt = RECONTENT_LAST);
        virtual SStringW        GetSelectedContent(CHARRANGE* lpchrg/*=NULL*/);
        virtual void            DeleteContent(UINT uIndex);
        virtual UINT            GetContentCount();
        virtual void            Clear();
        virtual void            ScrollToBottom();
        virtual BOOL            IsScrollAtTop();
        virtual BOOL            IsScrollAtBottom();

        //
        // extent methods
        //
        RichEditObj*     GetElementById(LPCWSTR lpszId);
        RichEditObj*     HitTest(POINT ptInControl);
        RichEditContent* GetContent(LPCWSTR pszId);
        RichEditContent* GetContent(UINT uIndex);
        RichEditOleBase* GetOleById(LPCWSTR lpszId);
        RichEditOleBase* GetSelectedOle(int cp = -1);
        UINT             DeleteContent(RichEditContent * prec);
        BOOL             GetCaretRect(CRect& rcCaret);
        BOOL             GetCaretRect2(CRect& rcCaret);
        BOOL             CanPaste();
        void             UpdateBkgndRenderTarget();
        void             MarkBkgndRtDirty();
        void             SetAutoFixVScroll(BOOL fix);
        void             ForceUpdateLayout();
        void             ReLocateContents(UINT start, int offset);
        BOOL             GetContentIndex(LPCWSTR pszId, UINT& index);
        void             UpdateContentPosition(UINT start, UINT end = RECONTENT_LAST);
        void             ScrollTo(LPCWSTR pszId);
        void             ScrollTo(UINT uIndex);

        //
        // richedit message wrapper
        //
        void    SetFontSize(int size);
        void    SetSelectionColor(COLORREF cr);
        void    GetSel(long* pStartCp, long* pEndCp);
        void    SetSel(int nStart, int nEnd);
        BOOL    SetParaFormat(PARAFORMAT2& pf);
        int     LineFromChar(int ncp);
        int     LineIndex(int nLineNum);
        int     LineLength(int nLineCP /* = -1 */);
        void    PosFromChar(UINT ncp, POINT* pPos);
        int     CharFromPos(POINT pt);
        int     GetFirstVisibleLine();
        int     GetRangeText(const CHARRANGE& chr, SStringW& s);
        void    SetAutoVScroll(BOOL bEnabled);
        BOOL    IsAutoVScroll();
        void    SelectAll();
        void    Copy();
        void    Cut();
        void    Paste();
        void    EmptyUndoBuffer();
        BOOL    GetDefCharFormat(CHARFORMAT& cf);
        BOOL    SetDefCharFormat(CHARFORMAT& cf);
    protected:

        // ------------------------------------------------------------------------------
        //
        // internal helpers and event handlers
        //
        // ------------------------------------------------------------------------------

        //
        //caret��ط���
        //

        virtual BOOL CreateCaret(HBITMAP pBmp, int nWid, int nHeight);
        virtual void SetCaretPos(int x, int y);

        //
        // internal helpers
        //

        BOOL    IsRenderTargetEmpty(IRenderTarget* pRt);
        void    EnableDragDrop(BOOL enable);
        RichEditObj* HitTest(RichEditObj* p, POINT ptInControl);
        RichEditContent* CreateRichEditConent(LPCWSTR lpszContent);
        void    PreUpdate(UpdateContext&);
        void    DoneUpdate(const UpdateContext&);
        void    ClipChildren(SWindow * pWnd, IRegion * prgn);
        void    DrawScrollbar(BOOL bVertical, UINT uCode, int nPos);
        void    SetContentsDirty();
        void    DrawVisibleGifs(IRenderTarget * pRt, const CRect& validRgnRect);
        void    DirectDrawOles(IRegion* prgn);
        void    DrawVisibleContents(IRenderTarget * pRt);
        void    UpdateVisibleCharRanges();
        BOOL    RecalcScrollbarPos(BOOL bVertical, UINT uCode, int nPos);

        //
        // overrided methods
        //

        HRESULT     DefAttributeProc(const SStringW & strAttribName, const SStringW & strValue, BOOL bLoading);

        //
        // ITimelineHandler, use to delay draw gifs
        //
        void        OnNextFrame();

        //
        // internal message handlers
        //

        LRESULT    OnCreate(LPVOID);
        LRESULT    OnNcCalcSize(BOOL bCalcValidRects, LPARAM lParam);
        void       OnSize(UINT nType, CSize size);
        void       OnDestroy();
        void       OnPaint(IRenderTarget * pRT);
        void       OnLButtonDown(UINT nFlags, CPoint point);
        void       OnLButtonDblClick(UINT nFlags, CPoint point);
        void       OnLButtonUp(UINT nFlags, CPoint point);
        void       OnRButtonDown(UINT nFlags, CPoint point);
        void       OnRButtonUp(UINT nFlags, CPoint point);
        void       OnMouseMove(UINT nFlags, CPoint point);
        BOOL       OnSetCursor(const CPoint &pt);
        BOOL       OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
        BOOL       OnUpdateToolTip(CPoint pt, SwndToolTipInfo &tipInfo);
        BOOL       OnScroll(BOOL bVertical, UINT uCode, int nPos);
        //---------------------------------
        LRESULT	   OnImeStartComposition(UINT uMsg, WPARAM wParam, LPARAM lParam);
        LRESULT	   OnImeComposition(UINT uMsg, WPARAM wParam, LPARAM lParam);
        SOUI_MSG_MAP_BEGIN()
            MSG_WM_CREATE(OnCreate)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_PAINT_EX(OnPaint)
            MSG_WM_SIZE(OnSize)
            MSG_WM_NCCALCSIZE(OnNcCalcSize)
            MSG_WM_LBUTTONDOWN(OnLButtonDown)
            MSG_WM_LBUTTONUP(OnLButtonUp)
            MSG_WM_RBUTTONDOWN(OnRButtonDown)
            MSG_WM_RBUTTONUP(OnRButtonUp)
            MSG_WM_LBUTTONDBLCLK(OnLButtonDblClick)
            MSG_WM_MOUSEWHEEL(OnMouseWheel)
            MSG_WM_MOUSEMOVE(OnMouseMove)
            MESSAGE_HANDLER_EX(WM_IME_STARTCOMPOSITION, OnImeStartComposition)
            MESSAGE_HANDLER_EX(WM_IME_COMPOSITION, OnImeComposition)
            SOUI_MSG_MAP_END()

    private:
        BOOL _onPostImmComposition(UINT msg, WPARAM wParam, LPARAM lParam);
    private:

        typedef SArray<RichEditContent*> RichContentArray;

        CRect                       _caretRect;         // �����richedit���ϽǵĹ��λ�á���¼����TxSetCaretPosʱ��λ�ã�
                                                        // ���������ٻ�ù��ĸ߶�
        CHARRANGE                   _visibleChr;        // �ɼ����ַ���Χ
        CHARRANGE                   _visibleOleChr;     // �ɼ���OLE��Χ
        CAutoRefPtr<IRenderTarget>  _pBackgroundRt;     // ����rt
        BOOL                        _isBackgroundDirty; // ���_pBackgroundRt�Ƿ�ʧЧ
        ITextDocument*              _pTextDoc;
        RichContentArray            _richContents;      // richedit��ʾ������
        RichEditObj*                _pLastHoverObj;     // ������������һ��obj
        time_t                      _lastDrawTicks;     // ��¼���һ��ˢ�µ�ʱ�䣬�����߶�ʱˢ�£�60ticksˢһ��
        CAutoRefPtr<IRegion>        _pDelayDrawRgn;     // ������
        CRect                       _originalInset;     // ԭʼ���õ��ڱ߾�
        BOOL                        _isDrawEnable;      // �ܷ�ˢ��UI
        BOOL                        _scrollbarOutdated; // ��ǹ������Ƿ���Ҫ������һ�Σ���ϸ�����OnScroll����
        BOOL                        _scrollbarAtBottom; // ��ǹ������Ƿ��ڵײ�
        BOOL                        _isDragging;        // ����Ƿ�����ק״̬
        BOOL                        _readOnlyBeforeDrag;// ��קǰ��ֻ��״̬
        BOOL                        _fixVScroll;        // ��̬ˢ�´�ֱ��������ȷ���������һƬ�հ׵����
        bool						_isCreatIme;		//����Ƿ���Ҫ�������뷨��������������λ��Ϊfalse
    };

}// namespace SOUI
