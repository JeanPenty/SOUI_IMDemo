#include "stdafx.h"
#include "souistd.h"
#include <Gdiplus.h>
#include "RichEditOleCtrls.h"
#include "../SImageView.h"
#include "RichEditOleBase.h"
#include "SImRichedit.h"
#include "../../extend.events/ExtendEvents.h"
#include "RichEditObjEvents.h"
#include "../../utils.h"
#include "HTTPDownloader.h"
#include "HtmlParser.h"
#include "../../extend.skins/SAntialiasSkin.h"
#include "ImgProvider.h"
#include <shellapi.h>
#include <commctrl.h>
#include <commoncontrols.h>

namespace SOUI
{

    //-----------------------------------------------------------------------------
    //
    // internal methods
    //
    //-----------------------------------------------------------------------------

    ISkinObj* GetFileIconSkin(const SStringW& fileName)
    {
        SHFILEINFO sfi;
        ZeroMemory(&sfi, sizeof(SHFILEINFO));
        ::SHGetFileInfo(fileName, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX);

        HIMAGELIST* imageList = NULL;
        HRESULT hResult = ::SHGetImageList(SHIL_EXTRALARGE, IID_IImageList, (void**)&imageList);

        HICON hIcon = NULL;
        if (hResult == S_OK &&
            ((IImageList*)imageList)->GetIcon(sfi.iIcon, ILD_NORMAL, &hIcon) == S_OK)
        {
            SAntialiasSkin* pSkin = new SAntialiasSkin();
            pSkin->LoadFromIcon(hIcon);
            return pSkin;
        }

        return NULL;
    }

    //-----------------------------------------------------------------------------
    //
    // impl RichEditImageOle
    //
    //-----------------------------------------------------------------------------

    // {9A7A5798-AB0E-4083-AE09-F21F4CC57486}
    const GUID IID_ImageOleCtrl =
    { 0x9a7a5798, 0xab0e, 0x4083, { 0xae, 0x9, 0xf2, 0x1f, 0x4c, 0xc5, 0x74, 0x86 } };

    SStringW RichEditImageOle::TagPath = _T("path");

    RichEditImageOle::RichEditImageOle()
    {
        _isClosed = FALSE;
        _pImageView = NULL;
        _oleGuid = IID_ImageOleCtrl;
        _maxSize.cx = _maxSize.cy = 500;
        _xmlLayout = L"LAYOUT:ImageOleLayout";
        _showMagnifier = TRUE;
    }

    RichEditImageOle::~RichEditImageOle()
    {
        //
        // 把图片skin的内存由_oleView管理，当_oleView释放时，会调用skin的release
        // 
    }

    HRESULT RichEditImageOle::QueryInterface(REFIID riid, void ** ppvObject)
    {
        if (riid == IID_ImageOleCtrl)
        {
            *ppvObject = this;
            AddRef();
            return S_OK;
        }

        return RichEditOleBase::QueryInterface(riid, ppvObject);
    }

    SStringW RichEditImageOle::GetSelFormatedText()
    {
        if (_path.IsEmpty())
        {
            return SStringW();
        }

        return MakeFormattedText(_objId, _subId, _imageType, _skinId, _path, _encoding, _showMagnifier);
    }

    SStringW RichEditImageOle::MakeFormattedText(const SStringW& imageId,
        const SStringW& subId,
        const SStringW& type,
        const SStringW& skinId,
        const SStringW& imagePath,
        const SStringW& encoding,
        BOOL showManifier)
    {
        SStringW formatedText;
        formatedText.Format(L"<img id=\"%s\" subid=\"%s\" type=\"%s\" skin=\"%s\" path=\"%s\" encoding=\"%s\" show-magnifier=\"%d\"/>",
            imageId,
            subId,
            type,
            skinId,
            imagePath,
            encoding,
            showManifier);

        return formatedText;
    }

    BOOL RichEditImageOle::DownLoadNetworkFile(const SStringW& url, SStringW& path)
    {
        if (url.IsEmpty())
        {
            return FALSE;
        }

        path = url;

        if (_wcsnicmp((LPCWSTR)url, L"https", 5) == 0)
        {
            // NOT SUPPORT HTTPS
            return FALSE;
        }
        else if (_wcsnicmp((LPCWSTR)url, L"http", 4) == 0)
        {
            path.Format(L"%s%s.tmp", GetTempPath(), GenGuid());

            DWORD dwStatus = 0;
            HTTPDownloader downloader;
            if (downloader.OpenURL((LPCWSTR)url) != 0)
            {
                return FALSE;
            }

            downloader.QueryStatusCode(dwStatus);
            if (dwStatus != HTTP_STATUS_OK || downloader.DownLoadFile(path) != 0)
            {
                return FALSE;
            }
        }

        HTMLHelper::HttpPath2LocalPath(path);
        return TRUE;
    }

    void RichEditImageOle::ShowManifier(BOOL show)
    {
        _showMagnifier = show;
        SWindow* pWnd = _oleView.FindChildByName(L"BtnManifier");
        if (pWnd)
        {
            pWnd->SetVisible(show, TRUE);
        }
    }

    BOOL RichEditImageOle::SetImagePath(const SStringW& path, const SStringW& skinId)
    {
        // 同步下载图片
        SStringW newPath;
        if (!DownLoadNetworkFile(path, newPath))
        {
            _path.Empty();
            return FALSE;
        }

        // 尽量保证用同一张图片，不要重复加载
        // 头像、表情一定要用同一张
        // 图片的拷贝，发送也可以
        // 但是接收到多张同样的图片就不行了

        _path = newPath;
        _skinId = skinId;

        if (_skinId.IsEmpty())
        {
            _skinId = (LPCWSTR)GenGuid();
        }

        ISkinObj* pSkin = ImageProvider::GetImage(_skinId);
        if (!pSkin)
        {
            SAntialiasSkin* pAntSkin = new SAntialiasSkin();
            if (!pAntSkin->LoadFromFile(_path))
            {
                delete pAntSkin;
                return FALSE;
            }
            ImageProvider::Insert(_skinId, pAntSkin);
            pSkin = pAntSkin;
        }

        SetImageSkin(pSkin);

        return TRUE;
    }

    BOOL RichEditImageOle::SetImageSkin(const SStringW& skin)
    {
        ISkinObj* pSkin = ImageProvider::GetImage(_skinId);
        if (SetImageSkin(pSkin))
        {
            _skinId = skin;
            return TRUE;
        }

        return FALSE;
    }

    BOOL RichEditImageOle::SetImageSkin(ISkinObj* pSkin)
    {
        if (!_pImageView || !pSkin)
        {
            return FALSE;
        }

        _sizeNatural = pSkin->GetSkinSize();
        double fRatio = GetZoomRatio(_sizeNatural, _maxSize);
        _sizeNatural.cx = LONG((double)_sizeNatural.cx * fRatio);
        _sizeNatural.cy = LONG((double)_sizeNatural.cy * fRatio);
        CalculateExtentSize(_sizeNatural);

        _pImageView->SetSkin(pSkin);
        _oleView.SetDelayDraw(_pImageView->GetFrameCount() > 1); // 大于1帧的图片需要延迟刷新
        _oleView.SetOleWindowRect(CRect());
        _oleView.Move(0, 0, _sizeNatural.cx, _sizeNatural.cy);

        //
        // 刷新UI
        //
        BOOL scrollToBottom = _pObjHost->IsScrollAtBottom();

        if (_spAdviseSink)
        {
            _spAdviseSink->OnViewChange(DVASPECT_CONTENT, -1);
        }

        //RichEditObj* pObj = this;
        //for (; pObj->GetParent(); pObj = pObj->GetParent());
        //_pObjHost->UpdateRichObj(pObj);

        if (scrollToBottom)
        {
            _pObjHost->ScrollToBottom();
        }

        return TRUE;
    }

    SStringW RichEditImageOle::GetImagePath()
    {
        return _path;
    }

    SStringW RichEditImageOle::GetImageSkin()
    {
        return _skinId;
    }

    SStringW RichEditImageOle::GetImageType()
    {
        return _imageType;
    }

	SStringW RichEditImageOle::GetImageUrl()
	{
		return _imgUrl;
	}

	void RichEditImageOle::SetImageType(const SStringW& strType)
	{
		_imageType = strType;
	}

    SStringW RichEditImageOle::GetEncoding()
    {
        return _encoding;
    }

	SStringW RichEditImageOle::GetSubId()
	{
		return _subId;
	}

