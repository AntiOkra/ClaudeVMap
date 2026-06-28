#pragma once

// 4 面体2次要素の面番号と対応する節点番号
const int cElementFaceIndex[4][6] = { {1,2,3,7,8,9},{0,3,2,6,8,5},{0,1,3,4,9,6},{0,2,1,5,7,4} };

class CAdxElementFace
{
public:
	CAdxElementFace(void);
	CAdxElementFace(int node_index[6], int& face_index);
	~CAdxElementFace(void);

	int m_NodeIndex[6];

	int m_FrontElementIndex;	// この面が表側になる要素
	int m_BackElementIndex;		// この面が裏側になる要素
	int m_NodeVertexSorted[3];	// 昇順にソートされた頂点ノードインデックス
};


class CAdxElement
{
public:
	CAdxElement(void);
	~CAdxElement(void);

	int m_ID;
	int m_NodeID[10];
	int m_NodeIndex[10];
	int m_FaceIndex[4];

	int Read(CStringArray& words);
};

