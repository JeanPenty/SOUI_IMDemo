#include "stdafx.h"
#include "souistd.h"
#include "RichEditOleCallback.h"
#include "dataobject.h"
#include "ClipboardConverter.h"
#include "SImRichEdit.h"
#include "helper\SMenuEx.h"

namespace SOUI
{

    //////////////////////////////////////////////////////////////////////////
    // RichEditOleCallback

    RichEditOleCallback::RichEditOleCallback(SImRichEdit * pHost)
        :m_dwRef(1)
        , m_iStorage(0)
    {
        HRESULT hResult = ::StgCreateDocfile(NULL,
            STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_DELETEONRELEASE,
            0, &m_stg);

        if (m_stg == NULL || hResult != S_OK)
        {
            //         AfxThrowOleException( hResult );
        }

        m_pHost = pHost;
    }

    RichEditOleCallback::~RichEditOleCallback()
    {
    }

    HRESULT RichEditOleCallback::GetNewStorage(LPSTORAGE* ppStg)
    {
        WCHAR tName[150];
        swprintf(tName, L"REStorage_%d", ++m_iStorage);

        if (m_iStorage % 100 == 0)
        {//ÿ100�������ύһ��,���ⴴ��stream or storage�����ڴ治���ʧ��
            m_stg->Commit(STGC_DEFAULT);
        }

        HRESULT hr = m_stg->CreateStorage(tName,
            STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE,
            0, 0, ppStg);
        if (FAILED(hr) && (hr & E_OUTOFMEMORY))
        {//ʧ�ܺ���storage�ύ������
            m_stg->Commit(STGC_DEFAULT);
            hr = m_stg->CreateStorage(tName,
                STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE,
                0, 0, ppStg);
        }

        return hr;
    }

    HRESULT RichEditOleCallback::QueryInterface(REFIID iid, void ** ppvObject)
    {
        HRESULT hr = S_OK;
        *ppvObject = NULL;

        if (iid == IID_IUnknown ||
            iid == IID_IRichEditOleCallback)
        {
            *ppvObject = this;
            AddRef();
        }
        else
        {
            hr = E_NOINTERFACE;
        }

        return hr;
    }

    ULONG RichEditOleCallback::AddRef()
    {
        return ++m_dwRef;
    }

    ULONG RichEditOleCallback::Release()
    {
        if (--m_dwRef == 0)
        {
            delete this;
            return 0;
        }

        return m_dwRef;
    }

    HRESULT RichEditOleCallback::GetInPlaceContext(
        LPOLEINPLACEFRAME FAR *lplpFrame,
        LPOLEINPLACEUIWINDOW FAR *lplpDoc,
        LPOLEINPLACEFRAMEINFO lpFrameInfo)
    {
        return S_OK;
    }

    HRESULT RichEditOleCallback::ShowContainerUI(BOOL fShow)
    {
        return S_OK;
    }

    HRESULT RichEditOleCallback::QueryInsertObject(
        LPCLSID lpclsid,
        LPSTORAGE lpstg,
        LONG cp)
    {
        if (lpclsid == NULL)
        {
            return E_INVALIDARG;
        }

        if (*lpclsid == IID_ImageOleCtrl ||
            *lpclsid == IID_FetchMoreOleCtrl ||
            *lpclsid == IID_SeparatorBarCtrl ||
            *lpclsid == IID_RichMetaFileOle ||
            *lpclsid == IID_RemainderOleCtrl )
        {
            return S_OK;
        }

        return E_FAIL;
    }

    HRESULT RichEditOleCallback::DeleteObject(LPOLEOBJECT lpoleobj)
    {
        return S_OK;
    }

    HRESULT RichEditOleCallback::GetClipboardData(
        CHARRANGE FAR *lpchrg,
        DWORD reco,
        LPDATAOBJECT FAR *lplpdataobj)
    {
        if (RECO_COPY != reco && RECO_CUT != reco)
        {
            return E_NOTIMPL;
        }

        SStringW str = m_pHost->GetSelectedContent(lpchrg);

        RichFormatConv conv;
        if (!conv.InitFromRichContent(str))
        {
            return E_NOTIMPL;
        }

        if (conv.ToDataObject(lplpdataobj))
        {
            return S_OK;
        }

        return E_NOTIMPL;
    }