    //
    // 图片ole只读取ImgCache里的图片，所以会忽略掉_path参数，_path用来生产粘贴板信息或者提供给presenter所用
    // 调用者需要预先加载图片进ImgCache里，然后调用SetImageSkin方法。
    //
    BOOL RichEditImageOle::InitOleWindow(IRichEditObjHost * pHost)
    {
        BOOL ret = RichEditOleBase::InitOleWindow(pHost);

        if (!_pImageView)
        {
            SWindow* p = _oleView.FindChildByName(L"player");
            _pImageView = static_cast<SImageView*>(p);
        }

        ShowManifier(_showMagnifier);

        //
        // 为节省内存，尽量使用同一张图片
        //
        ISkinObj* pSkin = ImageProvider::GetImage(_skinId);
        if (pSkin != NULL)
        {
            if (pSkin->GetStates() > 1)
            {
                if (_path.IsEmpty())
                {
                    return FALSE;
                }

                //
                // gif图片需要拷贝一份新的出来，因为GIF的当前显示帧被各个SImageView持有，如果公用一份图片内存，
                // 显示的时候只能显示最后一个SImageView的当前帧
                // 
                _skinId = GenGuid();
                SAntialiasSkin* pNewSkin = new SAntialiasSkin();
                pNewSkin->LoadFromFile(_path);
                ImageProvider::Insert(_skinId, pNewSkin);

                pSkin = pNewSkin;
            }

            SetImageSkin(pSkin);
        }
        else if (!_path.IsEmpty())
        {
            // 缓存里没有指定的图片就重新加载
            return SetImagePath(_path, _skinId);
        }

        return ret;
    }

    bool RichEditImageOle::OnImageLoaded(SOUI::EventArgs *pEvt)
    {
        EventImgCacheNotify* pev = (EventImgCacheNotify*)pEvt;

        if (!SWindowMgr::GetWindow(pev->Context))
        {
            STRACE(_T("NOT FOUND ole:%08x"), pev->Context);
            return true;
        }

        BOOL scrollToBottom = _pObjHost->IsScrollAtBottom();

        SetImageSkin(_skinId);

        RichEditObj* pObj = this;
        for (; pObj->GetParent(); pObj = pObj->GetParent());
        _pObjHost->UpdateRichObj(pObj);

        if (scrollToBottom)
        {
            _pObjHost->ScrollToBottom();
        }

        return true;
    }

    HRESULT RichEditImageOle::Close(DWORD dwSaveOption)
    {
        if (_pImageView)
        {
            _pImageView->Pause();
        }
        _isClosed = TRUE;
        return S_OK;
    }

    HRESULT RichEditImageOle::Draw(
        DWORD dwDrawAspect, LONG lindex,
        void *pvAspect,
        DVTARGETDEVICE *ptd,
        HDC hdcTargetDev,
        HDC hdcDraw,
        LPCRECTL lprcBounds,
        LPCRECTL lprcWBounds,
        BOOL(STDMETHODCALLTYPE *pfnContinue)(ULONG_PTR dwContinue),
        ULONG_PTR dwContinue)
    {
        HRESULT hr = S_OK;
        hr = RichEditOleBase::Draw(dwDrawAspect,
            lindex,
            pvAspect,
            ptd,
            hdcTargetDev,
            hdcDraw,
            lprcBounds,
            lprcWBounds,
            pfnContinue,
            dwContinue);
        if (_isClosed)
        {
            _isClosed = FALSE;
            if (_pImageView) _pImageView->Resume();
        }

        return hr;
    }

    HRESULT RichEditImageOle::InternalDraw(IRenderTarget* pRt, IRegion * prgn, DWORD cp)
    {
        CRect rgnBoxRect;
        prgn->GetRgnBox(rgnBoxRect);

        POINT originalOrgPt = { 0, 0 };
        POINT newOrgPt = { _objRect.left - rgnBoxRect.left, _objRect.top - rgnBoxRect.top };

        pRt->GetViewportOrg(&originalOrgPt);
        pRt->SetViewportOrg(newOrgPt);

        _oleView.RedrawRegion(pRt, NULL);

        CHARRANGE chr = { 0, 0 };
        _pObjHost->SendMessage(EM_EXGETSEL, NULL, (LPARAM)&chr);
        if (chr.cpMin <= cp && cp < chr.cpMax && chr.cpMax - chr.cpMin > 1)
        {
            CRect rcFrame(0, 0, _objRect.Width(), _objRect.Height());
            //pRt->InvertRect(rcFrame);  //通过反转每个像素的值，从而反转一个设备场景中指定的矩形(在选中时导致gif图片透明)
        }

        pRt->SetViewportOrg(originalOrgPt);
        return S_OK;
    }

    LRESULT RichEditImageOle::ProcessMessage(
        UINT msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        RichEditOleBase::ProcessMessage(msg, wParam, lParam, bHandled);
        if (msg == WM_LBUTTONDBLCLK)
        {
            _pObjHost->NotifyRichObjEvent(this, DBLCLICK_IMAGEOLE, 0, 0);
            bHandled = TRUE; // 不希望双击事件继续往richedit传递
        }
		else if (msg == WM_LBUTTONDOWN)
		{
			_pObjHost->NotifyRichObjEvent(this, LBUTTONDOWN_IMAGE, 0, 0);
			bHandled = TRUE;
		}

        return 0;
    }


