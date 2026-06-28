#pragma once

#include "MzPoint.h"
#include "AdxNode.h"
#include <vector>

typedef enum { Undefined, Node_1st, Node_2nd } tNodeType;		// 	未定義、主節点、中間節点

class CAdxNode
{
public:
	CAdxNode(void);
	~CAdxNode(void);

	int			m_ID;
	CMzPoint	m_Coord;
	CMzPoint	m_ForceVector;
	tNodeType	m_Type;

	std::vector<int>	m_ElementFaceIndex;	// このノードを頂点(最小インデクス)として持つ面インデクス

	int Read(CStringArray& words);
	int Copy(CAdxNode& n);
	void SetType(tNodeType node_type);

};

