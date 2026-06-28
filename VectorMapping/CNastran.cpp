#include "stdafx.h"
#include "CNastran.h"
#include "GeneralFunction.h"

CNastranNode::CNastranNode()
{
	m_Area = 0.0;
}

CNastranNode::~CNastranNode()
{
}

//           1         2         3         4         5         6         7         8
// 012345678901234567890123456789012345678901234567890123456789012345678901234567890
// GRID*           16282904                -0.761911865E+03 0.278623383E+03F      1
// *      1 0.951755142E+01
int CNastranNode::Read(CString& line1, CString& line2)
{
	CString sid, scoord_x, scoord_y, scoord_z;
	
	m_ID = -1;

	sid = line1.Mid(16, 8);
	scoord_x = line1.Mid(40, 16);
	scoord_y = line1.Mid(56, 16);
	scoord_z = line2.Mid(8, 16);

	m_ID = _ttoi(sid);
	m_Coord.x = _ttof(scoord_x);
	m_Coord.y = _ttof(scoord_y);
	m_Coord.z = _ttof(scoord_z);

	return 0;
}

CNastranElement::CNastranElement()
{
	m_Area = 0.0;
}

CNastranElement::~CNastranElement()
{
}

//           1         2         3         4         5         6         7         8
// 012345678901234567890123456789012345678901234567890123456789012345678901234567890
// CTRIA3  17245527       2163397611632467916324683
// CQUAD4  17591331       316290559162905581629106316291075
// CBEAM        706       1   88558   88559
int CNastranElement::Read(const CString& line)
{
	CString stype, sid;
	CString snode_id[4];
	
	m_ID = -1;

	stype = line.Left(6);
	stype.Trim();
	sid = line.Mid(8, 8);

	if ( stype=="CTRIA3" ) {
		m_type = CTRIA3;
		snode_id[0] = line.Mid(24,8);
		snode_id[1] = line.Mid(32,8);
		snode_id[2] = line.Mid(40,8);
		snode_id[3] = "-1";
	} else if ( stype=="CQUAD4" ) {
		m_type = CQUAD4;
		snode_id[0] = line.Mid(24,8);
		snode_id[1] = line.Mid(32,8);
		snode_id[2] = line.Mid(40,8);
		snode_id[3] = line.Mid(48,8);
	} else if ( stype=="CBEAM" ) {
		m_type = CBEAM;
		snode_id[0] = line.Mid(24,8);
		snode_id[1] = line.Mid(32,8);
		snode_id[2] = "-1";
		snode_id[3] = "-1";
	} else {
		return 1;
	}

	m_ID = _ttoi(sid);
	m_NodeID[0] = _ttoi(snode_id[0]);
	m_NodeID[1] = _ttoi(snode_id[1]);
	m_NodeID[2] = _ttoi(snode_id[2]);
	m_NodeID[3] = _ttoi(snode_id[3]);

	return 0;
}

CNastran::CNastran()
{
	m_LogFile = NULL;
}

CNastran::~CNastran()
{
}

void CNastran::RemoveAll()
{
	m_vNode.clear();
	m_vElement.clear();
	m_mNodeIDtoIndex.clear();
	m_mElementIDtoIndex.clear();
	m_LogFile = NULL;
}

void CNastran::LogWrite( CString& msg )
{
	if ( m_LogFile != NULL ) {
		m_LogFile->WriteString( msg );
	}
}