	/*

	//////////////////////////////////////////////////////////////////////////
	//
	//{9DB5554F-7BB7-4d2f-B4D7-3BD9F6E8901A}
	static const GUID IID_AudioOleCtrl = 
	{ 0x9db5554f, 0x7bb7, 0x4d2f, { 0xb4, 0xd7, 0x3b, 0xd9, 0xf6, 0xe8, 0x90, 0x1a } };

	RichEditAudioOle::RichEditAudioOle()
	{
		_oleGuid = IID_AudioOleCtrl;
		_maxSize.cx = _maxSize.cy = 180;
		_state = REAUDIO_STATE_NOPLAY;
		_canBeSelect = FALSE;
		_xmlLayout = L"LAYOUT:AudioOleLayout";
	}

	RichEditAudioOle::~RichEditAudioOle()
	{
		//
		// 把图片skin的内存由_oleView管理，当_oleView释放时，会调用skin的release
		// 
	}

	void RichEditAudioOle::AudioPlayFinish()
	{
		_state = REAUDIO_STATE_PLAYFINISH;
		SWindow* pLeftAudio = _oleView.FindChildByName2<SWindow>(L"left_audio");
		SWindow* pRightAudio = _oleView.FindChildByName2<SWindow>(L"right_audio");
		if (!pLeftAudio || !pRightAudio)
			return;

		if (L"left" == _audio_layout)
		{
			pLeftAudio->SetVisible(TRUE, TRUE);
			pRightAudio->SetVisible(FALSE, TRUE);
			SImageView* pImgPlay = pLeftAudio->FindChildByName2<SImageView>(L"play");
			if (pImgPlay)
				pImgPlay->SetVisible(TRUE, TRUE);

			SImageView* pImgPlaying = pLeftAudio->FindChildByName2<SImageView>(L"playing");
			if (pImgPlaying)
				pImgPlaying->SetVisible(FALSE, TRUE);

			SImageView* pImageReddot= pLeftAudio->FindChildByName2<SImageView>(L"isplayed");
			if (pImageReddot)
				pImageReddot->SetVisible(FALSE, TRUE);

			std::wstring wstrDuration;
			wstrDuration.append(_audio_duration);
			wstrDuration.append(L"\"");
			SWindow* pAudioDuration = pLeftAudio->FindChildByName2<SWindow>(L"audio_duration");
			if (pAudioDuration)
			{
				pAudioDuration->SetWindowText(wstrDuration.c_str());
				pAudioDuration->SetVisible(TRUE, TRUE);
			}
		}
		else if (L"right" == _audio_layout)
		{
			pLeftAudio->SetVisible(FALSE, TRUE);
			pRightAudio->SetVisible(TRUE, TRUE);
			SImageView* pImgPlay = pRightAudio->FindChildByName2<SImageView>(L"play");
			if (pImgPlay)
				pImgPlay->SetVisible(TRUE, TRUE);

			SImageView* pImgPlaying = pRightAudio->FindChildByName2<SImageView>(L"playing");
			if (pImgPlaying)
				pImgPlaying->SetVisible(FALSE, TRUE);

			SImageView* pImageReddot= pRightAudio->FindChildByName2<SImageView>(L"isplayed");
			if (pImageReddot)
				pImageReddot->SetVisible(FALSE, TRUE);

			std::wstring wstrDuration;
			wstrDuration.append(_audio_duration);
			wstrDuration.append(L"\"");
			SWindow* pAudioDuration = pRightAudio->FindChildByName2<SWindow>(L"audio_duration");
			if (pAudioDuration)
			{
				pAudioDuration->SetWindowText(wstrDuration.c_str());
				pAudioDuration->SetVisible(TRUE, TRUE);
			}
		}
	}

	bool RichEditAudioOle::OnAudioPlayClick(SOUI::EventArgs* pEvt)
	{
		_state = REAUDIO_STATE_PLAYING;
		SWindow* pLeftAudio = _oleView.FindChildByName2<SWindow>(L"left_audio");
		SWindow* pRightAudio = _oleView.FindChildByName2<SWindow>(L"right_audio");
		if (!pLeftAudio || !pRightAudio)
			return false;

		if (L"left" == _audio_layout)
		{
			pLeftAudio->SetVisible(TRUE, TRUE);
			pRightAudio->SetVisible(FALSE, TRUE);
			SImageView* pImgPlay = pLeftAudio->FindChildByName2<SImageView>(L"play");
			if (pImgPlay)
				pImgPlay->SetVisible(FALSE, TRUE);

			SImageView* pImgPlaying = pLeftAudio->FindChildByName2<SImageView>(L"playing");
			if (pImgPlaying)
			{
				pImgPlaying->SetVisible(TRUE, TRUE);
				pImgPlaying->Resume();
			}

			SImageView* pImageReddot= pLeftAudio->FindChildByName2<SImageView>(L"isplayed");
			if (pImageReddot)
				pImageReddot->SetVisible(FALSE, TRUE);

			std::wstring wstrDuration;
			wstrDuration.append(_audio_duration);
			wstrDuration.append(L"\"");
			SWindow* pAudioDuration = pLeftAudio->FindChildByName2<SWindow>(L"audio_duration");
			if (pAudioDuration)
			{
				pAudioDuration->SetWindowText(wstrDuration.c_str());
				pAudioDuration->SetVisible(TRUE, TRUE);
			}
		}
		else if (L"right" == _audio_layout)
		{
			pLeftAudio->SetVisible(FALSE, TRUE);
			pRightAudio->SetVisible(TRUE, TRUE);
			SImageView* pImgPlay = pRightAudio->FindChildByName2<SImageView>(L"play");
			if (pImgPlay)
				pImgPlay->SetVisible(FALSE, TRUE);

			SImageView* pImgPlaying = pRightAudio->FindChildByName2<SImageView>(L"playing");
			if (pImgPlaying)
			{
				pImgPlaying->SetVisible(TRUE, TRUE);
				pImgPlaying->Resume();
			}

			SImageView* pImageReddot= pRightAudio->FindChildByName2<SImageView>(L"isplayed");
			if (pImageReddot)
				pImageReddot->SetVisible(FALSE, TRUE);

			std::wstring wstrDuration;
			wstrDuration.append(_audio_duration);
			wstrDuration.append(L"\"");
			SWindow* pAudioDuration = pRightAudio->FindChildByName2<SWindow>(L"audio_duration");
			if (pAudioDuration)
			{
				pAudioDuration->SetWindowText(wstrDuration.c_str());
				pAudioDuration->SetVisible(TRUE, TRUE);
			}
		}

		_pObjHost->NotifyRichObjEvent(this, CLICK_AUDIO_PLAY, 0, 0);
		return true;
	}

	bool RichEditAudioOle::OnAudioPlayingClick(SOUI::EventArgs* pEvt)
	{		
		_state = REAUDIO_STATE_PLAYFINISH;
		SWindow* pLeftAudio = _oleView.FindChildByName2<SWindow>(L"left_audio");
		SWindow* pRightAudio = _oleView.FindChildByName2<SWindow>(L"right_audio");
		if (!pLeftAudio || !pRightAudio)
			return false;

		if (L"left" == _audio_layout)
		{
			pLeftAudio->SetVisible(TRUE, TRUE);
			pRightAudio->SetVisible(FALSE, TRUE);
			SImageView* pImgPlay = pLeftAudio->FindChildByName2<SImageView>(L"play");
			if (pImgPlay)
				pImgPlay->SetVisible(TRUE, TRUE);

			SImageView* pImgPlaying = pLeftAudio->FindChildByName2<SImageView>(L"playing");
			if (pImgPlaying)
			{
				pImgPlaying->SetVisible(FALSE, TRUE);
			}

			SImageView* pImageReddot= pLeftAudio->FindChildByName2<SImageView>(L"isplayed");
			if (pImageReddot)
				pImageReddot->SetVisible(FALSE, TRUE);

			std::wstring wstrDuration;
			wstrDuration.append(_audio_duration);
			wstrDuration.append(L"\"");
			SWindow* pAudioDuration = pLeftAudio->FindChildByName2<SWindow>(L"audio_duration");
			if (pAudioDuration)
			{
				pAudioDuration->SetWindowText(wstrDuration.c_str());
				pAudioDuration->SetVisible(TRUE, TRUE);
			}
		}
		else if (L"right" == _audio_layout)
		{
			pLeftAudio->SetVisible(FALSE, TRUE);
			pRightAudio->SetVisible(TRUE, TRUE);
			SImageView* pImgPlay = pRightAudio->FindChildByName2<SImageView>(L"play");
			if (pImgPlay)
				pImgPlay->SetVisible(TRUE, TRUE);

			SImageView* pImgPlaying = pRightAudio->FindChildByName2<SImageView>(L"playing");
			if (pImgPlaying)
			{
				pImgPlaying->SetVisible(FALSE, TRUE);
			}

			SImageView* pImageReddot= pRightAudio->FindChildByName2<SImageView>(L"isplayed");
			if (pImageReddot)
				pImageReddot->SetVisible(FALSE, TRUE);

			std::wstring wstrDuration;
			wstrDuration.append(_audio_duration);
			wstrDuration.append(L"\"");
			SWindow* pAudioDuration = pRightAudio->FindChildByName2<SWindow>(L"audio_duration");
			if (pAudioDuration)
			{
				pAudioDuration->SetWindowText(wstrDuration.c_str());
				pAudioDuration->SetVisible(TRUE, TRUE);
			}
		}

		_pObjHost->NotifyRichObjEvent(this, CLICK_AUDIO_PLAYING, 0, 0);
		return true;
	}
	
	BOOL RichEditAudioOle::InitOleWindow(IRichEditObjHost * pHost)
	{
		BOOL ret = RichEditOleBase::InitOleWindow(pHost);
		std::wstring wstrAudioDuration = _audio_duration;
		std::string strAudioDuration;
		KfUtility::ConvertUtf16ToUtf8(wstrAudioDuration, strAudioDuration);
		int nAudioDuration = atoi(strAudioDuration.c_str());
		if (nAudioDuration > 0 && nAudioDuration <= 15)
			_audio_len = REAUDIO_LEN_15;
		else if (nAudioDuration > 15 && nAudioDuration <= 30)
			_audio_len = REAUDIO_LEN_30;
		else if (nAudioDuration > 30 && nAudioDuration <= 45)
			_audio_len = REAUDIO_LEN_45;
		else
			_audio_len = REAUDIO_LEN_60;

		SStringW sstrPlaySkin, sstrPlayingSkin;
		if (L"left" == _audio_layout)
		{
			switch (_audio_len)
			{
			case REAUDIO_LEN_15:
				{
					sstrPlaySkin = L"skin.richaudio_15play";
					sstrPlayingSkin = L"skin.richaudio_15playing";
				}
				break;
			case REAUDIO_LEN_30:
				{
					sstrPlaySkin = L"skin.richaudio_30play";
					sstrPlayingSkin = L"skin.richaudio_30playing";
				}
				break;
			case REAUDIO_LEN_45:
				{
					sstrPlaySkin = L"skin.richaudio_45play";
					sstrPlayingSkin = L"skin.richaudio_45playing";
				}
				break;
			case REAUDIO_LEN_60:
				{
					sstrPlaySkin = L"skin.richaudio_60play";
					sstrPlayingSkin = L"skin.richaudio_60playing";
				}
				break;
			default:
				break;
			}
		}
		else
		{
			switch (_audio_len)
			{
			case REAUDIO_LEN_15:
				{
					sstrPlaySkin = L"skin.richaudio_right15play";
					sstrPlayingSkin = L"skin.richaudio_right15playing";
				}
				break;
			case REAUDIO_LEN_30:
				{
					sstrPlaySkin = L"skin.richaudio_right30play";
					sstrPlayingSkin = L"skin.richaudio_right30playing";
				}
				break;
			case REAUDIO_LEN_45:
				{
					sstrPlaySkin = L"skin.richaudio_right45play";
					sstrPlayingSkin = L"skin.richaudio_right45playing";
				}
				break;
			case REAUDIO_LEN_60:
				{
					sstrPlaySkin = L"skin.richaudio_right60play";
					sstrPlayingSkin = L"skin.richaudio_right60playing";
				}
				break;
			default:
				break;
			}
		}

		ISkinObj* pSkinPlay = GETSKIN(sstrPlaySkin, 100);
		ISkinObj* pSkinPlaying = GETSKIN(sstrPlayingSkin, 100);
		if (L"left" == _audio_layout)
		{
			SWindow* pLeftWindow = _oleView.FindChildByName2<SWindow>(L"left_audio");
			SWindow* pRightWindow = _oleView.FindChildByName2<SWindow>(L"right_audio");
			if (pLeftWindow && pRightWindow)
			{
				pLeftWindow->SetVisible(TRUE, TRUE);
				pRightWindow->SetVisible(FALSE, TRUE);
				SImageView* pImgPlay = pLeftWindow->FindChildByName2<SImageView>(L"play");
				SImageView* pImgPlaying = pLeftWindow->FindChildByName2<SImageView>(L"playing");
				if (pImgPlay && pSkinPlay)
				{
					pImgPlay->SetSkin(pSkinPlay);
					pImgPlay->SetVisible(TRUE, TRUE);
					pImgPlay->Resume();
					SUBSCRIBE(pImgPlay, EVT_CMD, RichEditAudioOle::OnAudioPlayClick);
				}
				if (pImgPlaying && pSkinPlaying)
				{
					pImgPlaying->SetSkin(pSkinPlaying);
					pImgPlaying->SetVisible(FALSE, TRUE);
					pImgPlaying->Resume();
					SUBSCRIBE(pImgPlaying, EVT_CMD, RichEditAudioOle::OnAudioPlayingClick);
				}
			}

			ISkinObj* pSkinReddot = GETSKIN(L"skin.richaudio_reddot", 100);
			SImageView* pImageReddot= pLeftWindow->FindChildByName2<SImageView>(L"isplayed");
			if (pImageReddot && pSkinReddot)
			{
				pImageReddot->SetSkin(pSkinReddot);
				if (L"0" == _audio_isplayed)
					pImageReddot->SetVisible(TRUE, TRUE);	
				else
					pImageReddot->SetVisible(FALSE, TRUE);
			}

			std::wstring wstrDuration;
			wstrDuration.append(_audio_duration);
			wstrDuration.append(L"\"");
			SWindow* pAudioDuration = pLeftWindow->FindChildByName2<SWindow>(L"audio_duration");
			if (pAudioDuration)
			{
				pAudioDuration->SetWindowText(wstrDuration.c_str());
				pAudioDuration->SetVisible(TRUE, TRUE);
			}
		}
		else if (L"right" == _audio_layout)
		{
			SWindow* pLeftWindow = _oleView.FindChildByName2<SWindow>(L"left_audio");
			SWindow* pRightWindow = _oleView.FindChildByName2<SWindow>(L"right_audio");
			if (pLeftWindow && pRightWindow)
			{
				pLeftWindow->SetVisible(FALSE, TRUE);
				pRightWindow->SetVisible(TRUE, TRUE);
				SImageView* pImgPlay = pRightWindow->FindChildByName2<SImageView>(L"play");
				SImageView* pImgPlaying = pRightWindow->FindChildByName2<SImageView>(L"playing");
				if (pImgPlay && pSkinPlay)
				{
					pImgPlay->SetSkin(pSkinPlay);
					pImgPlay->SetVisible(TRUE, TRUE);
					pImgPlay->Resume();
					SUBSCRIBE(pImgPlay, EVT_CMD, RichEditAudioOle::OnAudioPlayClick);
				}
				if (pImgPlaying && pSkinPlaying)
				{
					pImgPlaying->SetSkin(pSkinPlaying);
					pImgPlaying->SetVisible(FALSE, TRUE);
					pImgPlaying->Resume();
					SUBSCRIBE(pImgPlaying, EVT_CMD, RichEditAudioOle::OnAudioPlayingClick);
				}
			}

			ISkinObj* pSkinReddot = GETSKIN(L"skin.richaudio_reddot", 100);
			SImageView* pImageReddot= pRightWindow->FindChildByName2<SImageView>(L"isplayed");
			if (pImageReddot && pSkinReddot)
			{
				pImageReddot->SetSkin(pSkinReddot);
				if (L"0" == _audio_isplayed)
					pImageReddot->SetVisible(TRUE, TRUE);	
				else
					pImageReddot->SetVisible(FALSE, TRUE);
			}

			std::wstring wstrDuration;
			wstrDuration.append(_audio_duration);
			wstrDuration.append(L"\"");
			SWindow* pAudioDuration = pRightWindow->FindChildByName2<SWindow>(L"audio_duration");
			if (pAudioDuration)
			{
				pAudioDuration->SetWindowText(wstrDuration.c_str());
				pAudioDuration->SetVisible(TRUE, TRUE);
			}
		}

		return ret;
	}

	void RichEditAudioOle::UpdatePosition()
	{
		if (_spAdviseSink)
		{
 			if (REAUDIO_LEN_15 == _audio_len)
 				_sizeNatural.cx = 130;
 			else if (REAUDIO_LEN_30 == _audio_len)
 				_sizeNatural.cx = 150;
 			else if (REAUDIO_LEN_45 == _audio_len)
 				_sizeNatural.cx = 170;
 			else if (REAUDIO_LEN_60 == _audio_len)
 				_sizeNatural.cx = 190;
			_sizeNatural.cy = 40;

			_oleView.SetOleWindowRect(CRect(0, 0, 0, 0)); // 已经失效
			_oleView.Move(0, 0, _sizeNatural.cx, _sizeNatural.cy);
			CalculateExtentSize(_sizeNatural);
			_spAdviseSink->OnViewChange(DVASPECT_CONTENT, -1);
		}
	}
	

	//
	// ------------------------------------------------------------------------------
	// impl RichEditVideoOle
	//
	// {FAB32B7E-AC02-468c-A615-1E730243B62E}
	static const GUID IID_VideoOleCtrl = 
		{ 0xfab32b7e, 0xac02, 0x468c, { 0xa6, 0x15, 0x1e, 0x73, 0x2, 0x43, 0xb6, 0x2e } };
	RichEditVideoOle::RichEditVideoOle()
	{
		_oleGuid = IID_VideoOleCtrl;
		_sizeNatural.cx = 300;
		_sizeNatural.cy = 160;
		_state = REVIDEO_STATE_NORMAL;
		_canBeSelect = FALSE;
		_xmlLayout = L"LAYOUT:VideoOleLayout";
	}

	RichEditVideoOle::~RichEditVideoOle()
	{
	}

	BOOL RichEditVideoOle::InitOleWindow(IRichEditObjHost* pHost)
	{
		BOOL ret = RichEditOleBase::InitOleWindow(pHost);
		SWindow* pVideoWnd = _oleView.FindChildByName2<SWindow>(L"video_frame");
		if (pVideoWnd)
		{
			SImageWnd* pVideoCoverWnd = pVideoWnd->FindChildByName2<SImageWnd>(L"video_cover_img");
			IBitmap* pFrameImg = SResLoadFromFile::LoadImage(_video_cover_path);
			if (pVideoCoverWnd && pFrameImg)
			{
				pVideoCoverWnd->SetImage(pFrameImg);
				UpdateWindowLayout(pVideoCoverWnd);
			}
		}

		SImageView* pPlay = _oleView.FindChildByName2<SImageView>(L"play");
		ISkinObj* pPlaySkin = GETSKIN(L"skin.richvideo_play", 100);
		if (pPlay && pPlaySkin)
		{
			pPlay->SetSkin(pPlaySkin);
			pPlay->SetVisible(TRUE, TRUE);
			pPlay->Resume();

			SUBSCRIBE(pPlay, EVT_CMD, RichEditVideoOle::OnBnClickPlay);
		}
		
		SImageView* pLoading = _oleView.FindChildByName2<SImageView>(L"loading");
		ISkinObj* pLoadingSkin = GETSKIN(L"skin.richvideo_loading", 100);
		if (pLoading && pLoadingSkin)
		{
			pLoading->SetSkin(pLoadingSkin);
			pLoading->SetVisible(FALSE, TRUE);
			pLoading->Resume();
		}

		std::wstring wstrVideoDuration;
		std::string strVideoDuration;
		wstrVideoDuration = _video_duration;
		KfUtility::ConvertUtf16ToUtf8(wstrVideoDuration, strVideoDuration);
		int nVideoDuration = atoi(strVideoDuration.c_str());
		SStringW sstrVideoDuration;

		int nS = nVideoDuration%60;
		int nM = nVideoDuration/60%60;
		int nH = nVideoDuration / 3600;
		if (nVideoDuration/3600 < 24)
			sstrVideoDuration.Format(L"%02d:%02d", nM, nS);
		else
			sstrVideoDuration.Format(L"%02d:%02d:%02d", nH, nM, nS);

		SStatic* pVideoDuration = _oleView.FindChildByName2<SStatic>(L"video_duration");
		if (pVideoDuration)
			pVideoDuration->SetWindowText(sstrVideoDuration);

		return ret;
	}

	void RichEditVideoOle::SetVideoFrameImage(const SStringW& sstrImagePath)
	{
		_video_cover_path = sstrImagePath;
		SWindow* pVideoWnd = _oleView.FindChildByName2<SWindow>(L"video_frame");
		if (pVideoWnd)
		{
			SImageWnd* pVideoCoverWnd = pVideoWnd->FindChildByName2<SImageWnd>(L"video_cover_img");
			IBitmap* pFrameImg = SResLoadFromFile::LoadImage(_video_cover_path);
			if (pVideoCoverWnd && pFrameImg)
			{
				pVideoCoverWnd->SetImage(pFrameImg);
				UpdateWindowLayout(pVideoCoverWnd);
			}
		}
	}

	bool RichEditVideoOle::OnBnClickPlay(SOUI::EventArgs* pEvt)
	{
		if (REVIDEO_STATE_DOWNFINISH != _state)
		{
			_state = REVIDEO_STATE_DOWNLOADING;
			SWindow* pVideoWnd = _oleView.FindChildByName2<SWindow>(L"video_frame");
			if (pVideoWnd)
			{
				SImageWnd* pVideoCoverWnd = pVideoWnd->FindChildByName2<SImageWnd>(L"video_cover_img");
				IBitmap* pFrameImg = SResLoadFromFile::LoadImage(_video_cover_path);
				if (pVideoCoverWnd && pFrameImg)
				{
					pVideoCoverWnd->SetImage(pFrameImg);
					UpdateWindowLayout(pVideoCoverWnd);
				}
			}

			SImageView* pPlay = _oleView.FindChildByName2<SImageView>(L"play");
			ISkinObj* pPlaySkin = GETSKIN(L"skin.richvideo_play", 100);
			if (pPlay && pPlaySkin)
			{
				pPlay->SetSkin(pPlaySkin);
				pPlay->SetVisible(FALSE, TRUE);
				pPlay->Resume();
			}

			SImageView* pLoading = _oleView.FindChildByName2<SImageView>(L"loading");
			ISkinObj* pLoadingSkin = GETSKIN(L"skin.richvideo_loading", 100);
			if (pLoading && pLoadingSkin)
			{
				pLoading->SetSkin(pLoadingSkin);
				pLoading->SetVisible(TRUE, TRUE);
				pLoading->Resume();
			}

			std::wstring wstrVideoDuration;
			std::string strVideoDuration;
			wstrVideoDuration = _video_duration;
			KfUtility::ConvertUtf16ToUtf8(wstrVideoDuration, strVideoDuration);
			int nVideoDuration = atoi(strVideoDuration.c_str());
			SStringW sstrVideoDuration;

			int nS = nVideoDuration%60;
			int nM = nVideoDuration/60%60;
			int nH = nVideoDuration / 3600;
			if (nVideoDuration/3600 < 24)
				sstrVideoDuration.Format(L"%02d:%02d", nM, nS);
			else
				sstrVideoDuration.Format(L"%02d:%02d:%02d", nH, nM, nS);

			SStatic* pVideoDuration = _oleView.FindChildByName2<SStatic>(L"video_duration");
			if (pVideoDuration)
				pVideoDuration->SetWindowText(sstrVideoDuration);

			_pObjHost->NotifyRichObjEvent(this, CLICK_VIDEODOWNLOAD, 0, 0);
		}
		else
			_pObjHost->NotifyRichObjEvent(this, CLICK_VIDEOPLAY, 0, 0);
		return true;
	}

	void RichEditVideoOle::SetVideoPlayState(int nstate)
	{
		if (REVIDEO_STATE_DOWNFINISH == nstate)
		{
			_state = REVIDEO_STATE_DOWNFINISH;
			SWindow* pVideoWnd = _oleView.FindChildByName2<SWindow>(L"video_frame");
			if (pVideoWnd)
			{
				SImageWnd* pVideoCoverWnd = pVideoWnd->FindChildByName2<SImageWnd>(L"video_cover_img");
				IBitmap* pFrameImg = SResLoadFromFile::LoadImage(_video_cover_path);
				if (pVideoCoverWnd && pFrameImg)
				{
					pVideoCoverWnd->SetImage(pFrameImg);
					UpdateWindowLayout(pVideoCoverWnd);
				}
			}

			SImageView* pPlay = _oleView.FindChildByName2<SImageView>(L"play");
			ISkinObj* pPlaySkin = GETSKIN(L"skin.richvideo_play", 100);
			if (pPlay && pPlaySkin)
			{
				pPlay->SetSkin(pPlaySkin);
				pPlay->SetVisible(TRUE, TRUE);
				pPlay->Resume();
			}

			SImageView* pLoading = _oleView.FindChildByName2<SImageView>(L"loading");
			ISkinObj* pLoadingSkin = GETSKIN(L"skin.richvideo_loading", 100);
			if (pLoading && pLoadingSkin)
			{
				pLoading->SetSkin(pLoadingSkin);
				pLoading->SetVisible(FALSE, TRUE);
				pLoading->Resume();
			}

			std::wstring wstrVideoDuration;
			std::string strVideoDuration;
			wstrVideoDuration = _video_duration;
			KfUtility::ConvertUtf16ToUtf8(wstrVideoDuration, strVideoDuration);
			int nVideoDuration = atoi(strVideoDuration.c_str());
			SStringW sstrVideoDuration;

			int nS = nVideoDuration%60;
			int nM = nVideoDuration/60%60;
			int nH = nVideoDuration / 3600;
			if (nVideoDuration/3600 < 24)
				sstrVideoDuration.Format(L"%02d:%02d", nM, nS);
			else
				sstrVideoDuration.Format(L"%02d:%02d:%02d", nH, nM, nS);

			SStatic* pVideoDuration = _oleView.FindChildByName2<SStatic>(L"video_duration");
			if (pVideoDuration)
				pVideoDuration->SetWindowText(sstrVideoDuration);
		}
	}

	void RichEditVideoOle::UpdatePosition()
	{
		if (_spAdviseSink)
		{
			std::string strWidth, strHeight;
			std::wstring wstrWidth, wstrHeight;
			wstrWidth = _video_cover_width;
			wstrHeight = _video_cover_height;

			KfUtility::ConvertUtf16ToUtf8(wstrWidth, strWidth);
			KfUtility::ConvertUtf16ToUtf8(wstrHeight, strHeight);

			SIZE tempSize;
			tempSize.cx = atoi(strWidth.c_str());
			tempSize.cy = atoi(strHeight.c_str());

			double fRatio = GetZoomRatio(tempSize, _sizeNatural);
			tempSize.cx = LONG((double)tempSize.cx * fRatio);
			tempSize.cy = LONG((double)tempSize.cy * fRatio);
			_oleView.SetOleWindowRect(CRect(0, 0, 0, 0)); // 已经失效
			_oleView.Move(0, 0, tempSize.cx, tempSize.cy);
			CalculateExtentSize(tempSize);
			_spAdviseSink->OnViewChange(DVASPECT_CONTENT, -1);
		}
	}

	//
	// ------------------------------------------------------------------------------
	// impl RichEditWebOle
	//
	// {237482AA-F47B-4514-9433-6B18E913A9E9}
	static const GUID IID_LinkOleCtrl = 
		{ 0x237482aa, 0xf47b, 0x4514, { 0x94, 0x33, 0x6b, 0x18, 0xe9, 0x13, 0xa9, 0xe9 } };
	RichEditLinkOle::RichEditLinkOle()
	{
		_oleGuid = IID_LinkOleCtrl;
		_sizeNatural.cx = 240;
		_sizeNatural.cy = 90;
		_canBeSelect = FALSE;
		_xmlLayout = L"LAYOUT:LinkOleLayout";
	}

	RichEditLinkOle::~RichEditLinkOle()
	{
	}

	BOOL RichEditLinkOle::InitOleWindow(IRichEditObjHost* pHost)
	{
		BOOL bRet = RichEditOleBase::InitOleWindow(pHost);
		SWindow* pUrlWnd = _oleView.FindChildByName2<SWindow>(L"LinkWnd");
		if (pUrlWnd)
		{
			SUBSCRIBE(pUrlWnd, EVT_CMD, RichEditLinkOle::OnBnClickWebOle);
		}

		SStatic* pTitle = _oleView.FindChildByName2<SStatic>(L"link_title");
		if (pTitle)
			pTitle->SetWindowText(_link_title);

		SStatic* pContent = _oleView.FindChildByName2<SStatic>(L"link_content");
		if (pContent)
			pContent->SetWindowText(_link_content);

		SImageWnd* pUrlImg = _oleView.FindChildByName2<SImageWnd>(L"link_image");
		IBitmap* pImg = SResLoadFromFile::LoadImage(_link_imgpath);
		if (pUrlImg && pImg)
		{
			pUrlImg->SetImage(pImg);
			UpdateWindowLayout(pUrlImg);
		}
		return bRet;
	}

	bool RichEditLinkOle::OnBnClickWebOle(SOUI::EventArgs* pEvt)
	{
		_pObjHost->NotifyRichObjEvent(this, CLICK_LINK_OLE, 0, 0);
		return true;
	}

	void RichEditLinkOle::UpdatePosition()
	{
		if (_spAdviseSink)
		{
			_oleView.SetOleWindowRect(CRect(0, 0, 0, 0)); // 已经失效
			_oleView.Move(0, 0, _sizeNatural.cx, _sizeNatural.cy);
			CalculateExtentSize(_sizeNatural);
			_spAdviseSink->OnViewChange(DVASPECT_CONTENT, -1);
		}
	}

	//
	// ------------------------------------------------------------------------------
	// impl RichEditLocationOle
	//
	// {06EB0418-BD55-49e0-952A-AE86152B94F7}
	static const GUID IID_LocationOleCtrl =
	{ 0x6eb0418, 0xbd55, 0x49e0, { 0x95, 0x2a, 0xae, 0x86, 0x15, 0x2b, 0x94, 0xf7 } };

	RichEditLocationOle::RichEditLocationOle()
	{
		_oleGuid = IID_LocationOleCtrl;
		_sizeNatural.cx = 300;
		_sizeNatural.cy = 160;
		_canBeSelect = FALSE;
		_xmlLayout = L"LAYOUT:LocationOleLayout";
	}

	RichEditLocationOle::~RichEditLocationOle()
	{
	}

	BOOL RichEditLocationOle::InitOleWindow(IRichEditObjHost* pHost)
	{
		BOOL bRet = RichEditOleBase::InitOleWindow(pHost);
		IBitmap* pLocationImg = SResLoadFromFile::LoadImage(_location_imgpath);
		SImageWnd* pLocationImgWnd = _oleView.FindChildByName2<SImageWnd>(L"location_img");
		if (pLocationImg && pLocationImgWnd)
		{
			pLocationImgWnd->SetImage(pLocationImg);
			UpdateWindowLayout(pLocationImgWnd);
		}

		SStatic* pLocationTitle = _oleView.FindChildByName2<SStatic>(L"location_title");
		if (pLocationTitle)
		{
			pLocationTitle->SetWindowText(_location_title);
			UpdateWindowLayout(pLocationTitle);
		}

		SStatic* pLocationAddress = _oleView.FindChildByName2<SStatic>(L"location_address");
		if (pLocationAddress)
		{
			pLocationAddress->SetWindowText(_location_address);
			UpdateWindowLayout(pLocationAddress);
		}

		SWindow* pLocationWnd = _oleView.FindChildByName2<SWindow>(L"locationwnd");
		if (pLocationWnd)
		{
			SUBSCRIBE(pLocationWnd, EVT_CMD, RichEditLocationOle::OnBnClickLocation);
		}

		return bRet;
	}

	void RichEditLocationOle::SetLocationImage(const SStringW& sstrImagePath)
	{
		IBitmap* pLocationImg = SResLoadFromFile::LoadImage(sstrImagePath);
		SImageWnd* pLocationImgWnd = _oleView.FindChildByName2<SImageWnd>(L"location_img");
		if (pLocationImg && pLocationImgWnd)
		{
			pLocationImgWnd->SetImage(pLocationImg);
			UpdateWindowLayout(pLocationImgWnd);
		}
	}

	bool RichEditLocationOle::OnBnClickLocation(SOUI::EventArgs* pEvt)
	{
		_pObjHost->NotifyRichObjEvent(this, CLICK_LOCATION_OLE, 0, 0);
		return true;
	}

	void RichEditLocationOle::UpdatePosition()
	{
		if (_spAdviseSink)
		{
			std::string strWidth, strHeight;
			std::wstring wstrWidth, wstrHeight;
			wstrWidth = _img_width;
			wstrHeight = _img_height;

			KfUtility::ConvertUtf16ToUtf8(wstrWidth, strWidth);
			KfUtility::ConvertUtf16ToUtf8(wstrHeight, strHeight);

			SIZE tempSize;
			tempSize.cx = atoi(strWidth.c_str()) + 50;
			tempSize.cy = atoi(strHeight.c_str()) + 10;

			double fRatio = GetZoomRatio(tempSize, _sizeNatural);
			tempSize.cx = LONG((double)tempSize.cx * fRatio);
			tempSize.cy = LONG((double)tempSize.cy * fRatio);

			_oleView.SetOleWindowRect(CRect(0, 0, 0, 0)); // 已经失效
			_oleView.Move(0, 0, tempSize.cx, tempSize.cy);
			CalculateExtentSize(tempSize);
			_spAdviseSink->OnViewChange(DVASPECT_CONTENT, -1);
		}
	}

	//
    // ------------------------------------------------------------------------------
    // impl RichEditFileOle
    //
    // {E0ED3FC5-1645-4b7f-A0E2-86F5288F407B}
    static const GUID IID_FileOleCtrl =
    { 0xe0ed3fc5, 0x1645, 0x4b7f, { 0xa0, 0xe2, 0x86, 0xf5, 0x28, 0x8f, 0x40, 0x7b } };

    RichEditFileOle::RichEditFileOle()
    {
        _oleGuid = IID_FileOleCtrl;
        _sizeNatural.cx = 300;
        _sizeNatural.cy = 95;
		_canBeSelect = FALSE;
        _xmlLayout = L"LAYOUT:FileOleLayout";
    }

    RichEditFileOle::~RichEditFileOle()
    {
    }

    SStringW RichEditFileOle::MakeFormattedText(
        const SStringW& filePath,
        const SStringW& fileState,
        __int64 fileSize,
        int visibleLinks, 
		const SStringW& fileSuffix)
    {
        SStringW formattedText;

        formattedText.Format(L"<file selectable=\"0\" file-path=\"%s\" file-size=\"%I64d\" file-state=\"%s\" links=\"%d\" file-suffix=\"%s\" />",
            filePath,       // file-path
            fileSize,       // file-size
            fileState,      // file-state
            visibleLinks,	// links
			fileSuffix);  

        return formattedText;
    }

    //LRESULT RichEditFileOle::ProcessMessage(UINT msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    //{
    //    RichEditOleBase::ProcessMessage(msg, wParam, lParam, bHandled);
    //    if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK)
    //    {
    //        bHandled = TRUE; // 文件OLE不让RichEdit继续左击，否则会画出一个黑框
    //    }
    //    return 0;
    //}

	bool RichEditFileOle::OnFileNameClicked(SOUI::EventArgs* pEvt)
	{
		int linkFlag = LINK_OPEN_FILE;
		_pObjHost->NotifyRichObjEvent(this, CLICK_FILEOLE, linkFlag, (LPARAM)(LPCWSTR)_filePath);
		return true;
	}

    bool RichEditFileOle::OnLinkClicked(SOUI::EventArgs *pEvt)
    {
        int linkFlag = 0;

        SStringW linkName = pEvt->sender->GetName();
        if (linkName == _T("LnkSaveFile"))
        {
            linkFlag = LINK_SAVE;
        }
        else if (linkName == _T("LnkSaveFileAs"))
        {
            linkFlag = LINK_SAVEAS;
        }
        else if (linkName == _T("LnkCancelFile"))
        {
            linkFlag = LINK_CANCEL;
        }
        else if (linkName == _T("LnkOpenFile"))
        {
            linkFlag = LINK_OPEN_FILE;
        }
        else if (linkName == _T("LnkOpenFileDir"))
        {
            linkFlag = LINK_OPEN_DIR;
        }
        else if (linkName == _T("LnkContinueFile"))
        {
            linkFlag = LINK_CONTINUE;
        }
        else if (linkName == _T("LnkForwardFile"))
        {
            linkFlag = LINK_FORWARD;
        }
		else if (linkName == _T("LnkDownLoad"))
		{
			linkFlag = LINK_DOWNLOAD;
		}

        _pObjHost->NotifyRichObjEvent(this, CLICK_FILEOLE, linkFlag, (LPARAM)(LPCWSTR)_filePath);
        return true;
    }

    SStringW RichEditFileOle::GetSizeBeautyString(unsigned long long size)
    {
        const TCHAR* pLevelTable[] = { _T("B"), _T("KB"), _T("MB"), _T("GB") };

        int level = 0;
        long double fSize = (long double)size;
        for (; fSize > 1024.0f; ++level)
        {
            fSize /= 1024.0f;
        }

        return SStringW().Format(_T("%.2f %s"), fSize, pLevelTable[level]);
    }

    BOOL RichEditFileOle::InitOleWindow(IRichEditObjHost* pHost)
    {
        RichEditOleBase::InitOleWindow(pHost);

        __int64 size = _wtoi64(_fileSize);

        SetFileSize(size, FALSE);
        SetFilePath(_filePath);
        SetFileLinksVisible(_links);
        SetFileStateString(_fileState);
		SetFileSuffix(_fileSuffix);

        TCHAR* links[] = {
            _T("LnkSaveFile"),
            _T("LnkSaveFileAs"),
            _T("LnkCancelFile"),
            _T("LnkOpenFile"),
            _T("LnkOpenFileDir"),
            _T("LnkContinueFile"),
            _T("LnkForwardFile"),
			_T("LnkDownLoad"),
        };

		SWindow* pWndName = _oleView.FindChildByName(L"file_suffix");
		if (pWndName)
		{
			SUBSCRIBE(pWndName, EVT_CMD, RichEditFileOle::OnFileNameClicked);
		}

        for (int i = 0; i < sizeof(links) / sizeof(links[0]); ++i)
        {
            SWindow * pWnd = _oleView.FindChildByName(links[i]);
            if (pWnd)
            {
                SUBSCRIBE(pWnd, EVT_CMD, RichEditFileOle::OnLinkClicked);
            }
        }

        return TRUE;
    }

    void RichEditFileOle::SetFileSize(__int64 size, BOOL requestLayout)
    {
        //_fileSizeBytes = _wtoi64(size);
        _fileSizeBytes = size;

        SWindow * pSizeWnd = _oleView.FindChildByName(L"LblFileSize");
        if (pSizeWnd)
        {
            //_fileSize = size;

            SStringW text;
            text.Format(_T("%s"), GetSizeBeautyString(_fileSizeBytes));

            pSizeWnd->SetWindowText(text);
            if (requestLayout)
            {
                UpdateWindowLayout(pSizeWnd);
            }
        }
    }

    void RichEditFileOle::SetFileStateString(const SStringW& str)
    {
        SWindow * pWnd = _oleView.FindChildByName(L"LblState");
        if (pWnd)
        {
            pWnd->SetWindowText(str);
        }
    }

    void RichEditFileOle::SetFilePath(const SStringW& path)
    {
        _filePath = path;

        SWindow * pWnd = _oleView.FindChildByName(L"LblFileName");
        if (pWnd)
        {
            _fileName = path;
            int slash = _filePath.ReverseFind(_T('\\'));
            if (slash > 0)
            {
                _fileName = _filePath.Mid(slash + 1);
            }

            pWnd->SetWindowText(_fileName);
            pWnd->SetAttribute(L"tip", _fileName);
        }

        SImageWnd * pImageWin = static_cast<SImageWnd*>(_oleView.FindChildByName(L"ImgFileIcon"));
        if (pImageWin)
        {
            ISkinObj * pSkin = GetFileIconSkin(path);
            if (pSkin)
            {
                pImageWin->SetSkin(pSkin);
                pSkin->Release();
            }
        }
    }

	void RichEditFileOle::SetFileSuffix(const SStringW& suffix)
	{
		_fileSuffix = suffix;
		SWindow * pWnd = _oleView.FindChildByName(L"file_suffix");
		if (pWnd)
		{
			SStatic* pTextSuffix = pWnd->FindChildByName2<SStatic>(L"FileSuffix");
			if (pTextSuffix)
			{
				pTextSuffix->SetWindowText(_fileSuffix);
			}
		}
	}

	void RichEditFileOle::SetFileUrl(const SStringW& url)
	{
		_fileUrl = url;
	}

	SStringW RichEditFileOle::GetFileUrl()
	{
		return _fileUrl;
	}

    void RichEditFileOle::SetFileLinksVisible(int links)
    {
        SWindow * pWnd = NULL;
        BOOL visible = FALSE;

        if (pWnd = _oleView.FindChildByName(L"LnkSaveFile"))
        {
            visible = (links & LINK_SAVE) != 0;
            pWnd->SetVisible(visible);
        }

        if (pWnd = _oleView.FindChildByName(L"LnkSaveFileAs"))
        {
            visible = (links & LINK_SAVEAS) != 0;
            pWnd->SetVisible(visible);
        }

        if (pWnd = _oleView.FindChildByName(L"LnkCancelFile"))
        {
            visible = (links & LINK_CANCEL) != 0;
            pWnd->SetVisible(visible);
        }

        if (pWnd = _oleView.FindChildByName(L"LnkOpenFile"))
        {
            visible = (links & LINK_OPEN_FILE) != 0;
            pWnd->SetVisible(visible);
        }

        if (pWnd = _oleView.FindChildByName(L"LnkOpenFileDir"))
        {
            visible = (links & LINK_OPEN_DIR) != 0;
            pWnd->SetVisible(visible);
        }

        if (pWnd = _oleView.FindChildByName(L"LnkContinueFile"))
        {
            visible = (links & LINK_CONTINUE) != 0;
            pWnd->SetVisible(visible);
        }

        if (pWnd = _oleView.FindChildByName(L"LnkForwardFile"))
        {
            visible = (links & LINK_FORWARD) != 0;
            pWnd->SetVisible(visible);
        }

		if (pWnd = _oleView.FindChildByName(L"LnkDownLoad"))
		{
			visible = (links & LINK_DOWNLOAD) != 0;
			pWnd->SetVisible(visible);
		}

        pWnd = _oleView.FindChildByName(L"WndLinksContainer");
        UpdateWindowLayout(pWnd);
    }
	*/

