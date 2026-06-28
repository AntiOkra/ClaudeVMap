#include "StdAfx.h"
#include "AreaMap.h"

CArea::CArea(void)
{
}

CArea::~CArea(void)
{
}

bool CArea::IsEmpty()
{
	if (m_vNode.size() > 0) {
		return false;
	}
	else {
		return true;
	}
}

CAreaMap::CAreaMap(void)
{
}


CAreaMap::~CAreaMap(void)
{
}

int CAreaMap::MakeAreaMap(std::vector<CAdxNode*>& m_vNode, double pitch)
{
	// min, max Šm”F
	CMzPoint pmin;
	CMzPoint pmax;

	pmin.Set(1E100, 1E100, 1E100);
	pmax.Set(-1E100, -1E100, -1E100);

	for (int i = 0; i < m_vNode.size(); i++) {
		CAdxNode& n = *(m_vNode[i]);
		if (n.m_Coord.x < pmin.x) pmin.x = n.m_Coord.x;
		if (n.m_Coord.y < pmin.y) pmin.y = n.m_Coord.y;
		if (n.m_Coord.z < pmin.z) pmin.z = n.m_Coord.z;

		if (n.m_Coord.x > pmax.x) pmax.x = n.m_Coord.x;
		if (n.m_Coord.y > pmax.y) pmax.y = n.m_Coord.y;
		if (n.m_Coord.z > pmax.z) pmax.z = n.m_Coord.z;
	}

	Init(pmin, pmax, pitch);

	// Entry
	for (int i = 0; i < m_vNode.size(); i++) {
		Entry(m_vNode[i]);
	}

	return 0;
}


int CAreaMap::Init( CMzPoint& pmin, CMzPoint& pmax, double pitch )
{
	int rc = 0;

	RemoveAll();

	area_size = pitch;

	rc = GetIndex( pmin, min_index[0], min_index[1], min_index[2] );
	rc = GetIndex( pmax, max_index[0], max_index[1], max_index[2] );

	CArea *new_area;

	for (int iz=min_index[2]; iz<=max_index[2]; iz++ ) {
		for (int iy=min_index[1]; iy<=max_index[1]; iy++ ) {
			for (int ix=min_index[0]; ix<=max_index[0]; ix++ ) {
				new_area = new CArea;			
				m_vArea.push_back(new_area);
			}
		}
	}

	ixno = max_index[0] - min_index[0] + 1;
	iyno = max_index[1] - min_index[1] + 1;
	izno = max_index[2] - min_index[2] + 1;

	return 0;
}

int CAreaMap::GetIndex( CMzPoint& p, int& ix, int& iy, int& iz)
{
	ix = int(floor(p.x/area_size));
	iy = int(floor(p.y/area_size));
	iz = int(floor(p.z/area_size));

	return 0;
}

/*
int CAreaMap::GetArea( int& ix, int& iy, int& iz, CArea* p_area)
{
	int area_index;

	if (GetArrayIndex( ix, iy, iz, area_index )==0 ) {
		p_area = m_vArea[area_index];
		return 0;
	} else {
		p_area = NULL;
		return 1;
	}
}
*/

int CAreaMap::GetArrayIndex( int& ix, int& iy, int& iz, int& area_index )
{
	area_index = -1;

	if ( ix<min_index[0] || ix>max_index[0] ) {
		return 1;
	}

	if ( iy<min_index[1] || iy>max_index[1] ) {
		return 1;
	}

	if ( iz<min_index[2] || iz>max_index[2] ) {
		return 1;
	}

	area_index = ( iz - min_index[2] ) * ixno * iyno + ( iy - min_index[1] ) * ixno + ( ix - min_index[0] );

	return 0;
}

int CAreaMap::RemoveAll()
{
	for ( auto itr = m_vArea.begin(); itr != m_vArea.end(); ++itr) {
		delete *itr;
	}
	m_vArea.clear();

	ixno = 0;
	iyno = 0;
	izno = 0;
	
	for ( int i=0; i<3; i++ ) {
		min_index[i] = 0;
		max_index[i] = 0;
	}

	return 0;
}

int CAreaMap::Entry(CAdxNode* n)
{
	int ix, iy, iz;

	if (GetIndex(n->m_Coord, ix, iy, iz) != 0) {
		return 1;
	}

	int area_index;
	if (GetArrayIndex(ix, iy, iz, area_index) != 0) {
		return 1;
	}

	if (area_index < 0 || area_index >= m_vArea.size() ) {
		return 1;
	}

	CArea& area = *(m_vArea[area_index]);

	area.m_vNode.push_back(n);

	return 0;
}

int CArea::NearestNode(CMzPoint& p, CAdxNode** p_node, double& dst)
{
	*p_node = NULL;
	dst = 1E100;

	CAdxNode* p_tmp_node = NULL;
	double min_dst2 = 1E100;

	for (int i = 0; i < m_vNode.size(); i++) {
		CAdxNode& n = *(m_vNode[i]);
		double tmp_dst2 = p.DistanceSquared(n.m_Coord);
		if (tmp_dst2 < min_dst2) {
			p_tmp_node = m_vNode[i];
			min_dst2 = tmp_dst2;
		}
	}

	if (p_tmp_node != NULL) {
		*p_node = p_tmp_node;
		dst = sqrt(min_dst2);
		return 0;
	} else {
		dst = 1E10;
		return 1;
	}
}