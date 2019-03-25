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
		"湖南",
		"湖北",
		"广东",
		"广西壮族自治区",
		"河南",
		"河北",
		"山东",
		"山西",
		"北京市",
		"重庆市",
		"天津市",
		"上海市",
		"江苏",
		"江西",
		"黑龙江",
		"浙江",
		"新疆维吾尔自治区",
		"宁夏回族自治区",
		"辽宁",
		"青海",
		"陕西",
		"甘肃",
		"云南",
		"贵州",
		"西藏自治区",
		"四川",
		"内蒙古自治区",
		"台湾",
		"海南",
		"福建",
		"吉林",
		"安徽",
		"香港特别行政区",
		"澳门特别行政区",
	};

	const char* shamNames[] ={
		"张三",
		"李四",
		"王五",
		"赵六",
		"三英战吕布",
		"三打白骨精",
		"西游记",
		"水浒传",
		"三国演义",
		"红楼梦",
		"金瓶梅",
		"真香定律"
	};

	const char* shamGroupNames[] ={
		"你们肯定没我帅",
		"小姐姐快来玩啊",
		"一看就是个瓜皮",
		"你愁啥",
		"瞅你咋地",
		"再瞅一个试试",
		"试试就试试",
		"西陆",
		"南陆",
		"解放台湾",
		"干倒美帝",
		"一个中国"
	};

	//添加联系人的假数据
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
				PERSONAL_INFO(strUUID, strName, "", strArea, "鬼知道", "鸟随鸾凤腾飞远，人随贤良品自高。")));
		}
	}

	//添加群的假数据
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

			m_mapGroups.insert(std::make_pair(strUUID, GROUP_INFO(strUUID, strName, "五一四天假")));
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
	//处理搜索索引

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

	int i = 0;
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
		int x = (int)_tcstol(sstr, &str1, 16);//十六进制 

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
		if (c==0) break;  //如果到字符串尾则说明该字符串没有中文字符
		if (c&0x80)       //如果字符高位为1且下一字符高位也是1则有中文字符
			if (*str & 0x80) 
				return TRUE;
	}
	return FALSE;
}