#include "StdAfx.h"
#include "AdxNode.h"
#include "GeneralFunction.h"


CAdxNode::CAdxNode(void)
{
	m_ID = -1;
	m_Coord.Set(0.0, 0.0, 0.0);
	m_ForceVector.Set(0.0, 0.0, 0.0);
	m_Type = Undefined;
}


CAdxNode::~CAdxNode(void)
{
}

//
// 3                2.100002e+002    2.000002e+002    0.000000e+000   
//
int CAdxNode::Read(CStringArray& words)
{
	ASSERT(words.GetCount()>=4);

	m_ID = _ttoi(words[0]);

	m_Coord.x = _tstof(words[1]);
	m_Coord.y = _tstof(words[2]);
	m_Coord.z = _tstof(words[3]);

	return 0;
}

int CAdxNode::Copy(CAdxNode& n)
{
	m_ID = n.m_ID;
	m_Coord = n.m_Coord;
	m_ForceVector = n.m_ForceVector;

	return 0;
}

void CAdxNode::SetType(tNodeType node_type)
{
	if (m_Type == Undefined) {
		m_Type = node_type;
	}
	else {
		if (m_Type != node_type) {
			CString msg;
			msg.Format(_T("NodeType(Žå,•›)‚ª’è‚Ü‚è‚Ü‚¹‚ñ.Node_ID(%d)"), m_ID);
			AfxMessageBox(msg);
		}
	}
}