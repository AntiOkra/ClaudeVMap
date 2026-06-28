#include "stdafx.h"
#include "Adx.h"
#include "GeneralFunction.h"
#include <iterator>
#include <regex>


CAdx::CAdx()
{
}


CAdx::~CAdx()
{
}

int IncludeFilePath( CString& parent_path, CString& include_file, CString& include_path )
{
	// parent folder
	int ipos = parent_path.ReverseFind('\\');
	CString parent_folder = parent_path.Left(ipos);

	// include_path
	include_path = parent_folder + "\\" + include_file;

	return 0;
}

//
// element_set = Body_2_PID1_e8  #Body_2
//
int CAdx::ElementSetInf(CString& nodeset_line, CString& name, int& id, CString& comment)
{
	id = 0;
	comment = "";

	CStringArray words;
	CString delimiters = _T(" =");
	SplitString(nodeset_line, delimiters, words);

	// Name
	name = words[1];

	// Comment
	if (words.GetSize() >= 3) {
		comment = words[2];
		if (!comment.IsEmpty()) {
			if (comment.Left(1) == "#") {
				comment = comment.Right(comment.GetLength() - 1);
			}
		}
	}

	// 最初の数字　無ければ 0
	std::wstring sname(name.GetBuffer());

	std::wregex   pattern(L"[0-9]+");
	std::wsmatch  match_results;

	if (std::regex_search(sname, match_results, pattern)) 
	{
			id = stoi(match_results.str());
	}

	/*
	int ipos_body = nodeset_line.Find(_T("Body_"), 0);
	if (ipos_body>=0) {
		int ipos = nodeset_line.Find(_T("_"), ipos_body+5);
		CString id_str = nodeset_line.Mid(ipos_body+5, ipos - (ipos_body+5));
		if (IsInteger(id_str)) {
			id = _ttoi(id_str);
		}
	}
	*/
	
	return 0;
}

