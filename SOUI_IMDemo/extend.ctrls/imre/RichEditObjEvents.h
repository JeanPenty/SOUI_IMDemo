// ------------------------------------------------------------------------------
//
// RichEditObjEvents.h : 
//
// ��RichEdit������¼����塣��ole���ͣ�Ҳ���Զ����RichEditObj����
//
// ��ͷ�ļ�����ĺ��Ӧ EventRichEditObj.SubEventId �ֶ�, �¼�������ͨ��
// SubEventId��֪�¼�����������󷢳���
//
// ------------------------------------------------------------------------------


#pragma once

#define RICHOBJ_EVENT_BASE      0

#define DBLCLICK_IMAGEOLE               (RICHOBJ_EVENT_BASE+0)     // ˫��ͼƬole
#define CLICK_RICH_FETCHMORE            (RICHOBJ_EVENT_BASE+1)     // ����鿴����ole
#define DBLCLICK_RICH_METAFILE          (RICHOBJ_EVENT_BASE+2)     // ˫���ļ�ͼ��ole���������
#define CLICK_ARTICLEOLE                (RICHOBJ_EVENT_BASE+3)     // ���ͼ����Ϣ
#define CLICK_FILEOLE                   (RICHOBJ_EVENT_BASE+4)     // ������ļ�OLE��ĳ����ť
#define CLICK_FETCHMOREOLE_MORE_MSG     (RICHOBJ_EVENT_BASE+5)     // �����ȡ����OL��"�鿴������Ϣ"
#define CLICK_FETCHMOREOLE_OPEN_LINK    (RICHOBJ_EVENT_BASE+6)     // ����˻�ȡ����OLE��"����Ϣ��¼"
#define CLICK_LINK                      (RICHOBJ_EVENT_BASE+7)     // ������Լ�ʵ�ֵ�����
#define CLICK_VOICEOLE                  (RICHOBJ_EVENT_BASE+8)     // ���������OLE
#define CLICK_BK_ELE                    (RICHOBJ_EVENT_BASE+9)     // ����˱���Ԫ��
#define CLICK_EXTMSG_ELE                (RICHOBJ_EVENT_BASE+10)    // �������չ��ϢOLE
#define LBUTTONDOWN_IMAGE				(RICHOBJ_EVENT_BASE+11)
#define CLICK_AUDIO_PLAY				(RICHOBJ_EVENT_BASE+12)
#define CLICK_AUDIO_PLAYING				(RICHOBJ_EVENT_BASE+13)
#define CLICK_LOCATION_OLE				(RICHOBJ_EVENT_BASE+14)
#define CLICK_VIDEODOWNLOAD				(RICHOBJ_EVENT_BASE+15)
#define CLICK_VIDEOPLAY					(RICHOBJ_EVENT_BASE+16)
#define CLICK_LINK_OLE					(RICHOBJ_EVENT_BASE+17)