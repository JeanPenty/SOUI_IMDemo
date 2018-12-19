#pragma once
#include <core/swnd.h>

namespace SOUI
{
    //
    // ͼƬ��ʾ�ؼ�������GIF
    //

    class SImageView : public SWindow, public ITimelineHandler
    {
        SOUI_CLASS_NAME(SImageView, L"image")   //����GIF�ؼ���XM�ӵı�ǩ

    public:

        SImageView();
        ~SImageView();

        BOOL        ShowImageFile(LPCTSTR pszFileName);
        BOOL        IsPlaying();
        int         GetFrameCount();
        SIZE        GetImageSize();
        void        SetImageSize(SIZE size);
        void        Pause();
        void        Resume();
        void        ShowFrame(int frame, BOOL update = FALSE);
        SStringW    GetRealPath();
        void        SetSkin(ISkinObj* pSkin);
        ISkinObj*   GetSkin();

    protected:

        //SWindow���麯��
        virtual CSize GetDesiredSize(LPCRECT pRcContainer);

        //ITimerLineHander
        virtual void OnNextFrame();

    protected:

        int  GetFrameDelay();

        HRESULT OnAttrSkin(const SStringW & strValue, BOOL bLoading);

        SOUI_ATTRS_BEGIN()
            ATTR_CUSTOM(L"skin", OnAttrSkin)
            ATTR_INT(L"autoPlay", _isPlaying, FALSE)
            SOUI_ATTRS_END()

    protected:

        void OnPaint(IRenderTarget *pRT);
        void OnShowWindow(BOOL bShow, UINT nStatus);
        void OnDestroy();

        //SOUI�ؼ���Ϣӳ���
        SOUI_MSG_MAP_BEGIN()
            MSG_WM_PAINT_EX(OnPaint)    //���ڻ�����Ϣ
            MSG_WM_SHOWWINDOW(OnShowWindow)//������ʾ״̬��Ϣ
            MSG_WM_DESTROY(OnDestroy)
            SOUI_MSG_MAP_END()

    protected:

        ISkinObj *  _pImageSkin;
        BOOL        _isPlaying;
        int	        _currentFrame;
        int         _nextFrameInterval;
        SIZE        _imageSize;
        SStringW    _realPath;      /*< ������ʾ��ͼƬ·�� */
    };
}