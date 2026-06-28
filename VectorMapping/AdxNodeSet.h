#pragma once

#include"AdxNode.h"

#include<vector>
#include<map>

class CAdxNodeSet
{
public:
	CAdxNodeSet(void);
	~CAdxNodeSet(void);

	CString m_NameAdx;	// Adxデータ上の名称
	CString m_NameUser;	// ユーザー設定名称

	std::vector<int>	m_vNodeIndex;
};

