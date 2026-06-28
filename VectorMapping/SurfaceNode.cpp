#include "stdafx.h"
#include "SurfaceNode.h"


CSurfaceNode::CSurfaceNode()
{
	m_MappedForce.Set(0.0, 0.0, 0.0);
	m_LossForce.Set(0.0, 0.0, 0.0);
}

CSurfaceNode::~CSurfaceNode()
{
	m_AreaMap.RemoveAll();
}

int CSurfaceNode::MakeAreaMap()
{
	std::vector<CAdxNode*> raw;
	raw.reserve(m_vNode.size());
	for (size_t i = 0; i < m_vNode.size(); i++) {
		raw.push_back(m_vNode[i].get());
	}
	m_AreaMap.MakeAreaMap(raw, 50.0);

	return 0;
}

int CSurfaceNode::Mapping(CNastran& nas, double upr_limit, CMzPoint ratio)
{
	CAdxNode* p_node=NULL;
	double distance;
	CString msg;

	for (int i = 0; i < nas.m_vNode.size(); i++) {
		CNastranNode& n = *(nas.m_vNode[i]);

		/* Check Code
		{
			CAdxNode *p_node_m;
			double   distance_m;
			int      rc_m;
			rc_m = NearestNode(n.m_Coord, upr_limit, &p_node_m, distance_m);

			CAdxNode *p_node_s;
			double   distance_s;
			int      rc_s;
			rc_s = NearestNode(n.m_Coord, upr_limit, &p_node_s, distance_s);

			if (rc_m == rc_s && distance_m == distance_s && p_node_m == p_node_s) {
				msg = _T("NearestNode Same Result\n");
				LogWrite(msg);
			}
			else {
				msg = _T("NearestNode Not Same Result\n");
				LogWrite(msg);
				msg.Format(_T("rc_m(%d) no(%16x) distance_m(%10.3f)"), rc_m, p_node_m, distance_m);
				LogWrite(msg);
				msg.Format(_T("rc_s(%d) no(%16x) distance_s(%10.3f)"), rc_s, p_node_s, distance_s);
				LogWrite(msg);
			}
		}
		*/
		
		CMzPoint map_force;

		map_force.x = n.m_ForceVector.x * ratio.x;
		map_force.y = n.m_ForceVector.y * ratio.y;
		map_force.z = n.m_ForceVector.z * ratio.z;

		if (NearestNode(n.m_Coord, upr_limit, &p_node, distance) == 0) {
			// msg.Format(_T("PAIR_EXIST Nastran_Node(%d) ADX_NODE(%d) DST(%10.3f)\n"), n.m_ID,p_node->m_ID,distance);
			// LogWrite(msg);
			p_node->m_ForceVector += map_force;
			m_MappedForce += map_force;
		}
		else {
			/*
			if ( p_node == NULL ) {
				msg.Format(_T("PAIR_NOT_EXIST Nastran_Node(%d) ADX_NODE(00000000) DST(%10.3f)\n"), n.m_ID, distance);
			}
			else {
				msg.Format(_T("PAIR_NOT_EXIST Nastran_Node(%d) ADX_NODE(%d) DST(%10.3f)\n"), n.m_ID, p_node->m_ID,distance);
			}
			LogWrite(msg);
			*/
			m_LossForce += map_force;
		}
	}

	return 0;
}

int CSurfaceNode::NearestNodeSimple(CMzPoint& p, double upr_limit, CAdxNode** pp_node, double& distance)
{
	*pp_node = NULL;
	distance = 1E100;

	double upr_limit2 = upr_limit*upr_limit;
	bool exist_node = false;
	double min_dst2 = 1E100;
	CAdxNode *p_nearest_node = NULL;

	for (int i = 0; i < m_vNode.size(); i++) {
		CAdxNode& n = *(m_vNode[i]);
		double tmp_dst2 = n.m_Coord.DistanceSquared(p);
		if (tmp_dst2 < min_dst2) {
			min_dst2 = tmp_dst2;
			p_nearest_node = m_vNode[i].get();
			exist_node = true;
		}
	}

	if (exist_node) {
		if (min_dst2 < upr_limit2) {
			*pp_node = p_nearest_node;
			distance = sqrt(min_dst2);
			return 0;
		}
	}
	
	return 1;
}


int CSurfaceNode::NearestNode(CMzPoint& p, double upr_limit, CAdxNode** pp_node, double& distance)
{
	*pp_node = NULL;
	distance = 1E100;

	// AreaIndex
	int ax, ay, az;
	m_AreaMap.GetIndex(p, ax, ay, az);

	// 前後を含む3index分を検索
	bool exist_node = false;
	double min_dst = 1E100;
	CAdxNode *p_nearest_node = NULL;

	int area_index;
	CAdxNode *tmp_node = NULL;
	double tmp_dst = 0.0;

	// Expand the cell search range so it always covers upr_limit,
	// independent of the AreaMap grid pitch.
	int cell_range = 1;
	if (m_AreaMap.area_size > 0.0) {
		cell_range = (int)ceil(upr_limit / m_AreaMap.area_size);
		if (cell_range < 1) cell_range = 1;
	}

	for (int ix = ax - cell_range; ix <= ax + cell_range; ix++) {
		for (int iy = ay - cell_range; iy <= ay + cell_range; iy++) {
			for (int iz = az - cell_range; iz <= az + cell_range; iz++) {

				if (m_AreaMap.GetArrayIndex(ix, iy, iz, area_index) != 0) {
					continue;
				}

				CArea& a = *(m_AreaMap.m_vArea[area_index]);
				if (!a.IsEmpty()) {
					if (a.NearestNode(p, &tmp_node, tmp_dst) == 0) {
						// Track the true nearest node; the distance limit is
						// applied once, after the whole neighbourhood is scanned.
						// (A nearer-but-out-of-limit node must not shadow a
						//  valid within-limit node.)
						if (tmp_dst < min_dst) {
							min_dst = tmp_dst;
							p_nearest_node = tmp_node;
						}
					}
				}

			}
		}
	}

	if (p_nearest_node != NULL && min_dst < upr_limit) {
		exist_node = true;
	}

	if (exist_node) {
		*pp_node = p_nearest_node;
		distance = min_dst;
		return 0;
	} else {
		*pp_node = NULL;
		distance = min_dst;
		return 1;
	}
}

