#pragma once
#include "control\SCmnCtrl.h"

namespace SOUI
{
    class SButtonEx : public SButton
    {
        SOUI_CLASS_NAME(SButtonEx, L"buttonex")
    public:
        SButtonEx(void);
        ~SButtonEx(void);

    protected:
        void OnPaint(IRenderTarget *pRT);
        void DrawSkin(ISkinObj * pSkin, IRenderTarget *pRT);
        virtual void DrawText(IRenderTarget *pRT, LPCTSTR pszBuf, int cchText, LPRECT pRect, UINT uFormat);

        SOUI_MSG_MAP_BEGIN()
            MSG_WM_PAINT_EX(OnPaint)
            SOUI_MSG_MAP_END()

            HRESULT OnAttrImage(const SStringW & strValue, BOOL bLoading);

        SOUI_ATTRS_BEGIN()
            ATTR_SKIN(L"bkgndSkin", m_pbkgndSkin, TRUE)
            ATTR_SIZE(L"textShift", m_sizeTextShift, TRUE) // ���ʱ�ı��Ƿ�������ƫ��
            SOUI_ATTRS_END()

            SIZE       m_sizeTextShift;
        ISkinObj * m_pbkgndSkin;
    };
};