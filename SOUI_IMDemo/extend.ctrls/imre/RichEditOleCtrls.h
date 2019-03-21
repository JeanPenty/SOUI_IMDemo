// ------------------------------------------------------------------------------
//
// RichEditOleCtrls.h : interface of the RichEditOleCtrls class
//
// ��RichEdit OLE�ؼ����ͷ�ļ����壬�����������������¶���
// 
// - ͼƬ
// - �ļ���Ϣ
// - ��������ʷ��Ϣ
// - ��ȡ����
// - �ı�
// - @����Ϣ
//
// ------------------------------------------------------------------------------


#pragma once
#include "RichEditOleBase.h"
#include <vector>
#include "../SImageView.h"

namespace SOUI
{
    // ------------------------------------------------------------------------------
    //
    // RichEditImageOle
    //
    // ------------------------------------------------------------------------------
    extern "C" const GUID IID_ImageOleCtrl;

    class RichEditImageOle : public RichEditOleBase
    {
        DECLARE_REOBJ(RichEditImageOle, L"img")

    public:

        static SStringW TagPath;

        static SStringW MakeFormattedText(const SStringW& imageId,
            const SStringW& subId,
            const SStringW& type,
            const SStringW& skinId,
            const SStringW& imagePath,
            const SStringW& encoding,
            BOOL showManifier);

        RichEditImageOle();
        ~RichEditImageOle();

        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppvObject);
        HRESULT STDMETHODCALLTYPE Close(DWORD dwSaveOption);
        HRESULT STDMETHODCALLTYPE Draw(
            DWORD dwDrawAspect, LONG lindex,
            void *pvAspect,
            DVTARGETDEVICE *ptd,
            HDC hdcTargetDev,
            HDC hdcDraw,
            LPCRECTL lprcBounds,
            LPCRECTL lprcWBounds,
            BOOL(STDMETHODCALLTYPE *pfnContinue)(ULONG_PTR dwContinue),
            ULONG_PTR dwContinue);

        SStringW    GetSelFormatedText();
        void        ShowManifier(BOOL show);
        BOOL        SetImagePath(const SStringW& path, const SStringW& skinId);
        BOOL        SetImageSkin(const SStringW& skin);
        SStringW    GetImagePath();
        SStringW    GetImageSkin();
        SStringW    GetImageType();
        SStringW    GetEncoding();
        BOOL        InitOleWindow(IRichEditObjHost* pHost);
        HRESULT     InternalDraw(IRenderTarget* prt, IRegion * prgn, DWORD cp);

    protected:

        BOOL SetImageSkin(ISkinObj* pSkin);
        BOOL DownLoadNetworkFile(const SStringW& url, SStringW& path);
        bool OnImageLoaded(SOUI::EventArgs *pEvt);
        LRESULT ProcessMessage(UINT msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

        SOUI_ATTRS_BEGIN()
            ATTR_SIZE(L"max-size", _maxSize, FALSE)
            ATTR_STRINGW(L"path", _path, FALSE)
            ATTR_STRINGW(L"subid", _subId, FALSE)
            ATTR_STRINGW(L"type", _imageType, FALSE)
            ATTR_STRINGW(L"skin", _skinId, FALSE)
            ATTR_STRINGW(L"encoding", _encoding, FALSE)
            ATTR_INT(L"show-magnifier", _showMagnifier, FALSE)
            SOUI_ATTRS_END()

    private:

        SIZE            _maxSize;
        SStringW        _path;
        SStringW        _skinId;
        SImageView *    _pImageView;
        BOOL            _isClosed;
        SStringW        _imageType;
        SStringW        _encoding;
        SStringW        _subId;
        BOOL            _showMagnifier;
    };

	//-------------------------------------------------------------------------------
	//
	//	RichEditAudioOle
	//
	//-------------------------------------------------------------------------------
	extern "C" const GUID IID_AudioOleCtrl;
	class RichEditAudioOle : public RichEditOleBase
	{
		DECLARE_REOBJ(RichEditAudioOle, L"audio")
	public:
		enum AudioPlayFlag
		{
			AUDIO_PLAY_NO = 0,
			AUDIO_PLAY_PLAYING,
			AUDIO_PLAY_PLAYFINISHED,
		};