    //////////////////////////////////////////////////////////////////////////
    //
    // {C0402A65-5BBF-4bef-9861-C55CD3A07201}
    static const GUID IID_FetchMoreOleCtrl =
    { 0xc0402a65, 0x5bbf, 0x4bef, { 0x98, 0x61, 0xc5, 0x5c, 0xd3, 0xa0, 0x72, 0x1 } };

    RichEditFetchMoreOle::RichEditFetchMoreOle()
    {
        _oleGuid = IID_FetchMoreOleCtrl;
        _sizeNatural.cx = 95;
        _sizeNatural.cy = 25;
        _xmlLayout = L"LAYOUT:FetchMoreOleLayout";
        _state = REFM_STATE_NORMAL;
    }

    BOOL RichEditFetchMoreOle::InitOleWindow(IRichEditObjHost * pHost)
    {
        BOOL ret = __super::InitOleWindow(pHost);

        SLink * pLink = static_cast<SLink*>(_oleView.FindChildByName(L"LnkFetchMore"));
        if (pLink)
        {
            SUBSCRIBE(pLink, EVT_CMD, RichEditFetchMoreOle::OnClickFetchMore);
        }

        pLink = static_cast<SLink*>(_oleView.FindChildByName(L"LnkOpenHistory"));
        if (pLink)
        {
            SUBSCRIBE(pLink, EVT_CMD, RichEditFetchMoreOle::OnClickOpenHistory);
        }

        return ret;
    }

