#include "stdafx.h"
#include "SSplitBar.h"
namespace SOUI
{

    SSplitBar::SSplitBar() :m_bDragging(FALSE)
        , m_bResizeHostWnd(FALSE)
        , m_nSizeMin(0)
        , m_nSizeMax(0)
        , m_bVertical(FALSE)
    {

    }

    SSplitBar::~SSplitBar()
    {

    }

    LRESULT SSplitBar::OnCreate(LPVOID)
    {
        if (0 != __super::OnCreate(NULL)) return 1;
        if (m_bVertical)
        {
            m_nOrginPos = static_cast<SouiLayoutParamStruct*>(GetLayoutParam()->GetRawData())->posLeft.nPos;
        }
        else
        {
            m_nOrginPos = static_cast<SouiLayoutParamStruct*>(GetLayoutParam()->GetRawData())->posTop.nPos;
        }
        m_nTrackingPos = m_nOrginPos;

        return 0;
    }

    BOOL SSplitBar::OnSetCursor(const CPoint &pt)
    {
        HCURSOR hCursor = GETRESPROVIDER->LoadCursor(m_bVertical ? IDC_SIZEWE : IDC_SIZENS);
        SetCursor(hCursor);
        return TRUE;
    }

    void SSplitBar::OnLButtonDown(UINT nFlags, CPoint pt)
    {
        SWindow::OnLButtonDown(nFlags, pt);

        m_ptDragPrev = pt;
        m_bDragging = TRUE;
    }

    void SSplitBar::OnLButtonUp(UINT nFlags, CPoint pt)
    {
        SWindow::OnLButtonUp(nFlags, pt);

        m_bDragging = FALSE;
        if (m_bVertical)
        {
            m_nOrginPos = static_cast<SouiLayoutParamStruct*>(GetLayoutParam()->GetRawData())->posLeft.nPos;
        }
        else
        {
            m_nOrginPos = static_cast<SouiLayoutParamStruct*>(GetLayoutParam()->GetRawData())->posTop.nPos;
        }
        m_nTrackingPos = m_nOrginPos;
    }

    void SSplitBar::OnMouseMove(UINT nFlags, CPoint pt)
    {
        SWindow::OnMouseMove(nFlags, pt);

        static int nLastUpdateTicks = 0;

        int now = GetTickCount();
        if (now - nLastUpdateTicks < 30 || !m_bDragging)
        {
            return;
        }
        nLastUpdateTicks = now;

        int nWindowOffset = 0;
        int nOffset = 0;

        // ������������λ��ƫ��

        if (m_bVertical)
            nOffset = pt.x - m_ptDragPrev.x;
        else
            nOffset = pt.y - m_ptDragPrev.y;

        // ����ָ����µ�λ��

        //SwndLayout * pLayout = GetLayout();
        SouiLayoutParam *pSouiLayoutParam = GetLayoutParamT<SouiLayoutParam>();
        SouiLayoutParamStruct *pLayout = (SouiLayoutParamStruct*)pSouiLayoutParam->GetRawData();

        POS_INFO tempPi;
        if (m_bVertical)
        {
            tempPi = pLayout->posLeft;
        }
        else
        {
            tempPi = pLayout->posTop;
        }
        int nNewPos = m_nOrginPos.toPixelSize(GetScale()) + nOffset * tempPi.cMinus;

        /*
         *  - ��һ�����Ҫ���⴦��:��Ҫ�޸�hostwnd�ĳߴ�,top/left������-XXX�ķ�ʽ���塣
         *    ������߼��Ƚϸ��ӣ����Լ�Ҳ��ú���@_@
         *
         *  - ��Ϊ����Ҫ�޸��������ڵ�����£�left/top�����right/bottomΪê�㣬���޸���hostwnd�ĳߴ��,
         *    ����Ҫ���޸�λ����Ϣ���������߼���λ����Ϣ�ᱻ���Ӽ��㡣
         *    �ٸ����ӣ�����ԭʼ��y posλ����-100�����ڵĸ߶�Ϊ300����ʱy��λ����200(��Եײ�����)
         *    �϶��ָ��������ƶ���5��px�����λ����Ϣ���ĳ���-105�����ڼ�����5px�������295�����������
         *    ��λ�ñ����190
         *
         *  - ����������£�����maxsize��minisize�Ŀ��ơ���Ϊ�ָ�����bottom/right�����λ�ò��䣬���Դ�
         *    ʹ�õĽǶȽ�����û�г��� [maxsize,minisize] �ķ�Χ��
        */

        HWND hWnd = GetContainer()->GetHostHwnd();
        BOOL bZoomed = ::IsZoomed(hWnd);
        BOOL bResizeWnd = m_bResizeHostWnd && !bZoomed;
        if (!(bResizeWnd && tempPi.cMinus < 0))
        {
            if (nNewPos > m_nSizeMax)
                nNewPos = m_nSizeMax;

            if (nNewPos < m_nSizeMin)
                nNewPos = m_nSizeMin;

            if (nNewPos == tempPi.nPos.toPixelSize(GetScale()))
                return;

            tempPi.nPos.fSize = nNewPos;
        }

        // ��������

        nWindowOffset = nNewPos - m_nTrackingPos.toPixelSize(GetScale());
        nWindowOffset *= tempPi.cMinus;
        m_nTrackingPos.fSize = nNewPos;

        if (bResizeWnd)
            ResizeHostWindow(nWindowOffset);
        else
            RequestRelayout();
    }

    void SSplitBar::ResizeHostWindow(int nOffset)
    {
        if (m_bResizeHostWnd && nOffset != 0)
        {
            HWND hWnd = GetContainer()->GetHostHwnd();

            CRect rcWnd;
            ::GetWindowRect(hWnd, rcWnd);

            if (m_bVertical)
            {
                ::MoveWindow(hWnd,
                    rcWnd.left,
                    rcWnd.top,
                    rcWnd.Width() + nOffset,
                    rcWnd.Height(),
                    TRUE);
            }
            else
            {
                ::MoveWindow(hWnd,
                    rcWnd.left,
                    rcWnd.top,
                    rcWnd.Width(),
                    rcWnd.Height() + nOffset,
                    TRUE);
            }
        }
    }

}//namespace SOUI
