#pragma once

#include<vector>
#include"AdxNode.h"
#include"AreaMap.h"
#include"MzPoint.h"
#include"CNastran.h"

class CSurfaceNode
{
public:
	CSurfaceNode();
	~CSurfaceNode();

	std::vector<CAdxNode*>	m_vNode;
	CAreaMap				m_AreaMap;
	CMzPoint				m_MappedForce;
	CMzPoint				m_LossForce;

	CStdioFile*				m_LogFile;

	int Mapping(CNastran& nas,double upr_liimt,CMzPoint ratio);
	int NearestNode(CMzPoint& p, double upr_limit, CAdxNode** p_node, double& distance );
	int NearestNodeSimple(CMzPoint& p, double upr_limit, CAdxNode** p_node, double& distance);
	int MakeAreaMap();
	int ExportAdxForce(CString& fpath, CString& process);
	void LogWrite(CString &msg);
	int DumpNode(CString& fpath);
	int Dump(CString& fpath);
	void RemoveAll();
};