    /*
     * Notes:
     *  - During a paste operation or a drag event, determines if the data that is pasted or dragged should be accepted.
     *
     * Parameters:
     * @lpdataobj:
     *  - The data object being pasted or dragged.
     *
     * @lpcfFormat:
     *  - The clipboard format that will be used for the paste or drop operation.
     *    If the value pointed to by lpcfFormat is zero, the best available format
     *    will be used. If the callback changes the value pointed to by lpcfFormat,
     *    the rich edit control only uses that format and the operation will fail
     *    if the format is not available.
     *
     * @reco:
     *  - A clipboard operation flag, which can be one of these values.
     *    RECO_DROP: Drop operation (drag-and-drop).
     *    RECO_PASTE: Paste from the clipboard.
     *
     * @fReally:
     *  - Indicates whether the drag-drop is actually happening or if it is just a query.
     *    A nonzero value indicates the paste or drop is actually happening.
     *    A zero value indicates the operation is just a query, such as for EM_CANPASTE.
     *
     * @hMetaPict:
     *  - Handle to a metafile containing the icon view of an object if DVASPECT_ICON
     *    is being imposed on an object by a paste special operation.
     *
     * Returen Value:
     *  - �ɹ�����S_OK�����Remarks.
     *
     * Remarks:
     *  - �������ʧ�ܣ�richedit��ܾ���ε����ݲ���������ֹ��ק/ճ������������
     *    richedit����lpdataobj��������Է��ʵ����ݸ�ʽ������ճ����
     *  - �������һ����S_OK�ĳɹ���,��ζ��callback��Ҫ�Լ���������ܷ��ճ��(fReally��FALSE�����)��
     *    ������Ҫ�Լ���ճ��(fReally��TRUE�����)��
     *  - �������һ����S_OK�ĳɹ���,richedit������ؼ���read-only״̬
    */

    HRESULT RichEditOleCallback::QueryAcceptData(
        LPDATAOBJECT lpdataobj,
        CLIPFORMAT FAR *lpcfFormat,
        DWORD reco,
        BOOL fReally,
        HGLOBAL hMetaPict)
    {
        if (!fReally)
        {
            return OLE_S_STATIC;
        }

        RichFormatConv conv;
        if (conv.InitFromDataObject(lpdataobj) != 0)
        {
            //SStringW strContent;
            //conv.ToRichContent(strContent);
            //m_pHost->InsertContent(strContent, RECONTENT_CARET);
            //m_pHost->SetFocus();

            if (!m_pHost->AcceptContent(&conv) && !m_pHost->GetReadOnly())
            {
                SStringW strContent;
                conv.ToRichContent(strContent);
                m_pHost->InsertContent(strContent, RECONTENT_CARET);
                m_pHost->SetFocus();
            }

            /*
             * �����Լ��Ѿ������ճ��/��ק��������ϣ��richedit�����ٽ���һ��
             *
             * A success code other than S_OK means that the callback either checked the data
             * itself (if fReally is FALSE) or imported the data itself (if fReally is TRUE).
            */
            return S_FALSE;
        }

        return E_NOTIMPL;
    }

    HRESULT RichEditOleCallback::ContextSensitiveHelp(BOOL fEnterMode)
    {
        return E_NOTIMPL;
    }

    HRESULT RichEditOleCallback::GetDragDropEffect(
        BOOL fDrag,
        DWORD grfKeyState,
        LPDWORD pdwEffect)
    {
        return S_OK;
    }

    HRESULT RichEditOleCallback::GetContextMenu(
        WORD seltyp,
        LPOLEOBJECT lpoleobj,
        CHARRANGE FAR *lpchrg,
        HMENU FAR *lphmenu)
    {
        return S_OK;
    }

}; // namespace SOUI