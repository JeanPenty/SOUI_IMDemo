#include "StdAfx.h"
#include "GlobalUnits.h"

CGlobalUnits::CGlobalUnits(void)
{
}

CGlobalUnits::~CGlobalUnits(void)
{
}


CGlobalUnits* CGlobalUnits::GetInstance()
{
	static CGlobalUnits _Instance;
	return &_Instance;
}

std::string CGlobalUnits::GenerateUUID()
{
	char szbuf[100];
	GUID guid;
	::CoCreateGuid(&guid);
	sprintf(szbuf,
		"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
		guid.Data1,
		guid.Data2,
		guid.Data3,
		guid.Data4[0], guid.Data4[1],
		guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5],
		guid.Data4[6], guid.Data4[7]);

	std::string strUUID = szbuf;
	return strUUID;
}

void CGlobalUnits::OperateShamDate()
{
	const char* shamAreas[] = {
		"����",
		"����",
		"�㶫",
		"����׳��������",
		"����",
		"�ӱ�",
		"ɽ��",
		"ɽ��",
		"������",
		"������",
		"�����",
		"�Ϻ���",
		"����",
		"����",
		"������",
		"�㽭",
		"�½�ά���������",
		"���Ļ���������",
		"����",
		"�ຣ",
		"����",
		"����",
		"����",
		"����",
		"����������",
		"�Ĵ�",
		"���ɹ�������",
		"̨��",
		"����",
		"����",
		"����",
		"����",
		"����ر�������",
		"�����ر�������",
	};

	const char* shamNames[] ={
		"����",
		"����",
		"����",
		"����",
		"��Ӣս����",
		"����׹Ǿ�",
		"���μ�",
		"ˮ䰴�",
		"��������",
		"��¥��",
		"��ƿ÷",
		"���㶨��"
	};

	const char* shamGroupNames[] ={
		"���ǿ϶�û��˧",
		"С�������氡",
		"һ�����Ǹ���Ƥ",
		"���ɶ",
		"����զ��",
		"�ٳ�һ������",
		"���Ծ�����",
		"��½",
		"��½",
		"���̨��",
		"�ɵ�����",
		"һ���й�"
	};

	//�����ϵ�˵ļ�����
	{
		for (int i = 0; i < 50; i++)
		{
			std::string strUUID = GenerateUUID();
			int nNameIndex = rand()%11;
			std::string strTempName = shamNames[nNameIndex];

			std::ostringstream os;
			os.str("");
			os<<strTempName<<i;
			std::string strName = os.str();
			int nAreaIndex = rand()%33;
			std::string strArea = shamAreas[nAreaIndex];
			
			m_mapPersonals.insert(std::make_pair(strUUID, 
				PERSONAL_INFO(strUUID, strName, "", strArea, "��֪��", "�������ڷ�Զ����������Ʒ�Ըߡ�")));
		}
	}

	//���Ⱥ�ļ�����
	{
		for (int i = 0; i < 12; i++)
		{
			std::string strUUID = GenerateUUID();
			int nNameIndex = rand()%11;
			std::string strTempName = shamGroupNames[nNameIndex];

			std::ostringstream os;
			os.str("");
			os<<strTempName<<i;
			std::string strName = os.str();

			m_mapGroups.insert(std::make_pair(strUUID, GROUP_INFO(strUUID, strName, "��һ�����")));
		}
	}
}