/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       FileHelper.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/08/03
* 
* Describe    ʵ�������ļ���ص�helper
*/

#pragma once

#include <commdlg.h>
/**
* BuildFilePath
* @brief    �ݹ鴴����Ŀ¼
* @param    LPCTSTR pszCurPath --  ����·������ʼλ��
* @param    LPCTSTR pszNewPath --  ��·��
* @param    BOOL bPath --  pszNewPath��һ��·����ʶ
* @return   BOOL -- true�����ɹ�
* Describe  pszNewPathָ��һ���ļ���ʱ��ֻ�����ļ�·������
*/    
inline BOOL BuildFilePath(LPCTSTR pszCurPath,LPCTSTR pszNewPath,BOOL bPath=TRUE)
{
    TCHAR szCurDir[MAX_PATH+1];
    GetCurrentDirectory(MAX_PATH,szCurDir);
    if(!SetCurrentDirectory(pszCurPath)) return FALSE;
    TCHAR szNewPath[MAX_PATH+1];
    _tcscpy_s(szNewPath,_countof(szNewPath),pszNewPath);
    if(bPath)
    {
        int nLen=_tcslen(szNewPath);
        if(szNewPath[nLen-1]!=_T('\\'))
            _tcscat(szNewPath,_T("\\"));
    }
    LPTSTR pszPath=_tcschr(szNewPath,_T('\\'));
    while(pszPath)
    {
        *pszPath=0;
        if(!CreateDirectory(szNewPath,NULL)) return FALSE;
        *pszPath=_T('\\');
        pszPath=_tcschr(pszPath+1,_T('\\'));
    }
    SetCurrentDirectory(szCurDir);
    return TRUE;
}

class CFileDialogEx
{
public:

    OPENFILENAME m_ofn;
    BOOL m_bOpenFileDialog;            // TRUE for file open, FALSE for file save
    TCHAR m_szFileTitle[_MAX_FNAME];   // contains file title after return
    TCHAR m_szFileName[_MAX_PATH];     // contains full path name after return

    CFileDialogEx(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
        LPCTSTR lpszDefExt = NULL,
        LPCTSTR lpszFileName = NULL,
        DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT ,
        LPCTSTR lpszFilter = NULL,
        HWND hWndParent = NULL)
    {
        memset(&m_ofn, 0, sizeof(m_ofn)); // initialize structure to 0/NULL
        m_szFileName[0] = _T('\0');
        m_szFileTitle[0] = _T('\0');

        m_bOpenFileDialog = bOpenFileDialog;
        m_ofn.lStructSize = sizeof(m_ofn);
        m_ofn.lpstrFile = m_szFileName;
        m_ofn.nMaxFile = _MAX_PATH;
        m_ofn.lpstrDefExt = lpszDefExt;
        m_ofn.lpstrFileTitle = (LPTSTR)m_szFileTitle;
        m_ofn.nMaxFileTitle = _MAX_FNAME;
        m_ofn.Flags = dwFlags | OFN_EXPLORER | OFN_ENABLEHOOK | OFN_ENABLESIZING| OFN_NOCHANGEDIR;
        m_ofn.lpstrFilter = lpszFilter;
        m_ofn.hwndOwner = hWndParent;

        // setup initial file name
        if(lpszFileName != NULL)
            _tcscpy_s(m_szFileName, _countof(m_szFileName), lpszFileName);
    }

    INT_PTR DoModal(HWND hWndParent = ::GetActiveWindow())
    {
        if(m_ofn.hwndOwner == NULL)   // set only if not specified before
            m_ofn.hwndOwner = hWndParent;

        if(m_bOpenFileDialog)
            return ::GetOpenFileName(&m_ofn);
        else
            return ::GetSaveFileName(&m_ofn);
    }
};
