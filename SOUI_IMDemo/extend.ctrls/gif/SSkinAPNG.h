/********************************************************************
created:	2014/10/01
created:	01:10:2014   16:32
filename: 	DuiSkinAPNG.h
file base:	DuiSkinAPNG
file ext:	h
author:		huangjianxiong

purpose:	����PNG��APNG����չ֧�֣������ڵ�ǰʹ�õ�ͼƬ��������
            Ŀǰֻ��imgdecoder-png֧��APNG����
*********************************************************************/
#pragma once
#include <interface/SSkinobj-i.h>
#include <unknown/obj-ref-impl.hpp>

#include "SAniImgFrame.h"

namespace SOUI
{

    /**
    * @class     SSkinAPNG
    * @brief     APNGͼƬ���ؼ���ʾ����
    * 
    * Describe
    */
    class SSkinAPNG : public SSkinAni
    {
        SOUI_CLASS_NAME(SSkinAPNG, L"apng")
    public:
        SSkinAPNG()
        {

        }

        
        /**
         * LoadFromFile
         * @brief    ���ļ�����APNG
         * @param    LPCTSTR pszFileName --  �ļ���
         * @return   int -- APNG֡����0-ʧ��
         * Describe  
         */    
        int LoadFromFile(LPCTSTR pszFileName);

        /**
         * LoadFromMemory
         * @brief    ���ڴ����APNG
         * @param    LPVOID pBits --  �ڴ��ַ
         * @param    size_t szData --  �ڴ����ݳ���
         * @return   int -- APNG֡����0-ʧ��
         * Describe  
         */    
        int LoadFromMemory(LPVOID pBits,size_t szData);

        SOUI_ATTRS_BEGIN()
            ATTR_CUSTOM(L"src",OnAttrSrc)   //XML�ļ���ָ����ͼƬ��Դ��,(type:name)
        SOUI_ATTRS_END()
    protected:
        LRESULT OnAttrSrc(const SStringW &strValue,BOOL bLoading);
        
        int _InitImgFrame(IImgX *pImgX);

    };
}//end of name space SOUI