int CSurfaceNode::ExportAdxForce(CString& opath, CString& process)
{
	BOOL			ret;
	CStdioFile		oFile;
	CString			buf;

	// 出力ファイル
	ret = oFile.Open(opath, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone);
	if (ret == FALSE) {
		return 1;
	}

	// 先頭部
	buf.Format(_T("########################################################################################\n"));
	oFile.WriteString(buf);
	buf.Format(_T("$Load\n"));
	oFile.WriteString(buf);
	buf.Format(_T("########################################################################################\n"));
	oFile.WriteString(buf);
	buf.Format(_T("process_id = %s\n"),process);
	oFile.WriteString(buf);

	// データ部
	for (int i = 0; i < m_vNode.size(); i++) {
		CAdxNode& n = *(m_vNode[i]);

		if (n.m_ForceVector.x != 0.0 || n.m_ForceVector.y != 0.0 || n.m_ForceVector.z != 0.0) {
			buf.Format(_T("%8d 0 %12.6f\n"), n.m_ID, n.m_ForceVector.x);
			oFile.WriteString(buf);
			buf.Format(_T("%8d 1 %12.6f\n"), n.m_ID, n.m_ForceVector.y);
			oFile.WriteString(buf);
			buf.Format(_T("%8d 2 %12.6f\n"), n.m_ID, n.m_ForceVector.z);
			oFile.WriteString(buf);
		}
	}

	oFile.Close();

	return 0;
}

void CSurfaceNode::LogWrite(CString& msg)
{
	if (m_LogFile != NULL) {
		m_LogFile->WriteString(msg);
	}
}


int CSurfaceNode::DumpNode(CString& fpath)
{
	BOOL			ret;
	CStdioFile		oFile;
	CString			buf;

	ret = oFile.Open(fpath, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone);
	if (ret == FALSE) {
		return 1;
	}

	buf.Format(_T("NODE_ID,x,y,z,Force_x,Force_y,Force_z\n"));
	oFile.WriteString(buf);
	
	for (int i = 0; i < m_vNode.size(); i++) {
		CAdxNode& n = *(m_vNode[i]);
		buf.Format(_T("%8d,%12.6f,%12.6f,%12.6f,%12.6f,%12.6f,%12.6f\n"), n.m_ID, n.m_Coord.x, n.m_Coord.y, n.m_Coord.z, n.m_ForceVector.x, n.m_ForceVector.y, n.m_ForceVector.z);
		oFile.WriteString(buf);
	}

	oFile.Close();

	return 0;
}


int CSurfaceNode::Dump(CString& fpath)
{
	BOOL			ret;
	CStdioFile		oFile;
	CString			buf;

	ret = oFile.Open(fpath, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone);
	if (ret == FALSE) {
		return 1;
	}

	int array_index=0;

	for ( int i=m_AreaMap.min_index[0]; i<= m_AreaMap.max_index[0]; i++ ) {
		for (int j = m_AreaMap.min_index[1]; j <= m_AreaMap.max_index[1]; j++) {
			for (int k = m_AreaMap.min_index[2]; k <= m_AreaMap.max_index[2]; k++) {

				buf.Format(_T("------ AREA X:%d,Y:%d,Z:%d ----- "), i, j, k);
				oFile.WriteString(buf);

				if (m_AreaMap.GetArrayIndex(i, j, k, array_index) == 0) {

					buf.Format(_T("GetArrayIndex=%d\n"),array_index);
					oFile.WriteString(buf);

					CArea& a = *(m_AreaMap.m_vArea[array_index]);
					for (int l = 0; l < a.m_vNode.size(); l++) {
						CAdxNode&n = *(a.m_vNode[l]);
						buf.Format(_T("ID(%d) X:%10.3f, Y:%10.3f, Z:%10.3f"), n.m_ID, n.m_Coord.x, n.m_Coord.y, n.m_Coord.z);
						oFile.WriteString(buf);
					}

				} else {
					buf.Format(_T("GetArrayIndex ERROR"));
					oFile.WriteString(buf);
				}

			}
		}
	}

	oFile.Close();

	return 0;
}

void CSurfaceNode::RemoveAll()
{
	m_vNode.clear();

	m_AreaMap.RemoveAll();

	m_MappedForce.Set(0.0, 0.0, 0.0);
	m_LossForce.Set(0.0, 0.0, 0.0);

}