//
// 12345678901234567890123456789012345678901234567890123456789012345678901234567890
// GRID*           16282904                -0.761911865E+03 0.278623383E+03F      1
// * 1 0.636766167E+02
// CTRIA3  17245527       2163397611632467916324683
// CQUAD4  17591331       316290559162905581629106316291075
// 12345678901234567890123456789012345678901234567890123456789012345678901234567890
int CNastran::ReadNastranFile(const CString& fpath)
{

	CStdioFile		cFile;
	BOOL			ret = FALSE;
	CString			delimiters;
	CString			line,line2;
	CString			msg;
	CStringArray	words;
	int				rc = 0;

	std::unique_ptr<CNastranNode>	crnt_node;
	std::unique_ptr<CNastranElement> crnt_element;

	RemoveAll();

	ret = cFile.Open(fpath, CFile::modeRead | CFile::shareDenyNone);
	if (ret == FALSE) {
		msg.Format(_T("ファイルがOpenできません(%s)\n"),fpath );
		LogWrite( msg );
		return 1;
	}

	delimiters = " =:";

	int line_count = 0;

	while (cFile.ReadString(line)) {

		line_count++;

		if (line.Left(5) == "GRID*") {

			if (!cFile.ReadString(line2)) {
				msg.Format(_T(" #ERROR フォーマット異常あり(%d)行目\n"), line_count);
				LogWrite(msg);
				return 1;
			}
			line_count++;

			crnt_node.reset(new CNastranNode);

			if (crnt_node->Read(line, line2) != 0) {
				msg.Format(_T(" #ERROR フォーマット異常あり(%d)行目\n"), line_count);
				LogWrite(msg);
				return 1;
			}

			m_vNode.push_back(std::move(crnt_node));

		}
		else if (line.Left(6) == "CTRIA3") {

			crnt_element.reset(new CNastranElement);

			if (crnt_element->Read(line) != 0) {
				msg.Format(_T(" #ERROR フォーマット異常あり(%d)行目\n"), line_count);
				LogWrite(msg);
				return 1;
			}

			m_vElement.push_back(std::move(crnt_element));

		}
		else if (line.Left(6) == "CQUAD4") {

			crnt_element.reset(new CNastranElement);

			if (crnt_element->Read(line) != 0) {
				msg.Format(_T(" #ERROR フォーマット異常あり(%d)行目\n"), line_count);
				LogWrite(msg);
				return 1;
			}

			m_vElement.push_back(std::move(crnt_element));
		}
		else if (line.Left(5) == "CBEAM") {

			crnt_element.reset(new CNastranElement);

			if (crnt_element->Read(line) != 0) {
				msg.Format(_T(" #ERROR フォーマット異常あり(%d)行目\n"), line_count);
				LogWrite(msg);
				return 1;
			}

			m_vElement.push_back(std::move(crnt_element));
		}
	}

	cFile.Close();

	if (Indexing() != 0) {
		msg.Format(_T(" #ERROR インデックスを作成できません\n"), line_count);
		LogWrite(msg);
		return 1;
	}

	return 0;
}

int CNastran::Indexing()
{
	m_mNodeIDtoIndex.clear();
	for ( int i=0; i<m_vNode.size(); i++ ) {
		CNastranNode& n = *(m_vNode[i]);
		m_mNodeIDtoIndex[n.m_ID] = i;
	}

	m_mElementIDtoIndex.clear();
	for ( int i=0; i<m_vElement.size(); i++ ) {
		CNastranElement& e = *(m_vElement[i]);
		m_mElementIDtoIndex[e.m_ID] = i;
	}

	for (int i = 0; i<m_vElement.size(); i++) {
		CNastranElement& e = *(m_vElement[i]);
		int node_cnt = 0;
		if (e.m_type == CTRIA3) {
			node_cnt = 3;
		}
		else {
			node_cnt = 4;
		}
		for (int j = 0; j < node_cnt; j++) {
			e.m_NodeIndex[j] = m_mNodeIDtoIndex[e.m_NodeID[j]];
		}
	}

	return 0;
}

int CNastran::ReadNormalVectorFile(const CString& fpath)
{

	CStdioFile		cFile;
	BOOL			ret = FALSE;
	CString			delimiters;
	CString			line;
	CString			msg;
	CStringArray	words;

	//RemoveAll();

	ret = cFile.Open(fpath, CFile::modeRead | CFile::shareDenyNone);
	if (ret == FALSE) {
		msg.Format(_T("ファイルがOpenできません(%s)\n"),fpath );
		LogWrite( msg );
		return 1;
	}

	int line_count = 0;

	// 9行スキップ
	for ( int i=0; i<9; i++ ) {
		line_count++;
		if ( ! cFile.ReadString(line) ) {
			msg.Format(_T(" #ERROR フォーマット異常あり(%d)行目\n"), line_count);
			LogWrite(msg);
			return 1;
		}
	}

	delimiters = " ";

	// データ
	while (cFile.ReadString(line)) {

		line_count++;

		SplitString( line, delimiters, words );

		if ( words.GetCount() == 4 ) {

			int crnt_id = _ttoi(words[0]);

			auto it =  m_mNodeIDtoIndex.find(crnt_id);
			if (it == m_mNodeIDtoIndex.end() ) {
				msg.Format(_T(" #ERROR ノードIDが見つかりません(%d)行目. ID=(%d)\n"), line_count,crnt_id);
				LogWrite(msg);
				return 1;
			}

			int index = (*it).second;
			CNastranNode& n = *(m_vNode[index]);

			n.m_NormalPressureVector.x = _ttof(words[1]);
			n.m_NormalPressureVector.y = _ttof(words[2]);
			n.m_NormalPressureVector.z = _ttof(words[3]);
		}
	}

	cFile.Close();

	return 0;
}

