#pragma once

class CWindowEnumer
{
	CWindowEnumer();
	~CWindowEnumer();
	static std::vector<CRect> rcList;
public:	
	//��ȡ������㴰�ڲ���ȡ����
	static void EnumAllTopWindow()
	{
		//��ȡ��Ļ�����еĶ��㴰��,ÿ����һ�����ھ͵��ûص�����һ��
		HWND hWnd = GetDesktopWindow();        //�õ����洰��
		hWnd = GetWindow(hWnd, GW_CHILD);        //�õ���Ļ�ϵ�һ���Ӵ���
		rcList.clear();
		while (hWnd != NULL)                    //ѭ���õ����е��Ӵ���
		{
			if (IsWindowVisible(hWnd))  //�ж��Ƿ񶥲㴰�ڲ��ҿɼ�
			{
				CRect WindowRc;
				GetWindowRect(hWnd,&WindowRc);
				if(!WindowRc.IsRectEmpty())
					rcList.push_back(WindowRc);
			}
			hWnd = GetNextWindow(hWnd, GW_HWNDNEXT);
		}
	}
	static CRect GetPointInRect(POINT pt)
	{/*
		for (auto &rc:rcList)
		{
			if (rc.PtInRect(pt))
				return rc;
		}*/
		for (std::vector<CRect>::const_iterator ite= rcList.begin();ite!=rcList.end();ite++)
		{
			if (ite->PtInRect(pt))
				return *ite;
		}

		return CRect();
	}
};