int CAdx::Read(CString& fpath)
{
	CStdioFile		cFile;
	BOOL			ret=FALSE;
	CString			delimiters;
	CString			line;
	CStringArray	words;
	int				rc=0;
	bool			is_eof = false;
	bool			re_parse = false;
	std::unique_ptr<CAdxNodeSet>		crnt_node_set;
	std::unique_ptr<CAdxElementSet>	crnt_element_set;
	std::unique_ptr<CAdxElement>		crnt_element;
	std::unique_ptr<CAdxNode>			crnt_node;
	enum adx_state { None, Node, Element } crnt_state;

	// RemoveAll();

	ret = cFile.Open(fpath, CFile::modeRead | CFile::shareDenyNone);
	if (ret == FALSE) {
		return 1;
	}

	crnt_state = None;
	delimiters = " =:";
	is_eof = false;
	re_parse = false;

	while (!is_eof)	{

		// 読み込み 最終行に$EOFがあるとして処理
		if (re_parse) {
			re_parse = false;
		} else {
			if (cFile.ReadString(line) == FALSE) {
				is_eof = true;
				line = "$EOF";
			}
		}

		// 注釈スキップ
		if (line.Left(1) == "#") {
			continue;
		}

		// 空白行スキップ
		if (line.Trim() == "") {
			continue;
		}

		// 単語分割
		//SplitString(line, delimiters, words);
		//if (words.GetSize() == 0) {
		//	continue;
		//}

		switch (crnt_state) {

		case None:

			if (line.Left(1) == "$") {

				// 単語分割
				SplitString(line, delimiters, words);

				// タイプ別読み込み
				if (words[0] == "$Include") {
					CString include_path;
					IncludeFilePath(fpath, words[1], include_path);
					Read(include_path);
					continue;
				}
				else if (words[0] == "$Node") {
					crnt_state = Node;
					continue;
				}
				else if (words[0] == "$Element" && words[1] == "3DQuadraticTetrahedron") {
					crnt_state = Element;
					continue;
				}

			}

			break;

		case Node:

			// 単語分割
			SplitString(line, delimiters, words);

			// Nodeセクション終了
			if (line.Left(1) == "$") {
				if (crnt_node_set.get() != NULL) {
					m_vNodeSet.push_back(std::move(crnt_node_set));
				}
				crnt_node_set.reset(NULL);	
				crnt_state = None;
				re_parse = true;
				continue;
			}

			// node_set
			if (words[0] == "node_set") {
				crnt_node_set.reset(new CAdxNodeSet);
				crnt_node_set->m_NameAdx = words[1];
				continue;
			}

			// dimension
			if (words[0] == "dimension") {
				continue;
			}

			// index_format
			if (words[0] == "index_format") {
				continue;
			}

			// format
			if (words[0] == "format") {
				continue;
			}

			// node data
			crnt_node.reset(new CAdxNode);
			crnt_node->Read(words);
			m_vNode.push_back(std::move(crnt_node));

			if (crnt_node_set != NULL) {
				crnt_node_set->m_vNodeIndex.push_back(m_vNode.size() - 1);
			}

			break;

		case Element:

			// 単語分割
			SplitString(line, delimiters, words);

			// Elementセクション終了
			if (line.Left(1) == "$") {
				if (crnt_element_set.get() != NULL) {
					m_mElementsetNametoIndex[crnt_element_set->m_NameAdx] = m_vElementSet.size();
					m_vElementSet.push_back(std::move(crnt_element_set));
				}
				crnt_element_set.reset(NULL);
				crnt_state = None;
				re_parse = true;
				continue;
			}

			// element_set
			if (words[0] == "element_set") {
				crnt_element_set.reset(new CAdxElementSet);
				CString tmp_name;
				CString tmp_comment;
				int     tmp_id;
				ElementSetInf(line, tmp_name, tmp_id, tmp_comment);
				crnt_element_set->m_NameAdx = tmp_name;
				crnt_element_set->m_NameUser = tmp_comment;
				crnt_element_set->m_ID = tmp_id;
				continue;
			}

			// num_nodes_per_elementt
			if (words[0] == "num_nodes_per_element") {
				continue;
			}

			// index_format
			if (words[0] == "index_format") {
				continue;
			}

			// format
			if (words[0] == "format") {
				continue;
			}

			// element_data
			crnt_element.reset(new CAdxElement);
			crnt_element->Read(words);
			m_vElement.push_back(std::move(crnt_element));

			if (crnt_element_set != NULL) {
				crnt_element_set->m_vElementIndex.push_back(m_vElement.size() - 1);
			}

			break;
		}	
	}

	cFile.Close();

	return 0;
}

void CAdx::RemoveAll()
{
	m_vNodeSet.clear();
	m_vNode.clear();

	m_vElementSet.clear();
	m_vElement.clear();
	m_vElementFace.clear();

	m_mElementIDtoIndex.clear();
	m_mNodeIDtoIndex.clear();
}

//
int CAdx::Activate()
{
	// NodeMap作成
	for (int i = 0; i < m_vNode.size(); i++ ) {
		CAdxNode& n = *(m_vNode[i]);
		m_mNodeIDtoIndex[n.m_ID] = i;
	}

	// ElementMap作成
	for (int i = 0; i < m_vElement.size(); i++) {
		CAdxElement& e = *(m_vElement[i]);
		m_mElementIDtoIndex[e.m_ID] = i;
	}

	// Element 内 Node_Index set
	for (int i = 0; i < m_vElement.size(); i++) {
		CAdxElement& e = *(m_vElement[i]);

		for (int j = 0; j < 10; j++) {
			e.m_NodeIndex[j] = m_mNodeIDtoIndex[e.m_NodeID[j]];
		}

		for (int j = 0; j < 4; j++) {
			int index_node = e.m_NodeIndex[j];
			m_vNode[index_node]->SetType(Node_1st);
		}
		for (int j = 4; j < 10; j++) {
			int index_node = e.m_NodeIndex[j];
			m_vNode[index_node]->SetType(Node_2nd);
		}
		m_mElementIDtoIndex[e.m_ID] = i;
	}

	// ElementFace生成
	int n_index[6];
	int face_index = -1;
	for (int i = 0; i < m_vElement.size(); i++) {

		CAdxElement& e = *(m_vElement[i]);

		for (int j = 0; j < 4; j++) {

			for (int k = 0; k < 6; k++) {
				n_index[k] = e.m_NodeIndex[cElementFaceIndex[j][k]];
			}

			if ( FaceExist( n_index[0], n_index[1], n_index[2], face_index)) {
				CAdxElementFace& ef = *(m_vElementFace[face_index]);
				ef.m_BackElementIndex = i;
				e.m_FaceIndex[j] = face_index;
			}
			else {

				// Face生成
				std::unique_ptr<CAdxElementFace> new_face;
				new_face.reset(new CAdxElementFace(n_index, i));

				// Adxに登録
				m_vElementFace.push_back(std::move(new_face));

				// Elementに登録
				e.m_FaceIndex[j] = m_vElementFace.size() - 1;

				// Nodeに登録
				int v0, v1, v2;
				num3sort(n_index[0], n_index[1], n_index[2], v0, v1, v2);
				CAdxNode& n = *(m_vNode[v0]);
				n.m_ElementFaceIndex.push_back(m_vElementFace.size() - 1);
			}
		}
	}

	return 0;
}