		RichEditAudioOle();
		~RichEditAudioOle();
		BOOL InitOleWindow(IRichEditObjHost* pHost);

	private:
		int			_state;
		SIZE        _maxSize;
		SStringW	_audio_duration;
		SStringW	_audio_url;
		SStringW	_audio_layout;
		SStringW	_audio_isplayed;
	};

    // ------------------------------------------------------------------------------
    //
    // RichEditFileOle
    //
    // ------------------------------------------------------------------------------
	extern "C" const GUID IID_FileOleCtrl;

	class RichEditFileOle : public RichEditOleBase
	{
		DECLARE_REOBJ(RichEditFileOle, L"file")

	public:

		enum LinkFlag
		{
			LINK_DOWNLOAD = 0x0001,	//����
			LINK_SAVE = 0x0002,   // ���水ť
			LINK_SAVEAS = 0x0004,   // ���Ϊ��ť
			LINK_CANCEL = 0x0008,   // ȡ�����䰴ť
			LINK_OPEN_FILE = 0x0010,   // ���ļ���ť
			LINK_OPEN_DIR = 0x0020,   // ���ļ������ļ��а�ť
			LINK_CONTINUE = 0x0040,   // �������䰴ť
			LINK_FORWARD = 0x0080,   //  ת��
		};

		static SStringW MakeFormattedText(
			const SStringW& filePath,
			const SStringW& fileState,
			__int64 fileSize,
			int visibleLinks,
			const SStringW& fileSuffix);

		RichEditFileOle();
		~RichEditFileOle();

		virtual BOOL InitOleWindow(IRichEditObjHost* pHost);

		void SetFilePath(const SStringW& path);
		void SetFileLinksVisible(int links);
		void SetFileSize(__int64 size, BOOL requestLayout = TRUE);
		void SetFileStateString(const SStringW& str);
		SStringW GetFilePath();
		SStringW GetFileName();
		__int64 GetFileSize();
		SStringW GetFileSuffix();
		void SetFileSuffix(const SStringW& suffix);
		void SetFileUrl(const SStringW& url);
		SStringW GetFileUrl();
		SStringW GetFileOLeId(){return GetId();}

	protected:

		bool OnLinkClicked(SOUI::EventArgs *pEvt);
		bool OnFileNameClicked(SOUI::EventArgs* pEvt);

		SOUI_ATTRS_BEGIN()
			ATTR_STRINGW(L"file-path", _filePath, FALSE)
			ATTR_STRINGW(L"file-size", _fileSize, FALSE)
			ATTR_STRINGW(L"file-state", _fileState, FALSE)
			ATTR_INT(L"links", _links, FALSE)
			ATTR_STRINGW(L"file-suffix", _fileSuffix, FALSE)
			ATTR_STRINGW(L"file-url", _fileUrl, FALSE)
			SOUI_ATTRS_END()

			SStringW GetSizeBeautyString(unsigned long long size);

	private:

		SStringW _filePath;         // �ļ�·��
		SStringW _fileName;         // �ļ���
		SStringW _fileSize;         // �ļ��ߴ�
		SStringW _fileState;        // �ļ�״̬
		__int64  _fileSizeBytes;    // ���ε��ļ���С
		int      _links;            // �ɼ���ť�ı�ǣ���LinkFlag
		SStringW _fileSuffix;
		SStringW _fileUrl;
	};

    //
    // RichEditFileOle inlines
    //

    inline SStringW RichEditFileOle::GetFilePath()
    {
        return _filePath;
    }

    inline SStringW RichEditFileOle::GetFileName()
    {
        return _fileName;
    }

    // ------------------------------------------------------------------------------
    //
    // RichEditFetchMoreOle
    //
    // ------------------------------------------------------------------------------
    extern "C" const GUID IID_FetchMoreOleCtrl;

    class RichEditFetchMoreOle : public RichEditOleBase
    {
        DECLARE_REOBJ(RichEditFetchMoreOle, L"fetchmore")

    public:

        enum FetchMoreState
        {
            REFM_STATE_NORMAL,  // 正常状态，显示获取更多
            REFM_STATE_LOADING, // 正在加载状态，显示转圈�?
            REFM_STATE_END,     // 显示更多消息请在历史记录中查�?
        };

