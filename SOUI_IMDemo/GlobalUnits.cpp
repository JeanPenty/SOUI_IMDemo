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

void CGlobalUnits::OperatePinyinMap(const std::string& strPath)
{
	m_PinyinMap.clear();

	std::ifstream  fin(strPath.c_str(), std::ios::in); 
	char  line[1024] = {0};
	while(fin.getline(line, sizeof(line)))  
	{
		std::string strLine = line;
		std::string strKey, strValue;
		std::string::size_type pos;
		pos = strLine.find_first_of(' ');
		if (std::string::npos != pos)
		{
			strKey = strLine.substr(0, pos);
			strValue = strLine.substr(pos + 1);
			std::wstring wstrKey, wstrValue;

			SStringW sstrKey = S_CA2W(strKey.c_str());
			SStringW sstrValue = S_CA2W(strValue.c_str());
			wstrKey = sstrKey;
			wstrValue = sstrValue;
			m_PinyinMap.insert(std::make_pair(wstrKey, wstrValue));
		}
	}
}

void CGlobalUnits::OperateSerachIndex()
{
	//������������

	//personal 
	PersonalsMap::iterator iterPersonal = m_mapPersonals.begin();
	for (; iterPersonal != m_mapPersonals.end(); iterPersonal++)
	{
		SStringW sstrName = S_CA2W(iterPersonal->second.m_strName.c_str());
		std::wstring wstrName = sstrName;
		m_mapChineseSearch.insert(std::make_pair(wstrName, SEARCH_INFO(1, iterPersonal->second.m_strID)));

		std::wstring wstrSimple, wstrFull, wstrEncode;
		wstrEncode = EncodeChinese(wstrName.c_str());
		GetSimpleAndFull(wstrEncode, wstrSimple, wstrFull);

		m_mapPinyinSearch.insert(std::make_pair(wstrSimple, SEARCH_INFO(1, iterPersonal->second.m_strID)));
		m_mapPinyinSearch.insert(std::make_pair(wstrFull, SEARCH_INFO(1, iterPersonal->second.m_strID)));
	}

	//group
	GroupsMap::iterator iterGroup = m_mapGroups.begin();
	for (; iterGroup != m_mapGroups.end(); iterGroup++)
	{
		SStringW sstrName = S_CA2W(iterGroup->second.m_strGroupName.c_str());
		std::wstring wstrName = sstrName;
		m_mapChineseSearch.insert(std::make_pair(wstrName, SEARCH_INFO(2, iterGroup->second.m_strGroupID)));

		std::wstring wstrSimple, wstrFull, wstrEncode;
		wstrEncode = EncodeChinese(wstrName.c_str());
		GetSimpleAndFull(wstrEncode, wstrSimple, wstrFull);

		m_mapPinyinSearch.insert(std::make_pair(wstrSimple, SEARCH_INFO(1, iterGroup->second.m_strGroupID)));
		m_mapPinyinSearch.insert(std::make_pair(wstrFull, SEARCH_INFO(1, iterGroup->second.m_strGroupID)));
	}
}

void CGlobalUnits::OperateEmojis()
{
	/*	2019-03-26	yangjinpeng
	*	������Ŀ¼�µ�emojiͼƬ
	*/
	std::string strPath = "emojis\\*.png";
	long handle;
	struct _finddata_t fileinfo;
	handle = _findfirst(strPath.c_str(), &fileinfo);
	if(handle == -1)
		return;
	do
	{
		std::string strUUID = GenerateUUID();
		m_mapEmojisIndex.insert(std::make_pair(strUUID, fileinfo.name));

		std::string strTempPath;
		strTempPath.append("emojis\\");
		strTempPath.append(fileinfo.name);
		SStringW sstrPath = S_CA2W(strTempPath.c_str());
		IBitmap* pRes = SResLoadFromFile::LoadImage(sstrPath);
		if (pRes)
			m_mapFace.insert(std::make_pair(strUUID, pRes));

	} while (!_findnext(handle, &fileinfo));
	_findclose(handle);
}

std::wstring CGlobalUnits::EncodeChinese(const std::wstring& wstrSrc)
{
	int cur;
	SStringW sstrSrc, sstrTemp, sstrReturn;
	sstrSrc = wstrSrc.c_str();

	for (int i = 0; i < sstrSrc.GetLength(); i++)
	{
		cur = sstrSrc.GetAt(i);
		sstrTemp.Format(L"%04X", cur);
		sstrReturn = sstrReturn + sstrTemp;
	}

	std::wstring wstrReturn = sstrReturn;
	return wstrReturn;
}

std::wstring CGlobalUnits::DecodeChinese(const std::wstring& wstrSrc)
{
	SStringW sstrSrc, sstrTemp, sstrReturn;
	sstrSrc = wstrSrc.c_str();

	for (int i = 0; i < sstrSrc.GetLength(); i += 4)
	{
		SStringW sstr = sstrSrc.Mid(i, 4);
		wchar_t *str1 = sstr.GetBuffer(sstr.GetLength() + 2);
		int x = (int)_tcstol(sstr, &str1, 16);//ʮ������ 

		sstrTemp.Format(L"%c", x);
		sstrReturn = sstrReturn + sstrTemp;
	}

	std::wstring wstrReturn = sstrReturn;
	return wstrReturn;
}

BOOL CGlobalUnits::GetSimpleAndFull(const std::wstring& wstrSrc, std::wstring &SimpleStr, std::wstring &FullStr)
{
	if (wstrSrc.empty())
		return FALSE;

	SStringW sstrSrc, sstrSimple, sstrFull, sstrTemp;
	sstrSrc = wstrSrc.c_str();
	for (int i = 0; i < sstrSrc.GetLength(); i += 4)
	{
		SStringW sstr = sstrSrc.Mid(i, 4);
		std::multimap<std::wstring, std::wstring>::iterator iter = m_PinyinMap.find((const wchar_t*)sstr);
		if (iter != m_PinyinMap.end())
			sstrTemp = iter->second.c_str();

		sstrFull = sstrFull + sstrTemp;
		sstrSimple = sstrSimple + sstrTemp.Left(1);
	}

	SimpleStr = sstrSimple;
	FullStr = sstrFull;

	return TRUE;
}

BOOL CGlobalUnits::IsIncludeChinese(std::wstring& wstrSrc)
{
	std::string strSrc = S_CW2A(wstrSrc.c_str());
	char* str = (char*)strSrc.c_str();
	char c;
	while(1)
	{
		c=*str++;
		if (c==0) break;  //������ַ���β��˵�����ַ���û�������ַ�
		if (c&0x80)       //����ַ���λΪ1����һ�ַ���λҲ��1���������ַ�
			if (*str & 0x80) 
				return TRUE;
	}
	return FALSE;
}