#pragma once

#include<vector>
#include"MzPoint.h"
#include"AdxNode.h"

class CArea
{
public:
	CArea(void);
	~CArea(void);

	std::vector<CAdxNode*> m_vNode;

	int  NearestNode(CMzPoint& p, CAdxNode** p_node, double& dst);
	bool IsEmpty();
};


class CAreaMap
{
public:
	CAreaMap(void);
	~CAreaMap(void);

	std::vector<CArea*> m_vArea;

	int min_index[3];
	int max_index[3];
	int ixno;
	int iyno;
	int izno;

	CMzPoint o;
	double area_size;

	int MakeAreaMap(std::vector<CAdxNode*>&	m_vNode, double pitch);
	int Init( CMzPoint& pmin, CMzPoint& pmax, double pitch );
	int GetIndex( CMzPoint& p, int& ix, int& iy, int& iz);
	//int GetArea( int& ix, int& iy, int& iz, CArea* p_area);
	int Entry(CAdxNode* n);
	int RemoveAll();

	int GetArrayIndex(int& ix, int& iy, int& iz, int& area_index);

};

