#ifndef __EVENTS_DEFINE_H__
#define __EVENTS_DEFINE_H__

#pragma once
#include "stdafx.h"

//��ͼ����¼�
class EventSnapshotFinish : public TplEventArgs<EventSnapshotFinish>
{
	SOUI_CLASS_NAME(EventSnapshotFinish, L"evtsnapshot_finish")
public:
	EventSnapshotFinish(SObject* pSender)
		: TplEventArgs<EventSnapshotFinish>(pSender){}
	enum{EventID = EVT_SNAPSHOTFINISH};
};

#endif