// 同一の３ノードで構成される面が既にあるか確認する
bool CAdx::FaceExist(int& s, int& c, int& e, int& face_index)
{
	face_index = -1;

	int v0 = -1;
	int v1 = -1;
	int v2 = -1;

	num3sort(s, c, e, v0, v1, v2);

	// 最小インデクスのノードを頂点に持つ面とマッチング
	CAdxNode& dst_node = *(m_vNode[v0]);

	bool face_exist = false;

	for (int i = 0;i < dst_node.m_ElementFaceIndex.size(); i++) {
		CAdxElementFace& ef = *(m_vElementFace[dst_node.m_ElementFaceIndex[i]]);
		if (ef.m_NodeVertexSorted[0] == v0 && ef.m_NodeVertexSorted[1] == v1 && ef.m_NodeVertexSorted[2] == v2) {
			face_exist = true;
			face_index = dst_node.m_ElementFaceIndex[i];
			break;
		}
	}

	return face_exist;
}

int CAdx::SurfaceExtract(CString& es_name, std::vector<int>& vnode, std::vector<int>& vef)
{
	vnode.clear();
	vef.clear();

	// Search
	int index = -1;
	for (int i = 0; i < m_vElementSet.size(); i++) {
		CAdxElementSet& es = *(m_vElementSet[i]);
		if (es.m_NameAdx == es_name) {
			index = i;
			break;
		}
	}
	if (index == -1) {
		return 1;
	}

	// 表面Faceのみピックアップ
	CAdxElementSet& es = *(m_vElementSet[index]);
	for (int i = 0; i < es.m_vElementIndex.size(); i++) {
		int element_index = es.m_vElementIndex[i];
		CAdxElement& e = *(m_vElement[element_index]);
		for (int k = 0; k < 4; k++) {
			int face_index = e.m_FaceIndex[k];
			CAdxElementFace& ef = *(m_vElementFace[face_index]);
			if (ef.m_BackElementIndex == -1) {
				vef.push_back(face_index);
			}
		}
	}

	// 表面Faceのノードリスト作成
	std::map<int, int> map_node;
	for (int i = 0; i < vef.size(); i++) {
		CAdxElementFace& ef = *(m_vElementFace[vef[i]]);
		for (int j = 0; j < 6; j++) {
			map_node[ef.m_NodeIndex[j]] = vef[i];
		}
	}
	for (auto itr = map_node.begin(); itr != map_node.end(); ++itr) {
		vnode.push_back(itr->first);
	}

	return 0;
}

