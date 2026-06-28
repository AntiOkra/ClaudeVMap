#pragma once

#include<vector>
//#include<map>
#include<unordered_map>
#include<memory>

#include "AdxNode.h"
#include "AdxNodeSet.h"
#include "AdxElement.h"
#include "AdxElementSet.h"

#include "AreaMap.h"
#include "SurfaceNode.h"

class CAdx
{
public:
	CAdx();
	~CAdx();

	std::vector<std::unique_ptr<CAdxNode>>			m_vNode;
	std::vector<std::unique_ptr<CAdxNodeSet>>		m_vNodeSet;

	std::vector<std::unique_ptr<CAdxElement>>		m_vElement;
	std::vector<std::unique_ptr<CAdxElementFace>>	m_vElementFace;
	std::vector<std::unique_ptr<CAdxElementSet>>	m_vElementSet;

	std::unordered_map<int, int>	m_mNodeIDtoIndex;
	std::unordered_map<int, int>	m_mElementIDtoIndex;
	std::map<CString, int>			m_mElementsetNametoIndex;

	void RemoveAll();
	int Read(CString& fpath);
	int Activate();

	int ExportObj(CString& fpath, CStringArray& es_names);
	int ExportSurfaceNode(CString& fpath, CStringArray& es_names);

	bool IsEmpty();

	bool FaceExist(int& s, int& c, int& e, int& face_index);
	int  SurfaceExtract(CString& es_name, std::vector<int>& vnode, std::vector<int>& vef);
	int  ExtractSurfaceNode(CStringArray& es_names, CSurfaceNode& surface_node);
	int  ElementSetInf(CString& nodeset_line, CString& name, int& id, CString& comment);
	int  SortElementSet();
};

bool CompareAdxElementSet(CAdxElementSet* e1, CAdxElementSet* e2);

