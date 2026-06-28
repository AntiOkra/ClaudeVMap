#include "StdAfx.h"
#include "AdxElement.h"
#include "GeneralFunction.h"


CAdxElementFace::CAdxElementFace(void)
{
}
 
CAdxElementFace::CAdxElementFace(int node_index[6], int& face_index)
{
	for (int i = 0; i < 6; i++) {
		m_NodeIndex[i] = node_index[i];
	}

	m_FrontElementIndex = face_index;
	m_BackElementIndex = -1;

	num3sort(node_index[0], node_index[1], node_index[2], m_NodeVertexSorted[0], m_NodeVertexSorted[1], m_NodeVertexSorted[2]);
}


CAdxElementFace::~CAdxElementFace(void)
{
}


CAdxElement::CAdxElement(void)
{
	m_ID = -1;
}


CAdxElement::~CAdxElement(void)
{
}

//
// 971               528               529               530               531               554               556               557               555               559               558               

//
int CAdxElement::Read(CStringArray& words)
{
	ASSERT(words.GetCount() >= 11);

	m_ID = _ttoi(words[0]);

	for (int i = 0; i < 10; i++) {
		m_NodeID[i] = _ttoi(words[i + 1]);
	}

	return 0;
}