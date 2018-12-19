#pragma once

#include <interface/render-i.h>
namespace SOUI
{
    class SAniImgFrame
    {
    public:
        CAutoRefPtr<IBitmap> pBmp;
        int                  nDelay;
    };

    class SSkinAni : public SSkinObjBase
    {
        SOUI_CLASS_NAME(SSkinAni,L"skinani")
    public:
        SSkinAni():m_nFrames(0),m_iFrame(0),m_pFrames(NULL)
        {

        }

        virtual ~SSkinAni()
        {
            if(m_pFrames) delete [] m_pFrames;
        }


        /**
        * GetStates
        * @brief    ���GIF֡��
        * @return   int -- ֡��
        * Describe  
        */    
        virtual int GetStates(){return m_nFrames;}

        /**
        * GetSkinSize
        * @brief    ���ͼƬ��С
        * @return   SIZE -- ͼƬ��С
        * Describe  
        */    
        virtual SIZE GetSkinSize()
        {
            SIZE sz={0};
            if(m_nFrames>0 && m_pFrames)
            {
                sz=m_pFrames[0].pBmp->Size();
            }
            return sz;
        }

        /**
        * GetFrameDelay
        * @brief    ���ָ��֡����ʾʱ��
        * @param    int iFrame --  ֡��,Ϊ-1ʱ�����õ�ǰ֡����ʱ
        * @return   long -- ��ʱʱ��(*10ms)
        * Describe  
        */    
        long GetFrameDelay(int iFrame=-1)
        {
            if(iFrame==-1) iFrame=m_iFrame;
            long nRet=-1;
            if(m_nFrames>1 && iFrame>=0 && iFrame<m_nFrames)
            {
                nRet=m_pFrames[iFrame].nDelay;
            }
            return nRet;
        }
        
        IBitmap * GetFrameImage(int iFrame=-1)
        {
            if(iFrame==-1) iFrame=m_iFrame;
            long nRet=-1;
            if(m_nFrames>1 && iFrame>=0 && iFrame<m_nFrames)
            {
                return m_pFrames[iFrame].pBmp;
            }else
            {
                return NULL;
            }
        }


        /**
        * ActiveNextFrame
        * @brief    ������һ֡
        * @return   void 
        * Describe  
        */    
        void ActiveNextFrame()
        {
            if(m_nFrames>1)
            {
                m_iFrame++;
                if(m_iFrame==m_nFrames) m_iFrame=0;
                SelectActiveFrame(m_iFrame);
            }
        }

        /**
        * SelectActiveFrame
        * @brief    ����ָ��֡
        * @param    int iFrame --  ֡��
        * @return   void
        * Describe  
        */    
        void SelectActiveFrame(int iFrame)
        {
            if(m_nFrames>1 && iFrame<m_nFrames)
            {
                m_iFrame = iFrame;
            }
        }

    public:
        virtual int LoadFromFile(LPCTSTR pszFileName){return 0;}
        virtual int LoadFromMemory(LPVOID pData,size_t len){return 0;}
    protected:
        /**
        * Draw
        * @brief    ����ָ��֡��GIFͼ
        * @param    IRenderTarget * pRT --  ����Ŀ��
        * @param    LPCRECT rcDraw --  ���Ʒ�Χ
        * @param    DWORD dwState --  ����״̬�����ﱻ����Ϊ֡��
        * @param    BYTE byAlpha --  ͸����
        * @return   void
        * Describe  
        */    
        virtual void _Draw(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState,BYTE byAlpha=0xFF)
        {
            if(m_nFrames == 0 || !m_pFrames) return;
            if(dwState!=-1) SelectActiveFrame(dwState);
            CRect rcSrc(CPoint(0,0),GetSkinSize());
            pRT->DrawBitmapEx(rcDraw,m_pFrames[m_iFrame].pBmp,rcSrc,EM_STRETCH,byAlpha);
        }

        int m_nFrames;
        int m_iFrame;

        SAniImgFrame * m_pFrames;
    };

}