    void RichEditFetchMoreOle::ResetState()
    {
        _state = REFM_STATE_NORMAL;
        SWindow * pWnd = static_cast<SWindow*>(_oleView.FindChildByName(L"WndFetchMore"));
        if (pWnd)
        {
            pWnd->SetVisible(TRUE);
        }

        pWnd = static_cast<SWindow*>(_oleView.FindChildByName(L"WndOpenHistory"));
        if (pWnd)
        {
            pWnd->SetVisible(FALSE);
        }

        SImageView* pImageView = _oleView.FindChildByName2<SImageView>(L"ImgLoadingMore");
        if (pImageView)
        {
            pImageView->Pause();
            pImageView->SetVisible(FALSE);
        }
    }


    void RichEditFetchMoreOle::ShowLoadingState()
    {
        _state = REFM_STATE_LOADING;

        SWindow * pWnd = static_cast<SWindow*>(_oleView.FindChildByName(L"WndFetchMore"));
        if (pWnd)
        {
            pWnd->SetVisible(FALSE);
        }

        pWnd = static_cast<SWindow*>(_oleView.FindChildByName(L"WndOpenHistory"));
        if (pWnd)
        {
            pWnd->SetVisible(FALSE);
        }

        //SAntialiasSkin* pSkin = ImageCache::getSingleton().GetImage(IMAGE_RE_LOADING);
        ISkinObj* pSkin = GETSKIN(L"skin.richedit.loading", 100);
        SImageView* pImageView = _oleView.FindChildByName2<SImageView>(L"ImgLoadingMore");

        if (pImageView && pSkin)
        {
            pImageView->SetSkin(pSkin);
            pImageView->SetVisible(TRUE, TRUE);
            pImageView->Resume();
        }
    }