int CNastran::ReadTangentVectorFile(const CString& fpath)
{

	CStdioFile		cFile;
	BOOL			ret = FALSE;
	CString			delimiters;
	CString			line;
	CString			msg;
	CStringArray	words;

	//RemoveAll();

	ret = cFile.Open(fpath, CFile::modeRead | CFile::shareDenyNone);
	if (ret == FALSE) {
		msg.Format(_T("ファイルがOpenできません(%s)\n"),fpath );
		LogWrite( msg );
		return 1;
	}

	int line_count = 0;

	// 9行スキップ
	for ( int i=0; i<9; i++ ) {
		line_count++;
		if ( ! cFile.ReadString(line) ) {
			msg.Format(_T(" #ERROR フォーマット異常あり(%d)行目\n"), line_count);
			LogWrite(msg);
			return 1;
		}
	}

	delimiters = " ";

	// データ
	while (cFile.ReadString(line)) {

		line_count++;

		SplitString( line, delimiters, words );

		if ( words.GetCount() ==4 ) {

			int crnt_id = _ttoi(words[0]);

			auto it =  m_mNodeIDtoIndex.find(crnt_id);
			if (it == m_mNodeIDtoIndex.end() ) {
				msg.Format(_T(" #ERROR ノードIDが見つかりません(%d)行目. ID=(%d)\n"), line_count,crnt_id);
				LogWrite(msg);
				return 1;
			}

			int index = (*it).second;
			CNastranNode& n = *(m_vNode[index]);

			n.m_TangentPressureVector.x = _ttof(words[1]);
			n.m_TangentPressureVector.y = _ttof(words[2]);
			n.m_TangentPressureVector.z = _ttof(words[3]);
		}
	}

	cFile.Close();

	return 0;
}

// ノード毎の荷重ベクトルを求める
int CNastran::ForceCalc()
{

	// ノードの担当面積を求める
	for (int i=0; i<m_vElement.size(); i++ ) {

		CNastranElement& e = *(m_vElement[i]);

		// 要素面積を求める

		if ( e.m_type == CTRIA3 ) {

			CNastranNode& n0 = *(m_vNode[e.m_NodeIndex[0]]);
			CNastranNode& n1 = *(m_vNode[e.m_NodeIndex[1]]);
			CNastranNode& n2 = *(m_vNode[e.m_NodeIndex[2]]);

			e.m_Area = AreaTriangle(n0.m_Coord, n1.m_Coord, n2.m_Coord);

			double node_area = e.m_Area / 3.0;
			n0.m_Area += node_area;
			n1.m_Area += node_area;
			n2.m_Area += node_area;

		} else if ( e.m_type == CQUAD4 ) {

			CNastranNode& n0 = *(m_vNode[e.m_NodeIndex[0]]);
			CNastranNode& n1 = *(m_vNode[e.m_NodeIndex[1]]);
			CNastranNode& n2 = *(m_vNode[e.m_NodeIndex[2]]);
			CNastranNode& n3 = *(m_vNode[e.m_NodeIndex[3]]);

			e.m_Area = AreaQuad(n0.m_Coord, n1.m_Coord, n2.m_Coord, n3.m_Coord);

			double node_area = e.m_Area / 4.0;
			n0.m_Area += node_area;
			n1.m_Area += node_area;
			n2.m_Area += node_area;
			n3.m_Area += node_area;

		} else if ( e.m_type == CBEAM ) {

			CNastranNode& n0 = *(m_vNode[e.m_NodeIndex[0]]);
			CNastranNode& n1 = *(m_vNode[e.m_NodeIndex[1]]);

			double seg_length = 0.0;
			seg_length = n0.m_Coord.Distance(n1.m_Coord);

			e.m_Area = seg_length;

			double node_area = e.m_Area / 2.0;
			n0.m_Area += node_area;
			n1.m_Area += node_area;

		}
	}

	// 荷重を計算
	for ( int i=0; i<m_vNode.size(); i++ ) {
		CNastranNode& n = *(m_vNode[i]);
		n.m_PressureVector = n.m_NormalPressureVector + n.m_TangentPressureVector;
		n.m_ForceVector = n.m_PressureVector * n.m_Area * 1000.0;	// 単位:(N)
	}

	return 0;
}

