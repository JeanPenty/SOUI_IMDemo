#pragma once
#include "core/SWnd.h"

namespace SOUI
{
    class SWindowEx : public SWindow
    {
    public:
        SOUI_CLASS_NAME(SWindowEx, L"windowex")

    protected:
        SOUI_MSG_MAP_BEGIN()
            MSG_WM_LBUTTONDBLCLK(OnLButtonDown) //��˫����Ϣ����Ϊ����
            SOUI_MSG_MAP_END()
    };

};