    void RichEditFetchMoreOle::ShowOpenLinkState()
    {
        _state = REFM_STATE_END;

		SImageView* pImageView = _oleView.FindChildByName2<SImageView>(L"ImgLoadingMore");
		if (pImageView)
		{
			pImageView->Pause();
			pImageView->SetVisible(FALSE);
		}

        SWindow * pWnd = static_cast<SWindow*>(_oleView.FindChildByName(L"WndFetchMore"));
        if (pWnd)
        {
            pWnd->SetVisible(FALSE);
        }

        pWnd = static_cast<SWindow*>(_oleView.FindChildByName(L"WndOpenHistory"));
        if (pWnd)
        {
            pWnd->SetVisible(TRUE);
        }
    }

    void RichEditFetchMoreOle::HideOle()
    {
        SWindow* pWnd = _oleView.FindChildByName(L"WndParent");
        if (pWnd)
        {
            pWnd->SetVisible(FALSE, TRUE);
        }
    }

    bool RichEditFetchMoreOle::OnClickFetchMore(SOUI::EventArgs *pEvt)
    {
        ShowLoadingState();
        _pObjHost->NotifyRichObjEvent(this, CLICK_FETCHMOREOLE_MORE_MSG, 0, 0);
        return true;
    }

    bool RichEditFetchMoreOle::OnClickOpenHistory(SOUI::EventArgs *pEvt)
    {
        ShowOpenLinkState();
        _pObjHost->NotifyRichObjEvent(this, CLICK_FETCHMOREOLE_OPEN_LINK, 0, 0);
        return true;
    }

