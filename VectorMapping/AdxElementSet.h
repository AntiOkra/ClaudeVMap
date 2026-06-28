#pragma once

#include <vector>
#include <map>
#include "AdxElement.h"


//##TSV BODY  2 "Body_2"
//########################################################################################
//$Element  3DQuadraticTetrahedron
//########################################################################################
//element_set = Body_2_PID1_e8  #Body_2
//971               528               529               530               531               554               556               557               555               559               558               
//972               528               530               532               533               556               561               562               560               564               563               

class CAdxElementSet
{
public:
	CAdxElementSet(void);
	~CAdxElementSet(void);

	int     m_ID;		// ID(TSV_PRE)
	CString m_NameAdx;	// Adxデータ上の名称
	CString m_NameUser;	// ユーザー設定名称

	std::vector<int>	m_vElementIndex;

	//bool Compare(const CAdxElementSet* a, const CAdxElementSet* b) { if (a->m_ID < b->m_ID) { return true; } else { return false; } }
};