int CNastran::Dump_N(CString& fpath)
{
	BOOL			ret;
	CStdioFile		oFile;
	CString			buf;

	// 出力
	ret = oFile.Open(fpath, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone);
	if (ret == FALSE) {
		return 1;
	}

	// 要素情報出力
	/*
	for (int i = 0; i < m_vElement.size(); i++) {
		CNastranElement& e = *(m_vElement[i]);
		buf.Format(_T("E ID(%8d) ,%12.7f\n"), e.m_ID, e.m_Area);
		oFile.WriteString(buf);
	}
	*/

	// ノード・ヘッダ行
	buf.Format(_T("NODE_ID,X,Y,Z,NormalPressureX,NormalPressureY,NormalPressureZ,TangentPressureX,TangentPressureY,TangentPressureZ,Area,ForceX,ForceY,ForceZ\n"));
	oFile.WriteString(buf);

	// ノード情報出力
	for (int i = 0; i < m_vNode.size(); i++) {
		CNastranNode& n = *(m_vNode[i]);
		buf.Format(_T("%8d,%12.7f,%12.7f,%12.7f,%12.7f,%12.7f,%12.7f,%12.7f,%12.7f,%12.7f,%12.7f,%12.7f,%12.7f,%12.7f\n"), 
			n.m_ID,
			n.m_Coord.x, n.m_Coord.y, n.m_Coord.z,
			n.m_NormalPressureVector.x, n.m_NormalPressureVector.y, n.m_NormalPressureVector.z, 
			n.m_TangentPressureVector.x, n.m_TangentPressureVector.y, n.m_TangentPressureVector.z,
			n.m_Area,
			n.m_ForceVector.x, n.m_ForceVector.y, n.m_ForceVector.z
			);
		oFile.WriteString(buf);
	}

	oFile.Close();

	return 0;
}

int CNastran::Dump_E(CString& fpath)
{
	BOOL			ret;
	CStdioFile		oFile;
	CString			buf;

	// 出力
	ret = oFile.Open(fpath, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone);
	if (ret == FALSE) {
		return 1;
	}

	// 要素・ヘッダ行
	buf.Format(_T("ELEMENT_ID,Area\n"));
	oFile.WriteString(buf);

	// 要素情報出力
	for (int i = 0; i < m_vElement.size(); i++) {
		CNastranElement& e = *(m_vElement[i]);
		buf.Format(_T("%8d,%12.7f\n"), e.m_ID, e.m_Area);
		oFile.WriteString(buf);
	}

	/*
	// ノード・ヘッダ行
	buf.Format(_T("NODE_ID,X,Y,Z,NormalPressureX,NormalPressureY,NormalPressureZ,TangentPressureX,TangentPressureY,TangentPressureZ,Area,ForceX,ForceY,ForceZ\n"));
	oFile.WriteString(buf);

	// ノード情報出力
	for (int i = 0; i < m_vNode.size(); i++) {
		CNastranNode& n = *(m_vNode[i]);
		buf.Format(_T("NODE_ID(%8d),%12.7f,%12.7f,%12.7f,%12.7f,%12.7f,%12.7f,%12.7f,%12.7f,%12.7f,%12.7f,%12.7f,%12.7f,%12.7f\n"),
			n.m_ID,
			n.m_Coord.x, n.m_Coord.y, n.m_Coord.z,
			n.m_NormalPressureVector.x, n.m_NormalPressureVector.y, n.m_NormalPressureVector.z,
			n.m_TangentPressureVector.x, n.m_TangentPressureVector.y, n.m_TangentPressureVector.z,
			n.m_Area,
			n.m_ForceVector.x, n.m_ForceVector.y, n.m_ForceVector.z
		);
		oFile.WriteString(buf);
	}
	*/

	oFile.Close();

	return 0;
}

bool CNastran::IsEmpty()
{
	if (m_vNode.size() > 0) {
		return false;
	}
	else {
		return true;
	}
}

int  CNastran::GetTotalForce(CMzPoint& force)
{
	CMzPoint total_force(0.0, 0.0, 0.0);

	for (int i = 0; i < m_vNode.size(); i++) {
		CNastranNode& n = *(m_vNode[i]);
		total_force += n.m_ForceVector;
	}

	force = total_force;

	return 0;
}