    void RichEditFetchMoreOle::Subscribe(const ISlotFunctor & subscriber)
    {
        SLink * pLink = static_cast<SLink*>(_oleView.FindChildByName(L"LnkFetchMore"));
        if (pLink)
        {
            SUBSCRIBE(pLink, EVT_CMD, RichEditFetchMoreOle::OnClickFetchMore);
        }
    }

    //LRESULT RichEditFetchMoreOle::ProcessMessage(UINT msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    //{
    //    RichEditOleBase::ProcessMessage(msg, wParam, lParam, bHandled);
    //    if (msg == WM_LBUTTONDOWN)
    //    {
    //        bHandled = TRUE; // 查看更多OLE不让RichEdit继续左击，否则会画出一个黑框
    //    }
    //    return 0;
    //}

    void RichEditFetchMoreOle::UpdatePosition()
    {
        if (_spAdviseSink)
        {
            CRect rcHost = _pObjHost->GetHostRect();
            _sizeNatural.cx = rcHost.Width();

            CalculateExtentSize(_sizeNatural);
            _oleView.SetOleWindowRect(CRect(0, 0, 0, 0)); // 已经失效
            _oleView.Move(0, 0, _sizeNatural.cx, _sizeNatural.cy);
            _spAdviseSink->OnViewChange(DVASPECT_CONTENT, -1);
        }
    }

    //
    //-------------------------------------------------------------------------
    // RichSeparatorBar
    //
    // {8D5E6EF1-2A36-4930-91E7-1149303994BF}
    static const GUID IID_SeparatorBarCtrl =
    { 0x8d5e6ef1, 0x2a36, 0x4930, { 0x91, 0xe7, 0x11, 0x49, 0x30, 0x39, 0x94, 0xbf } };

    RichEditSeparatorBar::RichEditSeparatorBar()
    {
        _oleGuid = IID_SeparatorBarCtrl;
        _sizeNatural.cx = 95;
        _sizeNatural.cy = 25;
        _xmlLayout = L"LAYOUT:SeperatorOleLayout";
    }