int CAdx::ExportObj(CString& opath, CStringArray& es_names)
{
	BOOL			ret;
	CStdioFile		oFile;
	CString			buf;

	// 表面face,node抽出
	std::vector<int> vnode;
	std::vector<int> vef;

	for (int i = 0; i < es_names.GetSize(); i++) {

		std::vector<int> tmp_vnode;
		std::vector<int> tmp_vef;

		CString name_element_set = es_names[i];
		SurfaceExtract(name_element_set, tmp_vnode, tmp_vef);

		std::copy(tmp_vnode.begin(), tmp_vnode.end(), std::back_inserter(vnode));
		std::copy(tmp_vef.begin(), tmp_vef.end(), std::back_inserter(vef));
	}

	// ノード Index　変換テーブル
	std::map<int, int> map_index;
	for (int i = 0; i < vnode.size(); i++) {
			map_index[vnode[i]] = i + 1;
	}

	// 出力
	ret = oFile.Open(opath, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone);
	if (ret == FALSE) {
		return 1;
	}

	// ノード出力
	for (int i = 0; i < vnode.size(); i++) {
		CAdxNode& n = *(m_vNode[vnode[i]]);
		buf.Format(_T("v %10.3f %10.3f %10.3f\n"), n.m_Coord.x, n.m_Coord.y, n.m_Coord.z);
		oFile.WriteString(buf);
	}

	// 面要素出力
	for (int i = 0; i < vef.size(); i++) {
		CAdxElementFace& ef = *(m_vElementFace[vef[i]]);
		buf.Format(_T("f %8d %8d %8d\n"), map_index[ef.m_NodeIndex[0]], map_index[ef.m_NodeIndex[1]], map_index[ef.m_NodeIndex[2]]);
		oFile.WriteString(buf);
	}

	oFile.Close();

	return 0;
}


int CAdx::ExportSurfaceNode(CString& opath, CStringArray& es_names)
{
	BOOL			ret;
	CStdioFile		oFile;
	CString			buf;

	// 出力ファイル
	ret = oFile.Open(opath, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone);
	if (ret == FALSE) {
		return 1;
	}

	for (int i = 0; i < es_names.GetSize(); i++) {

		CString name_element_set = es_names[i];

		// 表面face,node抽出
		std::vector<int> vnode;
		std::vector<int> vef;

		SurfaceExtract(name_element_set, vnode, vef);

		// ノード Index　変換テーブル
		std::map<int, int> map_index;
		for (int i = 0; i < vnode.size(); i++) {
			map_index[vnode[i]] = i;
		}
		vnode.clear();
		for (auto x : map_index) {
			vnode.push_back(x.first);
		}

		// ノード出力
		for (int i = 0; i < vnode.size(); i++) {
			CAdxNode& n = *(m_vNode[vnode[i]]);
			buf.Format(_T("%8d,%10.3f,%10.3f,%10.3f\n"),n.m_ID, n.m_Coord.x, n.m_Coord.y, n.m_Coord.z);
			oFile.WriteString(buf);
		}

	}

	oFile.Close();

	return 0;
}

bool CAdx::IsEmpty()
{
	if (m_vNode.size() > 0) {
		return false;
	}
	else {
		return true;
	}
}

int CAdx::ExtractSurfaceNode(CStringArray& es_names, CSurfaceNode& surface_node)
{
	std::map<int, int> map_node_index;

	for (int i = 0; i < es_names.GetSize(); i++) {

		CString name_element_set = es_names[i];

		// 表面face,node抽出
		std::vector<int> vnode;
		std::vector<int> vef;

		SurfaceExtract(name_element_set, vnode, vef);

		// ノード追加
		for (int i = 0; i < vnode.size(); i++) {
			map_node_index[vnode[i]] = i;
		}	
	}

	// 実体ノードClone
	for (auto x : map_node_index) {
		int adx_node_index = x.first;

		// 2次ノードのみ抽出
		//if (m_vNode[adx_node_index]->m_Type == Node_2nd) {
			CAdxNode *new_node = new CAdxNode;
			new_node->Copy(*(m_vNode[adx_node_index]));
			new_node->m_ForceVector.Set(0.0, 0.0, 0.0);
			surface_node.m_vNode.push_back(new_node);
		//}
	}

	return 0;
}

int CAdx::SortElementSet()
{
	return 0;
}

bool CompareAdxElementSet(CAdxElementSet* e1, CAdxElementSet* e2)
{
	if (e1->m_ID < e2->m_ID) {
		return true;
	}
	else {
		return false;
	}
}