        RichEditFetchMoreOle();

        BOOL    InitOleWindow(IRichEditObjHost * pHost);
        void    ResetState();
        void    ShowLoadingState();
        void    ShowOpenLinkState();
        void    HideOle();
        void    Subscribe(const ISlotFunctor & subscriber);
        int     GetCurrentState() { return _state; }

    protected:

        bool    OnClickFetchMore(SOUI::EventArgs *pEvt);
        bool    OnClickOpenHistory(SOUI::EventArgs *pEvt);
        //LRESULT ProcessMessage(UINT msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        void    UpdatePosition();

    private:

        int _state;
    };

    // ------------------------------------------------------------------------------
    //
    // RichEditSeparatorBar
    //
    // ------------------------------------------------------------------------------
    extern "C" const GUID IID_SeparatorBarCtrl;

    class RichEditSeparatorBar : public RichEditOleBase
    {
        DECLARE_REOBJ(RichEditSeparatorBar, L"split")

    public:
        RichEditSeparatorBar();

    protected:
        //LRESULT ProcessMessage(UINT msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        void    UpdatePosition();
    };

    // ------------------------------------------------------------------------------
    //
    // RichEditReminderOle
    //
    // ------------------------------------------------------------------------------
    extern "C" const GUID IID_RemainderOleCtrl;

    class RichEditReminderOle : public RichEditOleBase
    {
        DECLARE_REOBJ(RichEditReminderOle, L"remainder")

    public:

        static SStringW MakeFormattedText(const SStringW& text,
            int textSize,
            COLORREF textColor);

        RichEditReminderOle();

        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppvObject);

        BOOL        InitFromXml(pugi::xml_node xmlNode);
        BOOL        InitOleWindow(IRichEditObjHost* pHost);
        SStringW    GetText() { return _text; }
        SStringW    GetSelFormatedText();

    protected:

        void    CalculateNatureSize();
        void    InitAttributes();

        SOUI_ATTRS_BEGIN()
            ATTR_COLOR(L"color-border", _borderColor, FALSE)
            ATTR_COLOR(L"color-bkgnd", _bkCorlor, FALSE)
            ATTR_COLOR(L"color-text", _textColor, FALSE)
            ATTR_INT(L"border-width", _borderWidth, FALSE)
            ATTR_STRINGW(L"font", _font, FALSE)
            ATTR_SIZE(L"max-size", _maxSize, FALSE)
            ATTR_INT(L"height", _height, FALSE)
            SOUI_ATTRS_END()

    private:

        COLORREF    _borderColor;
        COLORREF    _bkCorlor;
        COLORREF    _textColor;
        int         _borderWidth;
        SStringW    _text;
        SStringW    _font;
        SIZE        _maxSize;
        int         _height;
    };

    // ------------------------------------------------------------------------------
    //
    // RichMetaFile
    //
    // ------------------------------------------------------------------------------
    extern "C" const GUID IID_RichMetaFileOle;

    class RichEditMetaFileOle : public RichEditOleBase
    {
#define OLE_MAX_WIDTH 120
#define OLE_MIN_WIDTH 45
#define OLE_HEIGHT    65

        DECLARE_REOBJ(RichEditMetaFileOle, L"metafile")

    public:

        RichEditMetaFileOle();

        virtual SStringW GetSelFormatedText();
        virtual BOOL InitOleWindow(IRichEditObjHost* pHost);

        void SetFilePath(LPCWSTR lpszFileName);
        SStringW GetFilePath();

    protected:

        void    SetFileName(LPCWSTR lpszFileName);
        void    CalculateNatureSize(const SStringW& FileName);
        BOOL    LoadFileIcon();
        bool    OnOleWindowEvent(SOUI::EventArgs *pEvt);
        LRESULT ProcessMessage(UINT msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

        SOUI_ATTRS_BEGIN()
            ATTR_STRINGW(L"file", _filePath, FALSE)
            ATTR_STRINGW(L"font", _font, FALSE)
            SOUI_ATTRS_END()

    private:

        SStringW _filePath;
        SStringW _font;
    };

} // namespace SOUI