    void RichEditSeparatorBar::UpdatePosition()
    {
        if (_spAdviseSink)
        {
            _sizeNatural.cx = _pObjHost->GetHostRect().Width();
            _oleView.SetOleWindowRect(CRect(0, 0, 0, 0)); // 已经失效
            _oleView.Move(0, 0, _sizeNatural.cx, _sizeNatural.cy);
            CalculateExtentSize(_sizeNatural);
            _spAdviseSink->OnViewChange(DVASPECT_CONTENT, -1);
        }
    }

    //LRESULT RichEditSeparatorBar::ProcessMessage(UINT msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    //{
    //    if (msg == WM_LBUTTONDOWN)
    //    {
    //        bHandled = TRUE; // 分隔栏OLE不让RichEdit继续左击，否则会画出一个黑框
    //    }
    //    return 0;
    //}

    //////////////////////////////////////////////////////////////////////////
    //
    // {A5D897A6-4533-4e15-9A33-A5814F927768}
    static const GUID IID_RemainderOleCtrl =
    { 0xa5d897a6, 0x4533, 0x4e15, { 0x9a, 0x33, 0xa5, 0x81, 0x4f, 0x92, 0x77, 0x68 } };

    RichEditReminderOle::RichEditReminderOle() :
        _borderColor(CR_INVALID)
        , _bkCorlor(CR_INVALID)
        , _textColor(CR_INVALID)
        , _borderWidth(0)
        , _font(L"size:12")
        , _height(0)
    {
        _oleGuid = IID_RemainderOleCtrl;
        _maxSize.cx = _maxSize.cy = 150;
        _xmlLayout = L"LAYOUT:RemainderOleLayout";
    }

    HRESULT RichEditReminderOle::QueryInterface(REFIID riid, void ** ppvObject)
    {
        if (riid == IID_RemainderOleCtrl)
        {
            *ppvObject = this;
            AddRef();
            return S_OK;
        }

        return RichEditOleBase::QueryInterface(riid, ppvObject);
    }

    BOOL RichEditReminderOle::InitFromXml(pugi::xml_node xmlNode)
    {
        _text = xmlNode.text().get();
        return __super::InitFromXml(xmlNode);
    }

    SStringW RichEditReminderOle::GetSelFormatedText()
    {
        SStringW formattedText;
        formattedText.Format(_T("<%s ><![CDATA["), RichEditReminderOle::GetClassName());
        formattedText += _text;
        formattedText += SStringW().Format(_T("]]></%s>"), RichEditReminderOle::GetClassName());

        return formattedText;
    }

    SStringW RichEditReminderOle::MakeFormattedText(const SStringW& text,
        int textSize,
        COLORREF textColor)
    {
        SStringW color;
        color.Format(_T("#%02x%02x%02x"), GetRValue(textColor), GetGValue(textColor), GetBValue(textColor));

        SStringW formattedText;
        formattedText.Format(
            L"<%s color-text=\"%s\" font=\"size:%d\"><![CDATA[%s]]></%s>",
            RichEditReminderOle::GetClassName(),
            color,
            textSize,
            text,
            RichEditReminderOle::GetClassName());

        return formattedText;
    }

    void RichEditReminderOle::CalculateNatureSize()
    {
        CAutoRefPtr<IRenderTarget> pRT;
        GETRENDERFACTORY->CreateRenderTarget(&pRT, 0, 0);
        pRT->SelectObject(SFontPool::getSingleton().GetFont(_font, GetScale()));
        pRT->MeasureText(_text, _text.GetLength(), &_sizeNatural);

        if (_height != 0)
        {
            _sizeNatural.cy = _height;
        }

        _sizeNatural.cx += _borderWidth * 2;
        _sizeNatural.cy += _borderWidth * 2;

        if (_sizeNatural.cx > _maxSize.cx)
        {
            _sizeNatural.cx = _maxSize.cx;
        }
        if (_sizeNatural.cy > _maxSize.cy)
        {
            _sizeNatural.cy = _maxSize.cy;
        }
    }

    void RichEditReminderOle::InitAttributes()
    {
        SWindow * pText = _oleView.FindChildByName2<SStatic>(L"LblText");
        if (pText)
        {
            pText->SetWindowText(_text);
            pText->SetAttribute(L"font", _font);
            pText->GetStyle().SetTextColor(0, _textColor);
            pText->GetStyle().m_crBorder = _borderColor;
            pText->GetStyle().m_crBg = _bkCorlor;
            pText->GetStyle().SetAttribute(L"margin", SStringW().Format(L"%d,%d,%d,%d", _borderWidth, _borderWidth, _borderWidth, _borderWidth));
        }
    }

    BOOL RichEditReminderOle::InitOleWindow(IRichEditObjHost * pHost)
    {
        CalculateNatureSize();
        BOOL ret = RichEditOleBase::InitOleWindow(pHost);
        InitAttributes();
        return ret;
    }

    //
    // ------------------------------------------------------------------------------
    // impl RichMetaOleFile
    //
    // {3AAFAC01-0C3D-447a-AEDB-8C88FA1CF7B5}
    static const GUID IID_RichMetaFileOle =
    { 0x3aafac01, 0xc3d, 0x447a, { 0xae, 0xdb, 0x8c, 0x88, 0xfa, 0x1c, 0xf7, 0xb5 } };

    RichEditMetaFileOle::RichEditMetaFileOle() : _font(L"size:12")
    {
        _sizeNatural.cx = OLE_MIN_WIDTH;
        _sizeNatural.cy = OLE_HEIGHT;

        _oleGuid = IID_RichMetaFileOle;
        _xmlLayout = L"LAYOUT:MetaFileOleLayout";
    }

    void RichEditMetaFileOle::CalculateNatureSize(const SStringW& FileName)
    {
        CAutoRefPtr<IRenderTarget> pRT;
        GETRENDERFACTORY->CreateRenderTarget(&pRT, 0, 0);

        SIZE sizeText = { 0, 0 };
        pRT->SelectObject(SFontPool::getSingleton().GetFont(_font, GetScale()));
        pRT->MeasureText(FileName, FileName.GetLength(), &sizeText);

        _sizeNatural.cx = sizeText.cx;
        _sizeNatural.cx += 10;

        if (_sizeNatural.cx > OLE_MAX_WIDTH)
        {
            _sizeNatural.cx = OLE_MAX_WIDTH;
        }
        else if (_sizeNatural.cx < OLE_MIN_WIDTH)
        {
            _sizeNatural.cx = OLE_MIN_WIDTH;
        }
    }

    SStringW RichEditMetaFileOle::GetSelFormatedText()
    {
        if (_filePath.IsEmpty())
        {
            return SStringW();
        }

        return SStringW().Format(L"<metafile file=\"%s\" />", XMLEscape(_filePath));
    }

    BOOL RichEditMetaFileOle::InitOleWindow(IRichEditObjHost* pHost)
    {
        BOOL ret = RichEditOleBase::InitOleWindow(pHost);

        SetFilePath(_filePath);

        return ret;
    }

    void RichEditMetaFileOle::SetFilePath(LPCWSTR lpszFilePath)
    {
        _filePath = lpszFilePath;

        //
        // extract file name
        //

        SStringW FileName;

        if (!_filePath.IsEmpty())
        {
            int slash = _filePath.ReverseFind(TCHAR('\\'));

            if (slash >= 0)
            {
                FileName =
                    _filePath.Right(_filePath.GetLength() - slash - 1);
            }
        }

        //
        // update ui elements
        //

        SetFileName(FileName);
        LoadFileIcon();

        //
        // recalc size
        //

        CalculateNatureSize(FileName);
        CalculateExtentSize(_sizeNatural);

        _oleView.Move(0, 0, _sizeNatural.cx, _sizeNatural.cy);

        if (_spAdviseSink)
        {
            _spAdviseSink->OnViewChange(DVASPECT_CONTENT, -1);
        }
    }

    void RichEditMetaFileOle::SetFileName(LPCWSTR lpszFileName)
    {
        SWindow * pWin = _oleView.FindChildByName(L"LblFileName");

        if (pWin)
        {
            pWin->SetWindowText(lpszFileName);
        }
    }

    SStringW RichEditMetaFileOle::GetFilePath()
    {
        return _filePath;
    }

    BOOL RichEditMetaFileOle::LoadFileIcon()
    {
        SImageWnd * pWin = static_cast<SImageWnd*>(_oleView.FindChildByName(L"ImgFile"));

        if (!pWin)
        {
            return FALSE;
        }

        ISkinObj * pSkin = GetFileIconSkin(_filePath);
        if (!pSkin)
        {
            return FALSE;
        }

        pWin->SetSkin(pSkin);

        return TRUE;
    }

    LRESULT RichEditMetaFileOle::ProcessMessage(UINT msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        RichEditOleBase::ProcessMessage(msg, wParam, lParam, bHandled);
        if (msg == WM_LBUTTONDBLCLK)
        {
            _pObjHost->NotifyRichObjEvent(this, DBLCLICK_RICH_METAFILE, 0, 0);
            bHandled = TRUE; // 不希望双击事件继续往richedit传递
        }

        return 0;
    }

} // namespace SOUI
